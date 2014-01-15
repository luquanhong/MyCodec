/* ------------------------------------------------------------------
 * Copyright (C) 2008 PacketVideo
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 * -------------------------------------------------------------------
 */

#include "mpeg4_dec.h"



#define MAX_LAYERS 1
#define PVH263DEFAULTHEIGHT 288
#define PVH263DEFAULTWIDTH 352

// from m4v_config_parser.h
OSCL_IMPORT_REF int16 iGetM4VConfigInfo(uint8 *buffer, int length, int *width, int *height, int *, int *);

Mpeg4Decoder_OMX::Mpeg4Decoder_OMX()
{
    pFrame0 = NULL;
    pFrame1 = NULL;

    VO_START_CODE1[0] = 0x00;
    VO_START_CODE1[1] = 0x00;
    VO_START_CODE1[2] = 0x01;
    VO_START_CODE1[3] = 0x00;

    VOSH_START_CODE1[0] = 0x00;
    VOSH_START_CODE1[1] = 0x00;
    VOSH_START_CODE1[2] = 0x01;
    VOSH_START_CODE1[3] = 0xB0;

    VOP_START_CODE1[0] = 0x00;
    VOP_START_CODE1[1] = 0x00;
    VOP_START_CODE1[2] = 0x01;
    VOP_START_CODE1[3] = 0xB6;

    H263_START_CODE1[0] = 0x00;
    H263_START_CODE1[1] = 0x00;
    H263_START_CODE1[2] = 0x80;

}


/* Initialization routine */
int32 Mpeg4Decoder_OMX::Mp4DecInit()
{
    Mpeg4InitFlag = 0;
    return 0;
}



int modify_the_size(int FrameSize,int d_width,int d_height,int *bModify_size)
{
	if (/*d_width % 16 || */d_height %16)
	{
		*bModify_size=1;
		return int(d_width*d_height);
	}	
	return FrameSize;
}


/*Decode routine */
Bool Mpeg4Decoder_OMX::Mp4DecodeVideo(uint8* aOutBuffer, uint32* aOutputLength,
                                uint8** aInputBuf, uint32* aInBufSize,
                                PARAM_PORTDEFINITIONTYPE* aPortParam,
                                int32* aIsFirstBuffer, Bool aMarkerFlag, Bool *aResizeFlag)
{
    Bool Status = 1;
    static int32 display_Width, display_Height;
    int32 OldWidth, OldHeight, OldFrameSize;
	int bModify_size=0;


    OldWidth = aPortParam->nFrameWidth;
    OldHeight = aPortParam->nFrameHeight;
    *aResizeFlag = 0;

	

#ifdef _DEBUG
    static uint32 FrameCount = 0;
#endif
    uint32 UseExtTimestamp = 0;
    int32 TimeStamp;
    int32 MaxSize = BIT_BUFF_SIZE, FrameSize, InputSize, InitSize;
    uint8* pTempFrame, *pSrc[3];

    if (Mpeg4InitFlag == 0)
    {
        if (!aMarkerFlag)
        {
            InitSize = m4v_getVideoHeader(0, *aInputBuf, *aInBufSize);
        }
        else
        {
            InitSize = *aInBufSize;
        }

        if (PV_TRUE != InitializeVideoDecode(&display_Width, &display_Height,
                                             aInputBuf, (int32*)aInBufSize, MPEG4_MODE))
            return 0;

        Mpeg4InitFlag = 1;
        aPortParam->nFrameWidth = display_Width;
        aPortParam->nFrameHeight = display_Height;
        if ((display_Width != OldWidth) || (display_Height != OldHeight))
            *aResizeFlag = 1;

        *aIsFirstBuffer = 1;
        *aInBufSize -= InitSize;
        return 1;
    }

    MaxSize = *aInBufSize;

    if ((* (int32*)aInBufSize) <= 0)
    {
        return 0;
    }

    TimeStamp = -1;
    InputSize = *aInBufSize;

    // in case of H263, read the 1st frame to find out the sizes (use the m4v_config)
    if ((0 == *aIsFirstBuffer) && (H263_MODE == CodecMode))
    {
        int32 aligned_width, aligned_height;
        if (iGetM4VConfigInfo(*aInputBuf, *aInBufSize, (int *) &aligned_width, (int *) &aligned_height, (int*) &display_Width, (int *) &display_Height))
            return 0;

        aPortParam->nFrameWidth = display_Width; // use non 16byte aligned values (display_width) for H263
        aPortParam->nFrameHeight = display_Height; // like in the case of M4V (PVGetVideoDimensions also returns display_width/height)
        if ((display_Width != OldWidth) || (display_Height != OldHeight))
            *aResizeFlag = 1;

        *aIsFirstBuffer = 1;
        return 1;
    }

    Status = (Bool) PVDecodeVideoFrame(&VideoCtrl, aInputBuf,
                                           (uint32*) & TimeStamp,
                                           (int32*)aInBufSize,
                                           (uint32*) & UseExtTimestamp,
                                           (uint8*) pFrame0);

    if (Status == 1)
    {

#ifdef _DEBUG
        //printf("Frame number %d\n", ++FrameCount);
#endif
        // advance input buffer ptr
        *aInputBuf += (InputSize - *aInBufSize);

        pTempFrame = (uint8*) pFrame0;
        pFrame0 = (uint8*) pFrame1;
        pFrame1 = (uint8*) pTempFrame;

        PVGetVideoDimensions(&VideoCtrl, (int32*) &display_Width, (int32*) &display_Height);
        if ((display_Width != OldWidth) || (display_Height != OldHeight))
        {

            aPortParam->nFrameWidth = display_Width;
            aPortParam->nFrameHeight = display_Height;
            *aResizeFlag = 1;
        }
        FrameSize = (((display_Width + 15) >> 4) << 4) * (((display_Height + 15) >> 4) << 4);
		FrameSize=modify_the_size(FrameSize,display_Width,display_Height,&bModify_size);




        OldFrameSize = (((OldWidth + 15) >> 4) << 4) * (((OldHeight + 15) >> 4) << 4);

        // THIS SHOULD NEVER HAPPEN, but just in case
        // check so to not write a larger output into a smaller buffer
        if (FrameSize <= OldFrameSize)
        {
            *aOutputLength = (FrameSize * 3) >> 1;

			if (bModify_size)
			{
				pSrc[0] = VideoCtrl.outputFrame;
				pSrc[1] = pSrc[0] + OldFrameSize;
				pSrc[2] = pSrc[0] + OldFrameSize + OldFrameSize / 4;
			}else
			{
				pSrc[0] = VideoCtrl.outputFrame;
				pSrc[1] = pSrc[0] + FrameSize;
				pSrc[2] = pSrc[0] + FrameSize + FrameSize / 4;
			}
			

           

            *aOutputLength = (FrameSize * 3) >> 1;

          /*  if (bModify_size)
            {
				oscl_memcpy(aOutBuffer, pSrc[0], FrameSize);
				oscl_memcpy(aOutBuffer + OldFrameSize, pSrc[1], FrameSize >> 2);
				oscl_memcpy(aOutBuffer + OldFrameSize + OldFrameSize / 4, pSrc[2], FrameSize >> 2);
            }*/
            
            oscl_memcpy(aOutBuffer, pSrc[0], FrameSize);
            oscl_memcpy(aOutBuffer + FrameSize, pSrc[1], FrameSize >> 2);
            oscl_memcpy(aOutBuffer + FrameSize + FrameSize / 4, pSrc[2], FrameSize >> 2);
        }
        else
        {
            *aOutputLength = 0;
        }


    }
    else
    {
        *aInBufSize = InputSize;
        *aOutputLength = 0;
    }

    return Status;
}

