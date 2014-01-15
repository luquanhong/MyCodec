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
#ifndef AVC_DEC_H_INCLUDED
#define AVC_DEC_H_INCLUDED
/*
#ifndef OMX_Component_h
#include "omx_component.h"
#endif*/

#ifndef _AVCDEC_API_H_
#include "avcdec_api.h"
#endif
#include "uc_port.h"

//typedef struct OMX_TICKS
//{
  //  uint32 nLowPart;    /** low bits of the signed 64 bit tick value */
   // uint32 nHighPart;   /** high bits of the signed 64 bit tick value */
//} OMX_TICKS;
#define AVC_DEC_TIMESTAMP_ARRAY_SIZE 17

class AVCCleanupObject_OMX
{
        AVCHandle* ipavcHandle;

    public:
        AVCCleanupObject_OMX(AVCHandle* aAvcHandle = NULL)
        {
            ipavcHandle = aAvcHandle;
        }

        //! Use destructor to do all the clean up work
        ~AVCCleanupObject_OMX();
};


class AvcDecoder_OMX
{
    public:
        AvcDecoder_OMX()
        {
            iAvcDecoderCounterInstance++;
        };
        ~AvcDecoder_OMX()
        {
            iAvcDecoderCounterInstance--;
        };

        AVCCleanupObject_OMX*	pCleanObject;
        static AVCHandle		AvcHandle;
        static AVCDecSPSInfo	SeqInfo;
        static uint32			FrameSize;
        static uint8*			pDpbBuffer;
        uint8*			pNalBufferTemp;
        int32			NalSizeTemp;
        Bool		DecodeSliceFlag;
        static OMX_TICKS		DisplayTimestampArray[AVC_DEC_TIMESTAMP_ARRAY_SIZE];
        static OMX_TICKS		CurrInputTimestamp;
        static uint32			iAvcDecoderCounterInstance;


        int AvcDecInit_OMX();

        Bool AvcDecodeVideo_OMX(uint8* aOutBuffer, uint32* aOutputLength,
                                    uint8** aInputBuf, uint32* aInBufSize,
                                    PARAM_PORTDEFINITIONTYPE* aPortParam,
                                    uint32* iFrameCount, Bool aMarkerFlag,
                                    OMX_TICKS* aOutTimestamp,
                                    Bool *aResizeFlag);

        int AvcDecDeinit_OMX();

        Bool InitializeVideoDecode_OMX();

        Bool FlushOutput_OMX(uint8* aOutBuffer, uint32* aOutputLength, OMX_TICKS* aOutTimestamp, uint32 OldWidth, uint32 OldHeight);

        AVCDec_Status GetNextFullNAL_OMX(uint8** aNalBuffer, int32* aNalSize, uint8* aInputBuf, uint32* aInBufSize);

        static int32 AllocateBuffer_OMX(void* aUserData, int32 i, uint8** aYuvBuffer);

        static int32 ActivateSPS_OMX(void* aUserData, uint aSizeInMbs, uint aNumBuffers);
};

typedef class AvcDecoder_OMX AvcDecoder_OMX;

#endif	//#ifndef AVC_DEC_H_INCLUDED

