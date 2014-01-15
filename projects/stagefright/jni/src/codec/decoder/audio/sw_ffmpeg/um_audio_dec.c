#include "um.h"
#include "um_mm.h"
#include "um_sys_log.h"
#include "um_load_ffmpeg.h"
#include <assert.h>
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"

UMSint is_register_decoder;

static AVPacket mAvpkt;
typedef struct _FFMpegADecPriv
{
	AVCodec *codec;
	AVCodecContext *codecCtx;
	UMSint8 outBuf[AVCODEC_MAX_AUDIO_FRAME_SIZE];
	AVFrame *decodedFrame;
} FFMpegADecPriv;

/************************************************************************/
/* audio decoder                                                        */
/************************************************************************/
static UMSint ffmpeg_adec_init(UM_AudioDecoderCtx* thiz, enum CodecID audioCodecId)
{
	FFMpegADecPriv* priv = (FFMpegADecPriv*)thiz->priv;

	priv->codec = um_avcodec_find_decoder(audioCodecId);
	if(priv->codec == NULL)
	{
		UMLOG_ERR("================ ffmpeg_adec_init um_avcodec_find_decoder fail");
		return -1;
	}
	priv->codecCtx = um_avcodec_alloc_context3(priv->codec);
	UMLOG_INFO("ffmpeg_adec_init, avcodec_alloc_context3(), priv->codecCtx=%p", priv->codecCtx);
	if(um_avcodec_open2(priv->codecCtx, priv->codec, NULL) < 0) 
	{
		UMLOG_ERR("================ ffmpeg_adec_init um_avcodec_open2 fail");
		return -1;		
	}
	
	UMLOG_INFO("ffmpeg_adec_init, avcodec_open2(), priv->codecCtx=%p", priv->codecCtx);
	priv->decodedFrame = um_avcodec_alloc_frame();
	um_av_init_packet(&mAvpkt);
	//UMLOG_INFO("succeed to open audio(FFMPEG) decoder.");
	return 0;
}

UMSint um_adec_init(UMSint8* pkgname)
{
	int ret;
	char path[256] = {0};
	
	sprintf(path, "/data/data/%s/lib", pkgname);

	if(is_register_decoder==0)
	{	
	
		//um_get_lib_path(&path);
		ret = um_load_lib(path);
		
		if(ret < 0)
		{
			UMLOG_ERR("load the ffmpeg library fail.");
			return -1;
		}
		
		//avcodec_init();
		UMLOG_INFO("===========um_adec_init and lib path is %s",path);
		um_avcodec_register_all();					
	}
	is_register_decoder++;
	
	if(is_register_decoder > 2)
		UMLOG_ERR("the ffmpeg load have an exception");
	
	return 0;
}

UMSint um_adec_fini()
{
	is_register_decoder--;
	
	if(is_register_decoder == 0)
		um_unload_lib();

	return 0;
}

UM_AudioDecoderCtx* um_adec_create(UM_CodecParams* params)
{
	UMSint ret = -1;
	UM_AudioDecoderCtx* thiz = NULL;

	if(is_register_decoder==0)
	{
		//avcodec_init();
		um_avcodec_register_all();
		is_register_decoder = 1;
	}
	if((thiz = (UM_AudioDecoderCtx*)um_calloc(1, sizeof(UM_AudioDecoderCtx) + sizeof(FFMpegADecPriv))) != NULL)
	{
		memcpy(&thiz->params, params, sizeof(UM_CodecParams));
		switch(thiz->params.codec)
		{
			case CODEC_UID_AAC:
				ret = ffmpeg_adec_init(thiz, CODEC_ID_AAC);
				break;
		}
		if(ret < 0)
		{
			UMLOG_ERR("================um_adec_create ffmpeg_adec_init fail");
			um_adec_destroy(thiz);
			thiz = NULL;
		}
	}
	return thiz;
}

UMSint um_adec_destroy(UM_AudioDecoderCtx* thiz)
{
	FFMpegADecPriv* priv = NULL;
	
	if(!thiz) return -1;
	
	priv = (FFMpegADecPriv*)thiz->priv;
	if(priv->codecCtx != NULL)
	{
		UMLOG_INFO("ffmpeg_adec_init, avcodec_close() before, priv->codecCtx=%p", priv->codecCtx);
		um_avcodec_close(priv->codecCtx);
		UMLOG_INFO("ffmpeg_adec_init, avcodec_close() after, priv->codecCtx=%p", priv->codecCtx);
//		av_free(priv->codecCtx);
//              free((char*)priv->codecCtx - ((char*)priv->codecCtx)[-1]);
		UMLOG_INFO("ffmpeg_adec_init, av_free() after, priv->codecCtx=%p", priv->codecCtx);
		priv->codecCtx = NULL;
		priv->codec = NULL;
	}
	if(priv->decodedFrame != NULL)
	{
		um_avcodec_free_frame(&priv->decodedFrame);
		priv->decodedFrame = NULL;
	}
	um_free(thiz);
	return 0;
}

UMSint um_adec_decode(UM_AudioDecoderCtx* thiz, UMSint8* buf, UMSint bufLen)
{
	UMSint size,len;
	int got_frame = 0;
	FFMpegADecPriv* priv = (FFMpegADecPriv*)thiz->priv;

	thiz->outData = priv->outBuf;
	size = AVCODEC_MAX_AUDIO_FRAME_SIZE;	

	mAvpkt.data = (uint8_t*)buf;
	mAvpkt.size = bufLen;
	//len = avcodec_decode_audio2(priv->codecCtx, (int16_t*)(thiz->outData), &size, (uint8_t*)buf, bufLen);
   
	len = um_avcodec_decode_audio4(priv->codecCtx, priv->decodedFrame, &got_frame, &mAvpkt);
	
	if(len <= 0)
	{
		return -1;
	}
	if (got_frame)
	{
 		int data_size = um_av_samples_get_buffer_size(NULL, priv->codecCtx->channels,
                                                     priv->decodedFrame->nb_samples,
                                                     priv->codecCtx->sample_fmt, 1);
		thiz->outDataLen = data_size;
		thiz->outSample_rate = priv->codecCtx->sample_rate;
		thiz->outSample_fmt = priv->codecCtx->sample_fmt;
		thiz->outChannels = priv->codecCtx->channels;
		memcpy(thiz->outData, (int16_t*)priv->decodedFrame->data[0], data_size);
	}
	return len;
}

UMSint um_adec_reset(UM_AudioDecoderCtx* ctx)
{
	return 0;
}
