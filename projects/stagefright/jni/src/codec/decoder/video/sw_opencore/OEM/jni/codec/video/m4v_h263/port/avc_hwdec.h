/* ------------------------------------------------------------------
 * Copyright (C) 1998-2009 PacketVideo
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

#include "uc_port.h"

#include "SsbSipMfcApi.h"

#define MFC_FPS   // for MFC' performance test
#ifdef MFC_FPS
#include <sys/time.h>
#endif

#define USE_MFC_FRAME_BUFFER
#define MAX_TIMESTAMP 16

typedef enum {
    MFC_DECODE_STATUS_NOT_PICTURE,
    MFC_DECODE_STATUS_DECODE_SUCCESS,
    MFC_DECODE_STATUS_DECODE_FAIL,
    MFC_DECODE_STATUS_INIT_FAIL,
    MFC_DECODE_STATUS_NOT_CREATED,
} MFC_DECODE_STATUS;
typedef enum
{
    NALTYPE_SLICE = 1,  /* non-IDR non-data partition */
    NALTYPE_DPA = 2,    /* data partition A */
    NALTYPE_DPB = 3,    /* data partition B */
    NALTYPE_DPC = 4,    /* data partition C */
    NALTYPE_IDR = 5,    /* IDR NAL */
    NALTYPE_SEI = 6,    /* supplemental enhancement info */
    NALTYPE_SPS = 7,    /* sequence parameter set */
    NALTYPE_PPS = 8,    /* picture parameter set */
    NALTYPE_AUD = 9,    /* access unit delimiter */
    NALTYPE_EOSEQ = 10, /* end of sequence */
    NALTYPE_EOSTREAM = 11, /* end of stream */
   	NALTYPE_FILL = 12   /* filler data */
} NalUnitType;

class AvcHwDecoder_OMX
{
    public:
#ifdef MFC_FPS
        struct timeval  start, stop;
        unsigned int    time;
        int             frame_cnt;
        int             decode_cnt;
        int             need_cnt;

        unsigned int measureTime(struct timeval *start, struct timeval *stop);

#endif
        AvcHwDecoder_OMX() { m_iIndexTimestamp =0;};

        ~AvcHwDecoder_OMX() { m_iIndexTimestamp=0;};


        int AvcDecInit_OMX();
        int AvcDecDeinit_OMX();
        												
        Bool AvcDecodeVideo_OMX(uint8* aOutBuf, uint32* aOutBufSize,uint8** aInBuf, uint32* aInBufSize,PARAM_PORTDEFINITIONTYPE* aPortParam,
        				uint32* aFrameCount,Bool aMarkerFlag,OMX_TICKS* aOutTimestamp,Bool *aResizeFlag,OMX_TICKS aInTimestamp);

        Bool GetYuv(uint8* aOutBuf, uint32* aOutBufSize);
        void SetThumbnailMode(Bool fThumbnailMode) {m_fThumbnailMode = fThumbnailMode;}
        void ResetDecoder(void);
        int  PVAVCDecGetNALType(uint8 *bitstream, int size,int *nal_type, int *nal_ref_idc);
        int  PVAVCAnnexBGetNALUnit(uint8 *bitstream, uint8 **nal_unit,int *size);
        int	 GetNextFullNAL_OMX(uint8** aNalBuffer, int32* aNalSize, uint8* aInputBuf, uint32* aInBufSize);

    private:
        int CreateMfc(void);
        MFC_DECODE_STATUS MfcDecodeFrame(uint8 *indata, uint32 insize, OMX_TICKS intime, uint8 *outdata, uint32 *outsize, OMX_TICKS *outtime);
        int MfcGetVideoFlag(uint8 *data, uint32 size);

        uint32 iDisplay_Width, iDisplay_Height;
        void *m_hMfcHandle;
        unsigned int m_cMfcBuffSize;
        char* m_pMfcBuffBase;
        char* m_pMfcBuffCurr;
        Bool m_fMfcFirstFrameHandled;
        Bool m_fMfcCreated;
        Bool m_fMfcFailed;
        Bool m_fResized;
        Bool m_fThumbnailMode;
        SSBSIP_MFC_DEC_OUTBUF_STATUS m_DispStatus;

        OMX_TICKS m_aTimestamp[MAX_TIMESTAMP];
        int m_iIndexTimestamp;
        uint8 m_aDelimiter[4];
};

typedef class AvcHwDecoder_OMX AvcHwDecoder_OMX;

#endif  //#ifndef AVC_DEC_H_INCLUDED
