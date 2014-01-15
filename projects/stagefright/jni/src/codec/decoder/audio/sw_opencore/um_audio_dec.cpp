#include "um.h"
#include "um_mm.h"
#include "um_sys_log.h"
#include "aac_dec.h"

typedef struct _OpenCoreADecPriv
{
	OmxAacDecoder* aac_dec;
	AUDIO_PARAM_PCMMODETYPE aPcmModeType;
	UMSint8 outBuf[4*1024];
}OpenCoreADecPriv;

UMSint um_adec_init(UMSint8* packetName)
{
	return 0;
}

UMSint um_adec_fini()
{
	return 0;
}

/************************************************************************/
/* audio decoder                                                        */
/************************************************************************/
UM_AudioDecoderCtx* um_adec_create(UM_CodecParams* params)
{

	UMSint ret = -1;
	UM_AudioDecoderCtx* thiz = NULL;

	UMLOG_INFO("===audio have used the opencore audio decode module");
	if((thiz = (UM_AudioDecoderCtx*)um_calloc(1, sizeof(UM_AudioDecoderCtx) + sizeof(OpenCoreADecPriv))) != NULL)
	{
		OpenCoreADecPriv* priv = (OpenCoreADecPriv*)thiz->priv;
		
		memcpy(&thiz->params, params, sizeof(UM_CodecParams));
		switch(thiz->params.codec)
		{
			case CODEC_UID_AAC:
				if((priv->aac_dec = new OmxAacDecoder()) != NULL)
				{
					ret = (priv->aac_dec->AacDecInit(thiz->params.channels) == UM_FALSE) ? -1 : 0; //false = 0;
				}
				break;
		}
		if(ret == 0)
		{
			thiz->outData = priv->outBuf;
			memset(&priv->aPcmModeType, 0, sizeof(AUDIO_PARAM_PCMMODETYPE));
		}
		else
		{
			UMLOG_ERR("um_adec_create fail");
			um_adec_destroy(thiz);
			thiz = NULL;
		}
	}
	return thiz;
}

UMSint um_adec_destroy(UM_AudioDecoderCtx* thiz)
{

	OpenCoreADecPriv* priv = NULL;

	if(!thiz) return -1;
	
	priv = (OpenCoreADecPriv*)thiz->priv;
	if(priv->aac_dec)
	{
		priv->aac_dec->ResetDecoder();
		priv->aac_dec->AacDecDeinit();
		delete priv->aac_dec;
		priv->aac_dec = NULL;
	}
	um_free(thiz);

	return 0;
}

UMSint um_adec_decode(UM_AudioDecoderCtx* thiz, UMSint8* buf, UMSint bufLen)
{
	int ret;
	OpenCoreADecPriv* priv = NULL;
	UMSint leftLen = 0;
	static int32 aIsFirstBuffer = 0;
	int32 aSamplesPerFrame = 0;
	Bool aResizeFlag = 0;
    AUDIO_PARAM_AACPROFILETYPE aAacProfileType;

	if(!thiz || !buf || bufLen<=0) return -1;

	priv = (OpenCoreADecPriv*)thiz->priv;
	
	leftLen = bufLen;
	memset(&aAacProfileType, 0, sizeof(AUDIO_PARAM_AACPROFILETYPE));
	
	if(priv->aac_dec->AacDecodeFrames((int16*)thiz->outData,
									  (uint32*)&thiz->outDataLen, 
									  (uint8**)&buf, 
									  (uint32*)&leftLen,
									  &aIsFirstBuffer, 
									  &priv->aPcmModeType, 
									  &aAacProfileType, 
									  &aSamplesPerFrame, 
									  &aResizeFlag) ==0 )
	{
		thiz->outSample_rate = priv->aPcmModeType.nSamplingRate;
		thiz->outSample_fmt = priv->aPcmModeType.nBitPerSample;
		thiz->outChannels = priv->aPcmModeType.nChannels;
		thiz->outDataLen = 2*thiz->outDataLen;
		ret = 1;
	}
	else
	{
		UMLOG_ERR("AacDecodeFrames aIsFirstBuffer is %d,thiz->outSample_rate is %d thiz->outChannels %d",aIsFirstBuffer,thiz->outSample_rate,
						thiz->outChannels);
		ret = -1;
	}
	return ret;
}

UMSint um_adec_reset(UM_AudioDecoderCtx* thiz)
{
	if(!thiz) return -1;
	
	OpenCoreADecPriv* priv = NULL;
	
	priv = (OpenCoreADecPriv*)thiz->priv;
    priv->aac_dec->ResetDecoder();
	
	return 0;
}
