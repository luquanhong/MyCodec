/*
 * Copyright (c) 2007-2012 ubitus Inc. All Rights Reserved.
 *
 * This software is the confidential and proprietary information of
 * ubitus Inc. ("Confidential Information"). You shall not disclose
 * such Confidential Information and shall use it only in accordance
 * with the terms of the license agreement you entered into with ubitus
 * Inc. All unauthorized usages in any manner are expressly prohibited.
 *
 * UBITUS MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF
 * THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, OR NON-INFRINGEMENT. UBITUS SHALL NOT BE LIABLE FOR
 * ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR
 * DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.
*/

#define GLES2 1
#define GL_YUV_MODE 1

#ifdef GLES2
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#include <GLES/gl.h>
#include <GLES/glext.h>
#endif


#define DEFAULT_WIDTH 1920
#define DEFAULT_HEIGHT 1080

#ifdef __cplusplus
extern "C"
{
#endif


	typedef struct
	{
		// Handle to a program object
		GLuint program;

		// Attribute locations
		GLint  positionLoc;
		GLint  texCoordLoc;

		// Sampler location
		GLint samplerLoc;

		// Texture handle
		GLuint textureId[3];

	} ESRendererInfo;

	typedef struct
	{
		char y[DEFAULT_WIDTH * DEFAULT_HEIGHT];
		char u[DEFAULT_WIDTH * DEFAULT_HEIGHT /4];
		char v[DEFAULT_WIDTH * DEFAULT_HEIGHT /4];
		int width;
		int height;
	}ESRendererData;

	/*!
	*	\brief	initialize the opengl resource.
	*/
	void GLESInit();
	
	/*!
	*	\brief	release the opengl resource.
	*/
	void GLESRelease();
	
	/*!
	*	\brief	change the render view sizes.
	*	\param	width	it's the width of render data.
	*	\param	height	it's the height of render data.
	*	\return	if success, it will return UMSuccess.
	*/
	void GLESResize(int widht, int height);

	/*!
	*	\brief	load the need to render resources.
	*	\param	buf	it's input frame data buf.
	*	\param	width	it's the width of render data.
	*	\param	height	it's the height of render data.
	*	\return	if success, it will return UMSuccess.
	*/
	void GLESLoadDrawData(char *data, int width, int height);
	
	/*!
	*	\brief	tell the opengl is need to render.
	*	\return	if success, it will return UMSuccess.
	*/
	void GLESRender();

	
	void GLESPause();

	void GLESResume();


#ifdef __cplusplus
}
#endif
