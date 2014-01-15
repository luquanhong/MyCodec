#include "um.h"
#include "um_mm.h"
#include "um_sys_log.h"
#include "um_load_ffmpeg.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <dlfcn.h>
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"

typedef struct _FFMpegVDecPriv
{
	AVCodec *codec;
	AVCodecContext *codecCtx;
	AVFrame *yuvFrame, *bgyuvFrame;
	AVFrame *rgbFrame;
	struct SwsContext *swsCtx;
	AVFrame *decodedFrame;
}FFMpegVDecPriv;

static AVPacket mAvpkt;

UMSint is_register_decoder;


/************************************************************************/
/* ffmpeg video decoder                                                 */
/************************************************************************/
static UMSint ffmpeg_vdec_init(UM_VideoDecoderCtx* thiz, enum CodecID videoCodecId)
{
	FFMpegVDecPriv* priv = (FFMpegVDecPriv*)thiz->priv;

	/* find the mpeg4 video decoder */
	priv->codec = um_avcodec_find_decoder(videoCodecId);
	if(priv->codec==NULL) 
	{
		return -1;
	}
	priv->codecCtx = um_avcodec_alloc_context3(priv->codec);
	priv->yuvFrame = um_avcodec_alloc_frame();
	priv->decodedFrame = um_avcodec_alloc_frame();
	um_av_init_packet(&mAvpkt);

	/* open it */
	if(um_avcodec_open2(priv->codecCtx, priv->codec, NULL) < 0)
	{
		return -1;
	}
	return 0;
}

static void ffmpeg_vdec_close_impl(UM_VideoDecoderCtx* thiz)
{
	FFMpegVDecPriv* priv = (FFMpegVDecPriv*)thiz->priv;
	if(priv->codecCtx!=NULL)
	{
		um_avcodec_close(priv->codecCtx);
		um_av_free(priv->codecCtx);
		priv->codecCtx = NULL;
		priv->codec = NULL;
	}
	if(priv->yuvFrame != NULL)
	{
		um_avcodec_free_frame(&priv->yuvFrame);
		priv->yuvFrame = NULL;
	}
	if(priv->decodedFrame != NULL)
	{
		um_avcodec_free_frame(&priv->decodedFrame);
		priv->decodedFrame = NULL;
	}
	if(priv->bgyuvFrame!=NULL)
	{
		um_avcodec_free_frame(&priv->bgyuvFrame);
		priv->bgyuvFrame = NULL;
	}
	if(priv->rgbFrame!=NULL)
	{

		if(priv->rgbFrame->data[0]!=NULL)
		{
			um_av_free(priv->rgbFrame->data[0]);
		}

		um_avcodec_free_frame(&priv->rgbFrame);
		priv->rgbFrame = NULL;
	}
	if(priv->swsCtx!=NULL)
	{
		um_sws_freeContext(priv->swsCtx);
		priv->swsCtx = NULL;
	}
	if(thiz->outData)
	{
		um_av_free(thiz->outData);
		thiz->outData = NULL;
	}
}

static UMSint ffmpeg_vdec_reset_impl(UM_VideoDecoderCtx* thiz)
{
	enum CodecID videoCodecId;
	FFMpegVDecPriv* priv = (FFMpegVDecPriv*)thiz->priv;
	
	videoCodecId = priv->codecCtx->codec_id;
	ffmpeg_vdec_close_impl(thiz);
	//thiz->orgWidth = thiz->orgHeight = 0;
	if(ffmpeg_vdec_init(thiz, videoCodecId) == 0)
	{
		thiz->resetFlag = 0;
	}
	return (thiz->resetFlag==0) ? 0 : -1;
}

static void ffmpeg_vdec_copyframe(UM_VideoDecoderCtx* thiz, UMSint8* data[3], UMSint linesize[3], UMSint8* buf)
{	
	if(thiz->outWidth == linesize[0])
	{
		UMSint len = thiz->outWidth * thiz->outHeight;
		
		// y data
		memcpy(buf, data[0], len);
		buf += len;
		len = len >> 2;
		// u data
		memcpy(buf, data[1], len);
		buf += len;
		// v data
		memcpy(buf, data[2], len);		
	}
	else
	{
		UMSint i = 0;
		UMSint width = thiz->outWidth;
		UMSint height = thiz->outHeight;
		
		// y data
		for(i = 0;i < height;i++)
		{
			memcpy(buf, data[0], width);
			buf += width;
			data[0] += linesize[0];
		}
		width = width >> 1;
		height = height >> 1;
		// u data
		for(i = 0;i < height;i++)
		{
			memcpy(buf, data[1], width);
			buf += width;
			data[1] += linesize[1];
		}
		// v data
		for(i = 0;i < height;i++)
		{
			memcpy(buf, data[2], width);
			buf += width;
			data[2] += linesize[2];
		}
	}
}


