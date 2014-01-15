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
#include "avc_dec.h"
#include "avcdec_int.h"
//#include <android/log.h>


/*************************************/
/* functions needed for video engine */
/*************************************/
// init static members of AvcDecoder class
OMX_TICKS AvcDecoder_OMX::CurrInputTimestamp = 0;
uint8* AvcDecoder_OMX::pDpbBuffer = NULL;
uint32 AvcDecoder_OMX::FrameSize = 0;
uint32 AvcDecoder_OMX::iAvcDecoderCounterInstance = 0;
OMX_TICKS	AvcDecoder_OMX::DisplayTimestampArray[] = {0};
AVCHandle	AvcDecoder_OMX::AvcHandle;
AVCDecSPSInfo	AvcDecoder_OMX::SeqInfo;

/* These two functions are for callback functions of AvcHandle */
int32 CBAVC_Malloc_OMX(void* aUserData, int32 aSize, int32 aAttribute)
{
    OSCL_UNUSED_ARG(aUserData);
    OSCL_UNUSED_ARG(aAttribute);
    void* pPtr;

    pPtr = oscl_malloc(aSize);
    return (int32) pPtr;
}

void CBAVC_Free_OMX(void* aUserData, int32 aMem)
{
    OSCL_UNUSED_ARG(aUserData);
    oscl_free((uint8*) aMem);
}


AVCDec_Status CBAVCDec_GetData_OMX(void* aUserData, uint8** aBuffer, uint* aSize)
{
    OSCL_UNUSED_ARG(aUserData);
    OSCL_UNUSED_ARG(aBuffer);
    OSCL_UNUSED_ARG(aSize);
    return AVCDEC_FAIL;  /* nothing for now */
}


int32 AvcDecoder_OMX::AllocateBuffer_OMX(void* aUserData, int32 i, uint8** aYuvBuffer)
{
    OSCL_UNUSED_ARG(aUserData);
    //printf("Index %d\n", i);
    *aYuvBuffer = pDpbBuffer + i * FrameSize;
    //Store the input timestamp at the correct index
    DisplayTimestampArray[i] = CurrInputTimestamp;
    return 1;
}


void UnbindBuffer_OMX(void* aUserData, int32 i)
{
    OSCL_UNUSED_ARG(aUserData);
    OSCL_UNUSED_ARG(i);
    return;
}

int32 AvcDecoder_OMX::ActivateSPS_OMX(void* aUserData, uint aSizeInMbs, uint aNumBuffers)
{
    OSCL_UNUSED_ARG(aUserData);
    PVAVCDecGetSeqInfo(&(AvcHandle), &(SeqInfo));

    if (pDpbBuffer)
    {
        oscl_free(pDpbBuffer);
        pDpbBuffer = NULL;
    }

    FrameSize = (aSizeInMbs << 7) * 3;
    pDpbBuffer = (uint8*) oscl_malloc(aNumBuffers * (FrameSize));
    return 1;

}

/* initialize video decoder */
Bool AvcDecoder_OMX::InitializeVideoDecode_OMX()
{
    /* Initialize AvcHandle */
    AvcHandle.AVCObject = NULL;
    AvcHandle.userData = NULL;
    AvcHandle.CBAVC_DPBAlloc = ActivateSPS_OMX;
    AvcHandle.CBAVC_FrameBind = AllocateBuffer_OMX;
    AvcHandle.CBAVC_FrameUnbind = UnbindBuffer_OMX;
    AvcHandle.CBAVC_Malloc = CBAVC_Malloc_OMX;
    AvcHandle.CBAVC_Free = CBAVC_Free_OMX;

    return 1;
}

Bool AvcDecoder_OMX::FlushOutput_OMX(uint8* aOutBuffer, uint32* aOutputLength, OMX_TICKS* aOutTimestamp, uint32 OldWidth, uint32 OldHeight)
{
    AVCFrameIO Output;
    AVCDec_Status Status;
    int32 Index, Release, FrameSize;
    uint32 OldFrameSize = ((OldWidth + 15) & (~15)) * ((OldHeight + 15) & (~15));
		////char szLog[300] = {0};
    Output.YCbCr[0] = Output.YCbCr[1] = Output.YCbCr[2] = NULL;
   // sprintf(szLog, "FlushOutput_OMX step 1");
	 ////__android_log_write(ANDROID_LOG_ERROR, "up_audio_play", szLog); 	
    Status = PVAVCDecGetOutput(&(AvcHandle), &Index, &Release, &Output);
	
		//sprintf(szLog, "*****************get output status=%d******************",Status);
	 ////__android_log_write(ANDROID_LOG_ERROR, "up_audio_play", szLog); 	
    if (Status == AVCDEC_FAIL)
    {
        return 0;
    }
    *aOutTimestamp = DisplayTimestampArray[Index];
    *aOutputLength = 0; // init to 0

    if (Output.YCbCr[0])
    {
    	//  sprintf(szLog, "FlushOutput_OMX step 2");
	// //__android_log_write(ANDROID_LOG_ERROR, "up_audio_play", szLog); 	
        FrameSize = Output.pitch * Output.height;
        // it should not happen that the frame size is smaller than available buffer size, but check just in case
        if (FrameSize <= OldFrameSize)
        {
            *aOutputLength = (Output.pitch * Output.height * 3) >> 1;

            oscl_memcpy(aOutBuffer, Output.YCbCr[0], FrameSize);
            oscl_memcpy(aOutBuffer + FrameSize, Output.YCbCr[1], FrameSize >> 2);
            oscl_memcpy(aOutBuffer + FrameSize + FrameSize / 4, Output.YCbCr[2], FrameSize >> 2);
        }
        // else, the frame length is reported as zero, and there is no copying
       //  sprintf(szLog, "FlushOutput_OMX step 3");
			 ////__android_log_write(ANDROID_LOG_ERROR, "up_audio_play", szLog); 	
  	}


    return 1;
}


