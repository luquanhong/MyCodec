/***************************************************************************
 * FILE NAME: um_audio_dec.h
 *
 * Version: 1.0,  Date: 2012-08-06
 *
 * Description: 
 *
 * Platform: 
 *
 ***************************************************************************/
 
#ifndef _UM_H_
#define _UM_H_

/***************************************************************
*    ubmedia module type define
****************************************************************/
typedef int                 UMSint;

typedef unsigned int        UMUint;

typedef signed long         UMSint32;

typedef long long           UMSint64;

typedef unsigned long long  UMUint64;

typedef unsigned int        UMUint32;

typedef signed short        UMSint16;

typedef unsigned short      UMUint16;

typedef signed char         UMSint8;

typedef unsigned char       UMUint8;

typedef UMUint16            UMWChar;

typedef UMUint8             UMUTF8Char;

typedef int                 UMBool;

typedef signed int          UMResult;

typedef double UMDouble;

typedef float  UMFloat;

typedef const void*         UMHandle;

#define UMFailure (-1)

#define UMSuccess (0)

#define UM_FALSE            0

#define UM_TRUE             1

#define UM_INVALID_HANDLE   ((void*)UM_NULL)

#define UP_INVALID_INDEX    (-1)

#ifndef __cplusplus
#define UM_NULL             ((void *)0)
#else
#define UM_NULL             (0)
#endif


/***************************************************************
*    ubmedia module codec&render common struct define
****************************************************************/

typedef enum
{
	CODEC_UID_NONE = 0,
	CODEC_UID_AAC,
	CODEC_UID_H264,
	CODEC_UID_MPEG4,
	CODEC_UID_MPEG2VIDEO,
	
	CODEC_UID_H263,
}UM_CodecUID;

typedef enum
{
	FRAME_TYPE_NONE ,
	FRAME_TYPE_YUV,
	FRAME_TYPE_RGB,
}UM_VideoFrameType;


typedef struct UMCodecParams
{

	UM_CodecUID codec;
	UM_VideoFrameType frameType;
	UMSint 		avccmp;
    UMSint      fps;

	UMSint 		width;
	UMSint		height;

	UMSint 		channels; 
	UMSint		sampleRate;

}UM_CodecParams;

/*!
*	\brief	this global parameter for ffmpeg load 
*/
extern UMSint is_register_decoder;

/***************************************************************
*    ubmedia audio decoder interface define
****************************************************************/
typedef struct AudioDecoderContext
{
	UM_CodecParams params;
	UMSint8* 	outData;
	UMSint 		outDataLen;
	UMSint 		outSample_rate; 
	UMSint 		outSample_fmt;
	UMSint 		outChannels;
	UMSint8 	priv[0];
	UMSint8* 	reserved;
}UM_AudioDecoderCtx;


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*!
*	\brief	initialize the audio decoder.it's used for software decoding.
*/
UMSint um_adec_init(UMSint8* packetName);

/*!
*	\brief	release the resource of audio decoder resource.it's used for software decoding.
*/
UMSint um_adec_fini();

/*!
*	\brief	create the decoder with using the input params.
*	\param	params	the input params for setting decode.
*	\return	it will return AudioDecoderContext, if it create success. otherwise it will return null.
*/
UM_AudioDecoderCtx* um_adec_create(UM_CodecParams* params);

/*!
*	\brief	release the audio decoder resource when the decoder close.
*/
UMSint um_adec_destroy(UM_AudioDecoderCtx* thiz);

/*!
*	\brief	decode the input audio data.
*	\param	in	it's input audio frame data buf.
*	\param	len	it's the length of input audio simple data.
*	\return	if success, it will return UM_SUCCESS.
*/
UMSint um_adec_decode(UM_AudioDecoderCtx* thiz, UMSint8* buf, UMSint bufLen);


/*!
*	\brief	reset the audio decoder, it's used for software decoding.
*/
UMSint um_adec_reset(UM_AudioDecoderCtx* thiz);


/***************************************************************
*    ubmedia audio decoder interface define
****************************************************************/
typedef struct VideoDecoderContext
{
	UM_CodecParams params;//!<	the input params for setting decode.
	// output frame size
	UMSint outWidth; 
	UMSint outHeight;
	UMSint8* outData;
	UMSint outDataLen;
	UMSint outGotpicture;
	UMSint resetFlag;
	UMSint8 priv[0];
	UMSint8* reserved;
}UM_VideoDecoderCtx;

/*!
*	\brief	initialize the video decoder.it's used for software decoding.
*/
UMSint um_vdec_init(UMSint8* packetName);

/*!
*	\brief	release the resource of initialization.it's used for software decoding.
*/
UMSint um_vdec_fini();