UMSint um_vdec_init(UMSint8* pkgname)
{
	
	int ret;
	char path[256] = {0};
	
	sprintf(path, "/data/data/%s/lib", pkgname);
	
	if(is_register_decoder==0)
	{
		//UMLOG_INFO("sw_ffmpeg um_vdec_init avcodec_init");
		//avcodec_init();  //ffmpeg 1.0 remove this function.
		
		//um_get_lib_path(&path);		
		ret = um_load_lib(path);
		
		if(ret < 0)
		{
			UMLOG_ERR("load the ffmpeg library fail.");
			//return -1;
		}
		
		UMLOG_INFO("===========um_vdec_init enter,and lib path is %s",path);	
		um_avcodec_register_all();
		UMLOG_INFO("===========um_vdec_init exit");
	
	}
	is_register_decoder++;
	
	if(is_register_decoder > 2)
		UMLOG_ERR("the ffmpeg load have an exception");
		
	return 0;
}

UMSint um_vdec_fini()
{
	is_register_decoder--;
	
	if(is_register_decoder == 0)
		um_unload_lib();
	return 0;
}

UM_VideoDecoderCtx* um_vdec_create(UM_CodecParams* params)
{
	UMSint ret = -1;
	UM_VideoDecoderCtx* thiz = NULL;
	
	if(!params) return NULL;
	
	if((thiz = (UM_VideoDecoderCtx*)um_calloc(1, sizeof(UM_VideoDecoderCtx) + sizeof(FFMpegVDecPriv))) != NULL)
	{
		memcpy(&thiz->params, params, sizeof(UM_CodecParams));
		switch(thiz->params.codec)
		{
			case CODEC_UID_MPEG4:
				ret = ffmpeg_vdec_init(thiz, CODEC_ID_MPEG4);
				break;
			case CODEC_UID_MPEG2VIDEO:
				ret = ffmpeg_vdec_init(thiz, CODEC_ID_MPEG2VIDEO);
				break;
			case CODEC_UID_H264:
				ret = ffmpeg_vdec_init(thiz, CODEC_ID_H264);
				break;
			case CODEC_UID_H263:
				ret = ffmpeg_vdec_init(thiz, CODEC_ID_H263);
				break;
		}
		//UMLOG_INFO("sw_ffmpeg um_vdec_create codec=%d ret=%d", thiz->params.codec, ret);
		if(ret < 0)
		{
			//UMLOG_INFO("sw_ffmpeg um_vdec_create failed");
			um_vdec_destroy(thiz);
			thiz = NULL;
		}
	}
	return thiz;
}

UMSint um_vdec_destroy(UM_VideoDecoderCtx* thiz)
{
	if(!thiz) return -1;
	
	//UMLOG_INFO("sw_ffmpeg um_vdec_destroy");
	ffmpeg_vdec_close_impl(thiz);
	um_free(thiz);
	return 0;
}

UMSint um_vdec_decode(UM_VideoDecoderCtx* thiz, UMSint8* buf, UMSint bufLen)
{
	UMSint nLen = 0;
	FFMpegVDecPriv* priv = NULL;
	int got_picture = 0;
	
	if(!thiz || !buf || bufLen<=0) return -1;
	
	if(thiz->resetFlag)
	{
		ffmpeg_vdec_reset_impl(thiz);
	}
	priv = (FFMpegVDecPriv*)thiz->priv;

	mAvpkt.data = buf;
	mAvpkt.size = bufLen;
	thiz->outGotpicture = 0;
    	nLen = um_avcodec_decode_video2(priv->codecCtx, priv->yuvFrame, &thiz->outGotpicture, &mAvpkt);
	if(thiz->outGotpicture > 0)
	{
		if(thiz->outData != NULL && (thiz->outWidth != priv->codecCtx->width || thiz->outHeight != priv->codecCtx->height))
		{
			thiz->outWidth = priv->codecCtx->width;
			thiz->outHeight = priv->codecCtx->height;
			um_av_free(thiz->outData);
			thiz->outData = NULL;
			UMSint size = ((thiz->outWidth + 8) * (thiz->outHeight + 8) * 3) >> 1;
			thiz->outData = (UMSint8*)um_av_malloc(size);
			thiz->outDataLen = (thiz->outWidth * thiz->outHeight * 3) >> 1;
		}
		thiz->outWidth = priv->codecCtx->width;
		thiz->outHeight = priv->codecCtx->height;
		if(thiz->outData == NULL)
		{
			UMSint size = ((thiz->outWidth + 8) * (thiz->outHeight + 8) * 3) >> 1;
			
			thiz->outData = (UMSint8*)um_av_malloc(size);
			thiz->outDataLen = (thiz->outWidth * thiz->outHeight * 3) >> 1;
		}
		if(thiz->outData)
		{
			ffmpeg_vdec_copyframe(thiz, (UMSint8 **)priv->yuvFrame->data, priv->yuvFrame->linesize, thiz->outData);
		}
	}

	return nLen;
}

UMSint um_vdec_reset(UM_VideoDecoderCtx* thiz)
{
	if(!thiz) return -1;
	
	thiz->resetFlag++;
	return 0;
}

void um_vdec_setCompensateNum(UM_VideoDecoderCtx* thiz, int frameNumbers)
{
	return;
}