/* Initialization routine */
int AvcDecoder_OMX::AvcDecInit_OMX()
{
    if (0 == InitializeVideoDecode_OMX())
    {
        return -2;
    }

    //Set up the cleanup object in order to do clean up work automatically
    pCleanObject = new AVCCleanupObject_OMX(&AvcHandle); 

    DecodeSliceFlag = 0;
    pNalBufferTemp = NULL;

    return 0;
}


/*Decode routine */
 Bool AvcDecoder_OMX::AvcDecodeVideo_OMX(uint8* aOutBuffer, uint32* aOutputLength,
                                    uint8** aInputBuf, uint32* aInBufSize,
                                    PARAM_PORTDEFINITIONTYPE* aPortParam,
                                    uint32* iFrameCount, Bool aMarkerFlag,
                                    OMX_TICKS* aOutTimestamp,
                                    Bool *aResizeFlag)
{
    AVCDec_Status Status;
    uint32 Width, Height;
    uint32 crop_top, crop_bottom, crop_right, crop_left;
    uint8* pNalBuffer;
    int32 NalSize, NalType, NalRefId, PicType;
    AVCDecObject* pDecVid;
    static int32 FrameNo;
		//char szLog[300] = {0};
    *aResizeFlag = 0;
    uint32 OldWidth, OldHeight;

    OldWidth = 	aPortParam->nFrameWidth;
    OldHeight = aPortParam->nFrameHeight;

		  //sprintf(szLog, "AvcDecodeVideo_OMX step 1");
		 //__android_log_write(ANDROID_LOG_ERROR, "up_audio_play", szLog); 	
    if (1 == DecodeSliceFlag)
    {
    		 // sprintf(szLog, "AvcDecodeVideo_OMX step 2");
		 //__android_log_write(ANDROID_LOG_ERROR, "up_audio_play", szLog); 	
        Status = (AVCDec_Status) FlushOutput_OMX(aOutBuffer, aOutputLength, aOutTimestamp, OldWidth, OldHeight);

        if ((Status = PVAVCDecodeSlice(&(AvcHandle), pNalBufferTemp, NalSizeTemp)) == AVCDEC_PICTURE_OUTPUT_READY)
        {
            DecodeSliceFlag = 1;
        }
        else
        {
            if (pNalBufferTemp)
                oscl_free(pNalBufferTemp);
            pNalBufferTemp = NULL;
            DecodeSliceFlag = 0;
        }

        return 1;
    }

    if (!aMarkerFlag)
    {
        if (AVCDEC_FAIL == GetNextFullNAL_OMX(&pNalBuffer, &NalSize, *aInputBuf, aInBufSize))
        {
            Status = (AVCDec_Status) FlushOutput_OMX(aOutBuffer, aOutputLength, aOutTimestamp, OldWidth, OldHeight);

            if (AVCDEC_FAIL != Status)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
       //sprintf(szLog, "AvcDecodeVideo_OMX step 3");
		 //__android_log_write(ANDROID_LOG_ERROR, "up_audio_play", szLog); 	
    }
    else
    {
        pNalBuffer = *aInputBuf;
        NalSize = *aInBufSize;
        //Assuming that the buffer with marker bit contains one full NAL
        *aInBufSize = 0;
    }

    if (AVCDEC_FAIL == PVAVCDecGetNALType(pNalBuffer, NalSize, &NalType, &NalRefId))
    {
        return 0;
    }

    if (AVC_NALTYPE_SPS == (AVCNalUnitType)NalType)
    {
        if (PVAVCDecSeqParamSet(&(AvcHandle), pNalBuffer, NalSize) != AVCDEC_SUCCESS)
        {
            return 0;
        }

        pDecVid = (AVCDecObject*) AvcHandle.AVCObject;

        Width = (pDecVid->seqParams[0]->pic_width_in_mbs_minus1 + 1) * 16;
        Height = (pDecVid->seqParams[0]->pic_height_in_map_units_minus1 + 1) * 16;

        if (pDecVid->seqParams[0]->frame_cropping_flag)
        {
            crop_left = 2 * pDecVid->seqParams[0]->frame_crop_left_offset;
            crop_right = Width - (2 * pDecVid->seqParams[0]->frame_crop_right_offset + 1);

            if (pDecVid->seqParams[0]->frame_mbs_only_flag)
            {
                crop_top = 2 * pDecVid->seqParams[0]->frame_crop_top_offset;
                crop_bottom = Height - (2 * pDecVid->seqParams[0]->frame_crop_bottom_offset + 1);
            }
            else
            {
                crop_top = 4 * pDecVid->seqParams[0]->frame_crop_top_offset;
                crop_bottom = Height - (4 * pDecVid->seqParams[0]->frame_crop_bottom_offset + 1);
            }
        }
        else  /* no cropping flag, just give the first and last pixel */
        {
            crop_bottom = Height - 1;
            crop_right = Width - 1;
            crop_top = crop_left = 0;
        }

        aPortParam->nFrameWidth = crop_right - crop_left + 1;
        aPortParam->nFrameHeight = crop_bottom - crop_top + 1;

        //if( (OldWidth != aPortParam->format.video.nFrameWidth) || (OldHeight !=	aPortParam->format.video.nFrameHeight))
        // FORCE RESIZE ALWAYS FOR SPS
        *aResizeFlag = 1;

        *iFrameCount = 1;

    }

    else if (AVC_NALTYPE_PPS == (AVCNalUnitType) NalType)
    {
        if (PVAVCDecPicParamSet(&(AvcHandle), pNalBuffer, NalSize) != AVCDEC_SUCCESS)
        {
            return 0;
        }
    }

    else if (AVC_NALTYPE_SLICE == (AVCNalUnitType) NalType ||
             AVC_NALTYPE_IDR == (AVCNalUnitType) NalType)
    {
    	    //sprintf(szLog, "AvcDecodeVideo_OMX step 4");
		 //__android_log_write(ANDROID_LOG_ERROR, "up_audio_play", szLog); 	
        if ((Status = PVAVCDecodeSlice(&(AvcHandle), pNalBuffer, NalSize)) == AVCDEC_PICTURE_OUTPUT_READY)
        {
        		//sprintf(szLog, "AvcDecodeVideo_OMX step 6");
		 				//__android_log_write(ANDROID_LOG_ERROR, "up_audio_play", szLog); 
            Status = (AVCDec_Status) FlushOutput_OMX(aOutBuffer, aOutputLength, aOutTimestamp, OldWidth, OldHeight);

            if ((Status = PVAVCDecodeSlice(&(AvcHandle), pNalBuffer, NalSize)) == AVCDEC_PICTURE_OUTPUT_READY)
            {
                pNalBufferTemp = (uint8*) oscl_malloc(NalSize);
                oscl_memcpy(pNalBufferTemp, pNalBuffer, NalSize);
                NalSizeTemp = NalSize;
                DecodeSliceFlag = 1;

            }
        }
				  //sprintf(szLog, "AvcDecodeVideo_OMX step 5");
		 //__android_log_write(ANDROID_LOG_ERROR, "up_audio_play", szLog); 	
        if (Status == AVCDEC_PICTURE_READY)
        {
            FrameNo++;
            //printf("decode frame %d \n", FrameNo);
        }
    }

    else if ((AVCNalUnitType)NalType == AVC_NALTYPE_SEI)
    {
        if (PVAVCDecSEI(&(AvcHandle), pNalBuffer, NalSize) != AVCDEC_SUCCESS)
        {
            return 0;
        }
    }

    else if ((AVCNalUnitType)NalType == AVC_NALTYPE_AUD)
    {
        PicType = pNalBuffer[1] >> 5;
    }

    else if ((AVCNalUnitType)NalType == AVC_NALTYPE_EOSTREAM) // end of stream
    {
        return 1;
    }

    else
    {
        printf("\nNAL_type = %d, unsupported nal type or not sure what to do for this type\n", NalType);
    }
    return 1;

}


int AvcDecoder_OMX::AvcDecDeinit_OMX()
{
    if (pCleanObject)
    {
        OSCL_DELETE(pCleanObject);
        pCleanObject = NULL;
    }

    if (pDpbBuffer)
    {
        oscl_free(pDpbBuffer);
        pDpbBuffer = NULL;
    }

    return 0;
}


AVCDec_Status AvcDecoder_OMX::GetNextFullNAL_OMX(uint8** aNalBuffer, int32* aNalSize, uint8* aInputBuf, uint32* aInBufSize)
{
    uint32 BuffConsumed;
    uint8* pBuff = aInputBuf;
    uint32 InputSize;

    *aNalSize = *aInBufSize;
    InputSize = *aInBufSize;

    AVCDec_Status ret_val = PVAVCAnnexBGetNALUnit(pBuff, aNalBuffer, aNalSize);

    if (ret_val == AVCDEC_FAIL)
    {
        return AVCDEC_FAIL;
    }

    BuffConsumed = ((*aNalSize) + (int32)(*aNalBuffer - pBuff));
    aInputBuf += BuffConsumed;
    *aInBufSize = InputSize - BuffConsumed;

    return AVCDEC_SUCCESS;
}

AVCCleanupObject_OMX::~AVCCleanupObject_OMX()
{
    PVAVCCleanUpDecoder(ipavcHandle);

}