int32 Mpeg4Decoder_OMX::InitializeVideoDecode(
    int32* aWidth, int32* aHeight, uint8** aBuffer, int32* aSize, int32 mode)
{
    uint32 VideoDecOutputSize;
    int32 OK = 1;
    CodecMode = MPEG4_MODE;

    if (mode == 0)
    {
        CodecMode = H263_MODE;
    }

    OK = PVInitVideoDecoder(&VideoCtrl, aBuffer, (int32*) aSize, 1,
                            PVH263DEFAULTWIDTH, PVH263DEFAULTHEIGHT, CodecMode);

    if (OK)
    {
        PVGetVideoDimensions(&VideoCtrl, (int32*) aWidth, (int32*) aHeight);
        CodecMode = PVGetDecBitstreamMode(&VideoCtrl);

        if (CodecMode == H263_MODE && (*aWidth == 0 || *aHeight == 0))
        {
            *aWidth = PVH263DEFAULTWIDTH;
            *aHeight = PVH263DEFAULTHEIGHT;
        }

        PVSetPostProcType(&VideoCtrl, 0);
        VideoDecOutputSize = (((*aWidth + 15) & - 16) * ((*aHeight + 15) & - 16) * 3) / 2;
		//VideoDecOutputSize=(*aWidth) * (*aHeight) *3 /2;
        pFrame0 = (uint8*) oscl_malloc(VideoDecOutputSize);
        pFrame1 = (uint8*) oscl_malloc(VideoDecOutputSize);
        PVSetReferenceYUV(&VideoCtrl, pFrame1);
        return PV_TRUE;
    }
    else
    {
        return PV_FALSE;
    }


}

int32 Mpeg4Decoder_OMX::Mp4DecDeinit()
{
    Bool Status;

    if (pFrame0)
    {
        oscl_free(pFrame0);
        pFrame0 = NULL;
    }
    if (pFrame1)
    {
        oscl_free(pFrame1);
        pFrame1 = NULL;
    }
		if(Mpeg4InitFlag)
		{
    Status = (Bool) PVCleanUpVideoDecoder(&VideoCtrl);
    if (Status != 1)
    {
        return -1;
    }
 	 }
    return 0;
}

