/*!
*	\file	offer the same interface for decode.
*/

#include <gui/Surface.h>
#include "um.h"
#include "um_sys_thread.h"
#include "um_sys_log.h"
#include "UM_MediaPlayer.h"



/*!
*	\brief	type define.
*/
typedef	struct _StageFrightVDecPriv
{
	UMMediaPlayer* mPlayer;	//!< main instance for decode.
	
}StageFrightVDecPriv;

/*!
*	\brief	set the surface for the output port	of decode, it's only needed by dsp.
*/
int um_vdec_setSurface(UM_VideoDecoderCtx* thiz, void* surface)
{

	StageFrightVDecPriv* priv = NULL;

	if(!thiz ||!surface)
		return UMFailure;
	
	priv = (StageFrightVDecPriv*)thiz->priv;
	
	Surface* const p = (Surface*)surface;
	
	sp<Surface> isurface=sp<Surface>(p);
	if(priv)
	{
		UMMediaPlayer* player=(UMMediaPlayer*)priv->mPlayer;  
		player->setSurface(isurface);
	}
	
	return UMSuccess;
}


/*!
*	\brief	video init.
*/
UMSint um_vdec_init(UMSint8* packetName)
{
	
	return UMSuccess;
}

/*!
*	\brief	video finit.
*/
UMSint um_vdec_fini()
{
	return UMSuccess;
}

/*!
*	\brief	create the video player instance for decode.
*/
UM_VideoDecoderCtx* um_vdec_create(UM_CodecParams* params)
{
	UM_VideoDecoderCtx* thiz = NULL;
	int codeType;
	int avccmp = 0;
	
	if(!params)
		return UM_NULL;
		
	if((thiz = (UM_VideoDecoderCtx*)malloc(sizeof(UM_VideoDecoderCtx) + sizeof(StageFrightVDecPriv))) != NULL)
	{
		StageFrightVDecPriv* priv = (StageFrightVDecPriv* )thiz->priv;
		memcpy(&thiz->params, params, sizeof(UM_CodecParams));
		
		if(params->codec == CODEC_UID_MPEG4 )
		{
			UMLOG_INFO("the codeType is params->codec = CODEC_UID_MPEG4");
			codeType = 1;
		}
		else if(params->codec == CODEC_UID_H264)
		{
			UMLOG_INFO("the codeType is params->codec = CODEC_UID_H264");
			codeType = 2;	
			avccmp = params->avccmp;
		}
		else
		{
			UMLOG_ERR("the codeType not support.");
			return UM_NULL;
		}

		priv->mPlayer = new UMMediaPlayer(codeType, params->width, params->height, avccmp);
	}

	return thiz;
}

/*!
*	\brief	destroy the video player instance for decode.
*/
UMSint um_vdec_destroy(UM_VideoDecoderCtx* thiz)
{
	StageFrightVDecPriv* priv = NULL;
	
	if(!thiz)
		return UMFailure;
	
	priv = (StageFrightVDecPriv*)thiz->priv;

	if(priv->mPlayer!=NULL)
	{
		priv->mPlayer->release();	
		delete priv->mPlayer;
		priv->mPlayer = NULL;
	}
	
	free(thiz);

	return UMSuccess;
}

/*!
*	\brief	decode the input data.
*	\param	in	it's input frame data buf.
*	\param	len	it's the length of input frame data.
*	\return	if success, it will return UM_SUCCESS.
*/
UMSint um_vdec_decode(UM_VideoDecoderCtx* thiz, UMSint8* buf, UMSint bufLen)
{
	StageFrightVDecPriv* priv;
	UMMediaPlayer* player;
	int ret;
	
	if(!thiz || !buf || bufLen <= 0 ||
		UM_NULL == (priv = (StageFrightVDecPriv*)thiz->priv) ||
		UM_NULL == (player =(UMMediaPlayer*)priv->mPlayer))
	{
		return UMFailure;
	}
	
	ret = player->setInputData((char*)buf, bufLen, 0);//!< when the first invoke, it will create decoder according to the buf. 
	player->play();//!< when the first invoke, it will start the decode thread.

	return ret;
}

/*!
*	\brief	stop the decode input data.
*/
void um_vdec_stop(UM_VideoDecoderCtx* thiz)
{
	StageFrightVDecPriv* priv = NULL;

	if(!thiz)
		return ;
		
	priv = (StageFrightVDecPriv*)thiz->priv;
	
	if(priv->mPlayer)
	{
		if(priv->mPlayer->isPlaying())
			priv->mPlayer->stop();
	}
}

/*!
*	\brief	reset the decode whill it's needed, now it's not implement on the dsp.
*/
UMSint um_vdec_reset(UM_VideoDecoderCtx* thiz)
{
	if(!thiz) 
		return UMFailure;
	
	thiz->resetFlag++;

	return UMSuccess;
}