/*!
*	\brief	create the video decoder with using the input params.
*	\param	params	the input params for setting decode.
*	\return	it will return VideoDecoderContext, if it create success. otherwise it will return null.
*/
UM_VideoDecoderCtx* um_vdec_create(UM_CodecParams* params);

/*!
*	\brief	release the video decoder resource when the decoder close.
*/
UMSint um_vdec_destroy(UM_VideoDecoderCtx* thiz);

/*!
*	\brief	pass the display surface to decoder,it's only used for hardware decoding.
*	\param	thiz 	the decoder context.
*	\param	surface 	the input pramas, it's passed from java layer. 
*	\return	UMSuccess  	the surface set succesful.
*			UMFailure		have error with setting the surface.
*/
UMSint um_vdec_setSurface(UM_VideoDecoderCtx* thiz,void* surface);

/*!
*	\brief	decode the input video data.
*	\param	in	it's input video frame data buf.
*	\param	len	it's the length of input video frame data.
*	\return	if success, it will return UM_SUCCESS.
*/
UMSint um_vdec_decode(UM_VideoDecoderCtx* thiz, UMSint8* buf, UMSint bufLen);

/*!
*	\brief	reset the video decoder, it's used for software decoding.
*/
UMSint um_vdec_reset(UM_VideoDecoderCtx* thiz);

/*!
*	\brief	add the compensate frames for hardware decoder, it's used for hardware decoding.
*	\param	thiz	the decoder context.
*	\param	frameNumbers	needed frame number.
*/
void um_vdec_setCompensateNum(UM_VideoDecoderCtx* thiz, UMSint frameNumbers);

/*!
*	\brief	get the surface width & hight information
*	\param	thiz	the decoder context.
*	\param	width	return the width of the surface
*	\param	hight	return the hight of the surface
*	\return	if success, it will return UM_SUCCESS.
*/
UMSint um_vdec_getSurfaceSize(UM_VideoDecoderCtx* thiz, void* surface, int* width, int* hight);

/***************************************************************
*    ubmedia audio render interface define
****************************************************************/
/*!
*	\brief	audio render type
*/
typedef enum
{
	UM_NATIVE_AUDIOTRACK,
	UM_OPENSL,
}UM_AudioRenderType;

/*!
*	\brief	the audio render params,reserved for future. 
*/
typedef struct AudioRenderContext
{
	UM_AudioRenderType type;
	UMSint nChannels;
	UMSint nSamplingRate; 
	UMBool bCallback;
}UM_AudioRenderCtx;

UMSint um_audio_init(UM_AudioRenderCtx* thiz, void* callback);

UMSint um_audio_fini(UM_AudioRenderCtx* thiz);

UMSint um_audio_render(UM_AudioRenderCtx* thiz, UMSint* buf, UMSint bufLen);



/***************************************************************
*    ubmedia video render interface define
****************************************************************/
/*!
*	\brief	opengl version
*/
typedef enum
{
	UM_OPENGL,
	UM_OPENGLES11,
	UM_OPENGLES20,
}UM_VideoRenderType;

/*!
*	\brief	the opengl params,reserved for future. 
*/
typedef struct VideoRenderContext
{
	UM_VideoRenderType type;
	UM_CodecParams params;
} UM_VideoRenderCtx;

/*!
*	\brief	This function is not use.reserved for the future.
*/
UMSint um_glrender_init();

/*!
*	\brief	This function is not use.reserved for the future.
*/
UMSint um_glrender_fini();

/*!
*	\brief	initialize the opengl resource.
*/
UM_VideoRenderCtx* um_glrender_create(UM_CodecParams* params);

/*!
*	\brief	release the opengl resource.
*/
UMSint um_glrender_destory(UM_VideoRenderCtx* thiz);

/*!
*	\brief	load the need to render resources.
*	\param	buf	it's input frame data buf.
*	\param	width	it's the width of render data.
*	\param	height	it's the height of render data.
*	\return	if success, it will return UMSuccess.
*/
UMSint um_glrender_draw(UM_VideoRenderCtx* thiz, UMSint8* buf, UMSint width, UMSint height);

/*!
*	\brief	tell the opengl is need to render.
*	\return	if success, it will return UMSuccess.
*/
UMSint um_glrender_render(UM_VideoRenderCtx* thiz);

/*!
*	\brief	tell the opengl is need to render.
*	\param	width	it's the width of render data.
*	\param	height	it's the height of render data.
*	\return	if success, it will return UMSuccess.
*/
UMSint um_glrender_resize(UM_VideoRenderCtx* thiz, UMSint width, UMSint height);

/*!
*	\brief	This function is not use.reserved for the future.
*/
UMSint um_glrender_pause(UM_VideoRenderCtx* thiz);

/*!
*	\brief	This function is not use.reserved for the future.
*/
UMSint um_glrender_resume(UM_VideoRenderCtx* thiz);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
