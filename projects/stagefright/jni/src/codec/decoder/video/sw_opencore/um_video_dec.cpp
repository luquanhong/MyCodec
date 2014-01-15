#include "um.h"
#include "um_mm.h"
#include "um_sys_log.h"
#include <android/log.h>
#include "mpeg4_dec.h"

//#define USING_OMX_H264_HW

#ifdef USING_OMX_H264_HW
#include "avc_hwdec.h"
#else
#include "avc_dec.h"
#endif

#define UM_MAX_BUFFER_SIZE 1920*1080*1.5

typedef struct _OpenCoreVDecPriv
{
	Mpeg4Decoder_OMX* mpeg4_dec;
#ifdef USING_OMX_H264_HW
	AvcHwDecoder_OMX* h264_hwdec;
#else
	AvcDecoder_OMX* h264_swdec;
#endif
	PARAM_PORTDEFINITIONTYPE aPortParam;
}OpenCoreVDecPriv;

/************************************************************************/
/* video decoder    		                                            */
/************************************************************************/
static UMSint omx_vdec_reset_impl(UM_VideoDecoderCtx* thiz)
{
	OpenCoreVDecPriv* priv = (OpenCoreVDecPriv*)thiz->priv;
	
	if(priv->mpeg4_dec)
	{
		priv->mpeg4_dec->Mp4DecInit();
	}
#ifdef USING_OMX_H264_HW
	else if(priv->h264_hwdec)
	{
		priv->h264_hwdec->AvcDecInit_OMX();
	}
#else
	else if(priv->h264_swdec)
	{
		priv->h264_swdec->AvcDecInit_OMX();
	}
#endif
	thiz->resetFlag = 0;
	return 0;
}

UMSint um_vdec_init(UMSint8* packetName)
{
	return 0;
}

UMSint um_vdec_fini()
{
	return 0;
}

UM_VideoDecoderCtx* um_vdec_create(UM_CodecParams* params)
{
	UMSint ret = -1;
	UM_VideoDecoderCtx* thiz = NULL;
	if((thiz = (UM_VideoDecoderCtx*)um_calloc(1, sizeof(UM_VideoDecoderCtx) + sizeof(OpenCoreVDecPriv))) != NULL)
	{
		OpenCoreVDecPriv* priv = (OpenCoreVDecPriv*)thiz->priv;
		priv->mpeg4_dec = NULL;
		priv->h264_swdec = NULL;
		memcpy(&thiz->params, params, sizeof(UM_CodecParams));
		switch(thiz->params.codec)
		{
			case CODEC_UID_MPEG4:
				if((priv->mpeg4_dec = new Mpeg4Decoder_OMX()) != NULL)
				{
					ret = (priv->mpeg4_dec->Mp4DecInit()==0) ? 0 : -1;
				}
				break;
			case CODEC_UID_MPEG2VIDEO:
				break;
			case CODEC_UID_H264:				
#ifdef USING_OMX_H264_HW
				if((priv->h264_hwdec = new AvcHwDecoder_OMX()) != NULL)
				{
					ret = (priv->h264_hwdec->AvcHwDecoder_OMX()==0) ? 0 : -1;
				}
#else
				if((priv->h264_swdec = new AvcDecoder_OMX()) != NULL)
				{
					ret = (priv->h264_swdec->AvcDecInit_OMX()==0) ? 0 : -1;
				}
#endif
				break;
			case CODEC_UID_H263:
				break;
		}
		if(ret == 0)
		{
			memset(&priv->aPortParam, 0, sizeof(PARAM_PORTDEFINITIONTYPE));
			priv->aPortParam.nFrameWidth = thiz->params.v.width; 
			priv->aPortParam.nFrameHeight= thiz->params.v.height;
			thiz->outData = (UMSint8*)um_malloc(UM_MAX_BUFFER_SIZE);
			thiz->outData = NULL;
		}
		else
		{
			um_vdec_destroy(thiz);
			thiz = NULL;
		}
	}
	return thiz;
}

UMSint um_vdec_destroy(UM_VideoDecoderCtx* thiz)
{
	OpenCoreVDecPriv* priv = NULL;
	if(!thiz) return -1;
	
	priv = (OpenCoreVDecPriv*)thiz->priv;
	if(priv->mpeg4_dec)
	{
		priv->mpeg4_dec->Mp4DecDeinit();
		delete priv->mpeg4_dec;
		priv->mpeg4_dec = NULL;
	}
#ifdef USING_OMX_H264_HW
	if(priv->h264_hwdec)
	{
		priv->h264_hwdec->AvcDecDeinit_OMX();
		delete priv->h264_hwdec;
		priv->h264_hwdec = NULL;
	}
#else
	if(priv->h264_swdec)
	{
		priv->h264_swdec->AvcDecDeinit_OMX();
		delete priv->h264_swdec;
		priv->h264_swdec = NULL;
	}
#endif

	if (thiz->outData)
	{
		um_free(thiz->outData);
		thiz->outData = NULL;
	}
	
	um_free(thiz);
	return 0;
}

UMSint um_vdec_decode(UM_VideoDecoderCtx* thiz, UMSint8* buf, UMSint bufLen)
{
	OpenCoreVDecPriv* priv = NULL;
	int32 aFrameCount = 0, leftLen = 0;
	Bool ret = 0;
	Bool aResizeFlag = 0;
	if(!thiz || !buf || bufLen<=0) return -1;
	
	if(thiz->resetFlag)
	{
		omx_vdec_reset_impl(thiz);
	}
	priv = (OpenCoreVDecPriv*)thiz->priv;
	thiz->outGotpicture = 0;
	
	leftLen = bufLen;
	if(priv->mpeg4_dec)
	{
		ret = priv->mpeg4_dec->Mp4DecodeVideo((uint8*)thiz->outData, 
						(uint32*)&thiz->outDataLen, 
						(uint8**)&buf, 
						(uint32*)&leftLen, 
						&priv->aPortParam, 
						&aFrameCount, 
						0, 
						&aResizeFlag);
	}
#ifdef USING_OMX_H264_HW
	else if(priv->h264_hwdec)
	{
		ret = priv->h264_hwdec->AvcDecodeVideo_OMX((uint8*)thiz->outData,
				(uint32*)&thiz->outDataLen, 
				(uint8**)&buf, 
				(uint32*)&leftLen, 
				&priv->aPortParam, 
				(uint32*)&aFrameCount, 
				0, 
				&timestamp, 
				&aResizeFlag);
	}
#else
	else if(priv->h264_swdec)
	{
		OMX_TICKS timestamp = 0;
		
		ret = priv->h264_swdec->AvcDecodeVideo_OMX((uint8*)thiz->outData,
				(uint32*)&thiz->outDataLen, 
				(uint8**)&buf, 
				(uint32*)&leftLen, 
				&priv->aPortParam, 
				(uint32*)&aFrameCount, 
				0, 
				&timestamp, 
				&aResizeFlag);
	}
#endif
	if(ret == 1)
	{
		thiz->outWidth = priv->aPortParam.nFrameWidth;
		thiz->outHeight = priv->aPortParam.nFrameHeight;
		thiz->outGotpicture = 1;
		// decoded data length
		return bufLen - leftLen;
	}
	return -1;
}

UMSint um_vdec_reset(UM_VideoDecoderCtx* thiz)
{
	if(!thiz) return -1;
	
	thiz->resetFlag++;
	return 0;
}

void um_vdec_setCompensateNum(int frameNumbers)
{
	return;
}
