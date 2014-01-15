/*!
 * \file	offer the same interface for render.
 *
 *
 *
 * Version: 1.0,  Date: 2012-08-06
 *
 * Description: 
 *
 * Platform: 
 *
*/
#include <stdlib.h>
#include <unistd.h>
#include "um_sys_log.h"
#include "um.h"
#include "um_glrender_impl.h"


/*!
*	\brief	This function is not use.reserved for the future.
*/
UMSint um_glrender_init()
{	
	return UMSuccess;
}


/*!
*	\brief	This function is not use.reserved for the future.
*/
UMSint um_glrender_fini()
{
	return UMSuccess;
}

/*!
*	\brief	initialize the opengl resource.
*/
UM_VideoRenderCtx* um_glrender_create(UM_CodecParams* params)
{
	UM_VideoRenderCtx* thiz = NULL;
	
		
	if((thiz = (UM_VideoRenderCtx*)malloc(sizeof(UM_VideoRenderCtx) ) )!= NULL)
	{
		thiz->type = UM_OPENGLES20;
		GLESInit();
	}
	
	return thiz;
}

/*!
*	\brief	release the opengl resource.
*/
UMSint um_glrender_destory(UM_VideoRenderCtx* thiz)
{

	if(!thiz)
		return UMFailure;
		
	GLESRelease();	
	free(thiz);

	return UMSuccess;
}

/*!
*	\brief	load the need to render resources.
*	\param	buf	it's input frame data buf.
*	\param	width	it's the width of render data.
*	\param	height	it's the height of render data.
*	\return	if success, it will return UMSuccess.
*/
UMSint um_glrender_draw(UM_VideoRenderCtx* thiz, UMSint8* buf, UMSint width, UMSint height)
{
	if(!thiz)
		return UMFailure;
			
	GLESLoadDrawData((char*)buf, width, height);	
	return UMSuccess;
}

/*!
*	\brief	tell the opengl is need to render.
*	\return	if success, it will return UMSuccess.
*/
UMSint um_glrender_render(UM_VideoRenderCtx* thiz)
{
		
	GLESRender();
	return UMSuccess;
}

/*!
*	\brief	reserved for future.
*	\return	if success, it will return UMSuccess.
*/
UMSint um_glrender_pause(UM_VideoRenderCtx* thiz)
{
	if(!thiz)
		return UMFailure;
		
	GLESPause();
	return UMSuccess;
}

UMSint um_glrender_resume(UM_VideoRenderCtx* thiz)
{
	if(!thiz)
		return UMFailure;

	GLESResume();
	return UMSuccess;
}

/*!
*	\brief	change the render view sizes.
*	\param	width	it's the width of render data.
*	\param	height	it's the height of render data.
*	\return	if success, it will return UMSuccess.
*/
UMSint um_glrender_resize(UM_VideoRenderCtx* thiz, UMSint width, UMSint height)
{
	if(!thiz)
		return UMFailure;
		
	GLESResize(width, height);
	return UMSuccess;
}

