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
#include "avc_hwdec.h"

//#define LOG_NDEBUG 0
#define LOG_TAG "OMXAVC_MFC"
//#include <utils/Log.h>
/* Initialization routine */
int AvcHwDecoder_OMX::AvcDecInit_OMX()
{
#ifdef MFC_FPS
    time = 0;
    frame_cnt = 0;
    decode_cnt = 0;
    need_cnt = 0;
#endif

    iDisplay_Width = 0;
    iDisplay_Height = 0;

    m_aDelimiter[0] = 0x00;
    m_aDelimiter[1] = 0x00;
    m_aDelimiter[2] = 0x00;
    m_aDelimiter[3] = 0x01;
		
		m_fMfcCreated=0;
		m_fMfcFailed=0;
    m_iIndexTimestamp = 0;
    memset(m_aTimestamp, 0, sizeof(m_aTimestamp));

    if (CreateMfc() < 0)
    {
        //LOGE("CreateMfc fail\n");
        return -1;
    }

    //LOGV("AvcDecInit_OMX() Success");
    return 0;
}

int AvcHwDecoder_OMX::CreateMfc(void)
{
    void *pPhyBuf;
    int val;

    if (m_fMfcCreated==1)
    {
        //LOGE("CreateMfc() aleady done \n");
        return 0;
    }

    m_hMfcHandle = SsbSipMfcDecOpen();
    if (m_hMfcHandle == NULL)
    {
        //LOGE("SsbSipMfcDecOpen() Failed.\n");
        return -1;
    }

    m_pMfcBuffBase = (char *)SsbSipMfcDecGetInBuf(m_hMfcHandle, &pPhyBuf, (1024 * 500));

    m_pMfcBuffCurr = m_pMfcBuffBase;
    m_cMfcBuffSize = 0;
    m_fMfcFirstFrameHandled = 0;
    m_fMfcCreated = 1;
    m_fMfcFailed = 0;
    m_fResized = 0;
    m_fThumbnailMode = 0;

    val = 1;
    SsbSipMfcDecSetConfig(m_hMfcHandle, MFC_DEC_SETCONF_EXTRA_BUFFER_NUM, &val);

    val = 1;
    SsbSipMfcDecSetConfig(m_hMfcHandle, MFC_DEC_SETCONF_DISPLAY_DELAY, &val);

    return 0;
}
/*Decode routine */
Bool AvcHwDecoder_OMX::AvcDecodeVideo_OMX(uint8* aOutBuf, uint32* aOutBufSize,uint8** aInBuf, uint32* aInBufSize,PARAM_PORTDEFINITIONTYPE* aPortParam,
        				uint32* iFrameCount,Bool aMarkerFlag,OMX_TICKS* aOutTimestamp,Bool *aResizeFlag,OMX_TICKS aInTimestamp)
{
    Bool Status = 1;
    MFC_DECODE_STATUS ret;
    int val = 0;
     uint8* pNalBuffer;
    int32 NalSize, NalType, NalRefId, PicType;
  //  char sz[252];

    *aResizeFlag = 0;
#ifdef MFC_FPS
    gettimeofday(&start, NULL);
    decode_cnt++;
#endif
    if (m_fMfcFailed==1)
        return 0;
    if (!m_fMfcFirstFrameHandled && m_fThumbnailMode)
    {
        val = 0;
        SsbSipMfcDecSetConfig(m_hMfcHandle, MFC_DEC_SETCONF_DISPLAY_DELAY, &val);
    }
     if (!aMarkerFlag)
    {
        if (0 == GetNextFullNAL_OMX(&pNalBuffer, &NalSize, *aInBuf, aInBufSize))
        {
                return 0;
        }
    }
    else
    {
        pNalBuffer = *aInBuf;
        NalSize = *aInBufSize;
        //Assuming that the buffer with marker bit contains one full NAL
        *aInBufSize = 0;
    }
     if (0 == PVAVCDecGetNALType(pNalBuffer, NalSize, &NalType, &NalRefId))
    {
        return 0;
    }
    if (NALTYPE_SEI == (NalUnitType)NalType || NALTYPE_AUD == (NalUnitType)NalType)
    	{
    		 return 1;
    	}
    ret = MfcDecodeFrame(pNalBuffer, NalSize, aInTimestamp, aOutBuf, aOutBufSize, aOutTimestamp);
    switch (ret)
    {
        case MFC_DECODE_STATUS_INIT_FAIL:
        case MFC_DECODE_STATUS_NOT_CREATED:
            Status = 0;
            m_fMfcFailed = 1;
             *aOutBufSize=0;
            //LOGE("MfcDecodeFrame Init fail\n");
            AvcDecDeinit_OMX();

            break;

        case MFC_DECODE_STATUS_DECODE_FAIL:
            Status = 0;
            //LOGE("MfcDecodeFrame fail\n");
            *aOutBufSize=0;
            if (!m_fThumbnailMode)
                *aInBufSize  = 0;
            break;

        case MFC_DECODE_STATUS_NOT_PICTURE:
#ifdef MFC_FPS
            need_cnt++;
#endif
            Status = 1;
           // *aOutBufSize=0;
           // *aInBufSize  = 0;
            break;

        case MFC_DECODE_STATUS_DECODE_SUCCESS:
#ifdef MFC_FPS
            gettimeofday(&stop, NULL);
            time += measureTime(&start, &stop);
            frame_cnt++;
#endif
            if ((aPortParam->nFrameWidth  != (unsigned int)iDisplay_Width) ||
                (aPortParam->nFrameHeight != (unsigned int)iDisplay_Height))
            {
                printf("iDisplay_Width : %d\n", (int)iDisplay_Width);
                printf("iDisplay_Height: %d\n", (int)iDisplay_Height);
								
                aPortParam->nFrameWidth  = iDisplay_Width;
                aPortParam->nFrameHeight = iDisplay_Height;

                uint32 min_stride = ((aPortParam->nFrameWidth + 15) & (~15));
                uint32 min_sliceheight = ((aPortParam->nFrameHeight + 15) & (~15));

            /*    aPortParam->format.video.nStride = min_stride;
                aPortParam->format.video.nSliceHeight = min_sliceheight;
                aPortParam->nBufferSize = (aPortParam->format.video.nSliceHeight * aPortParam->format.video.nStride * 3) >> 1;
*/
                *aResizeFlag = 1;
                m_fResized = 1;
                Status = 1;
                //*aInBufSize = 0;
                break;
            }

            (*iFrameCount)++;
            Status = 1;

          //  if (m_DispStatus != MFC_GETOUTBUF_DISPLAY_ONLY)
            //    *aInBufSize = 0;

            break;

        default:
            Status = 0;
            printf("UnExpected Operation\n");
            break;
    }

    if (Status == 0)
    {
        *aOutBufSize = 0;
    }

    return Status;
}
int FrameSize=0;
Bool AvcHwDecoder_OMX::GetYuv(uint8* aOutBuf, uint32* aOutBufSize)
{
    SSBSIP_MFC_DEC_OUTPUT_INFO MfcOutInfo;

    m_DispStatus = SsbSipMfcDecGetOutBuf(m_hMfcHandle, &MfcOutInfo);
    if (m_DispStatus == MFC_GETOUTBUF_DISPLAY_DECODING || m_DispStatus == MFC_GETOUTBUF_DISPLAY_ONLY)
    {
        //LOGV("GetYuv() yaddress[0x%08x], caddress[0x%08x]\n", MfcOutInfo.YPhyAddr, MfcOutInfo.CPhyAddr);

        FrameSize = MfcOutInfo.img_width * MfcOutInfo.img_height;
     //   tile_to_linear_4x2_420(aOutBuf, (unsigned char *)MfcOutInfo.YVirAddr,(unsigned char *)MfcOutInfo.CVirAddr, MfcOutInfo.img_width, MfcOutInfo.img_height);
      // tile_to_linear_4x2(aOutBuf, (unsigned char *)MfcOutInfo.YVirAddr, MfcOutInfo.img_width, MfcOutInfo.img_height);
      // tile_to_linear_4x2(aOutBuf + FrameSize, (unsigned char *)MfcOutInfo.CVirAddr, MfcOutInfo.img_width, MfcOutInfo.img_height / 2);
/*#ifdef USE_MFC_FRAME_BUFFER
        if (m_fThumbnailMode)
        {
            tile_to_linear_4x2(aOutBuf, (unsigned char *)MfcOutInfo.YVirAddr, MfcOutInfo.img_width, MfcOutInfo.img_height);
            tile_to_linear_4x2(aOutBuf + FrameSize, (unsigned char *)MfcOutInfo.CVirAddr, MfcOutInfo.img_width, MfcOutInfo.img_height / 2);
        }
        else
        {
            oscl_memcpy(aOutBuf, &FrameSize, sizeof(FrameSize));
            oscl_memcpy(aOutBuf + sizeof(FrameSize), &(MfcOutInfo.YPhyAddr), sizeof(MfcOutInfo.YPhyAddr));
            oscl_memcpy(aOutBuf + sizeof(FrameSize) + (sizeof(void *) * 1), &(MfcOutInfo.CPhyAddr), sizeof(MfcOutInfo.CPhyAddr));
            oscl_memcpy(aOutBuf + sizeof(FrameSize) + (sizeof(void *) * 2), &(MfcOutInfo.YVirAddr), sizeof(MfcOutInfo.YVirAddr));
            oscl_memcpy(aOutBuf + sizeof(FrameSize) + (sizeof(void *) * 3), &(MfcOutInfo.CVirAddr), sizeof(MfcOutInfo.CVirAddr));
        }
#else*/
     //   oscl_memcpy(aOutBuf, MfcOutInfo.YVirAddr, FrameSize);
       // oscl_memcpy(aOutBuf + FrameSize, MfcOutInfo.CVirAddr, (FrameSize+1) >> 1);
//#endif
		
		oscl_memcpy(aOutBuf, &FrameSize, sizeof(FrameSize));
        oscl_memcpy(aOutBuf + sizeof(FrameSize), &(MfcOutInfo.YPhyAddr), sizeof(MfcOutInfo.YPhyAddr));
        oscl_memcpy(aOutBuf + sizeof(FrameSize) + (sizeof(void *) * 1), &(MfcOutInfo.CPhyAddr), sizeof(MfcOutInfo.CPhyAddr));
        oscl_memcpy(aOutBuf + sizeof(FrameSize) + (sizeof(void *) * 2), &(MfcOutInfo.YVirAddr), sizeof(MfcOutInfo.YVirAddr));
        oscl_memcpy(aOutBuf + sizeof(FrameSize) + (sizeof(void *) * 3), &(MfcOutInfo.CVirAddr), sizeof(MfcOutInfo.CVirAddr));
        *aOutBufSize = FrameSize + ((FrameSize + 1) >> 1);
    }
    else
    {
        //LOGE("SsbSipMfcDecGetOutBuf status is not MFC_GETOUTBUF_DISPLAYABLE (%d)\n", m_DispStatus);
        *aOutBufSize = 0;
        return 0;
    }

    return 1;
}

MFC_DECODE_STATUS AvcHwDecoder_OMX::MfcDecodeFrame(uint8 *indata, uint32 insize, OMX_TICKS intime, uint8 *outdata, uint32 *outsize, OMX_TICKS *outtime)
{
    int fVideoFrame = 0;
    char sz[256];
    SSBSIP_MFC_IMG_RESOLUTION displayResolution;
    int value;
    
    if (m_fMfcCreated == 0)
    {
        //LOGE("mfc codec not yet created \n");
        return MFC_DECODE_STATUS_NOT_CREATED;
    }

    if (m_fResized)
        m_fResized = 0;
		
    //LOGV("MfcDecodeSlice indata size is [%d]\n", insize);
    fVideoFrame = MfcGetVideoFlag(indata, insize);
    if (fVideoFrame < 0)
    {
        return MFC_DECODE_STATUS_DECODE_FAIL;
    }
    if (fVideoFrame == 1 && m_cMfcBuffSize != 0)
    {
        if (m_fMfcFirstFrameHandled == 0)
        {
            /* Decoder initialization through config stream and first I-frame */
            //LOGV("SsbSipMfcDecInit() m_cMfcBuffSize is [%d]\n", m_cMfcBuffSize);
            if (SsbSipMfcDecInit(m_hMfcHandle, H264_DEC, m_cMfcBuffSize) != MFC_RET_OK)
            {
                //LOGE("SsbSipMfcDecInit Failed: m_cMfcBuffSize is [%d]\n", m_cMfcBuffSize);
                return MFC_DECODE_STATUS_INIT_FAIL;
            }

            if (SsbSipMfcDecGetConfig(m_hMfcHandle, MFC_DEC_GETCONF_BUF_WIDTH_HEIGHT, &displayResolution) != MFC_RET_OK)
            {
                //LOGE("SsbSipMfcDecGetConfig failed");
                return MFC_DECODE_STATUS_INIT_FAIL;
            }
	
            iDisplay_Width = displayResolution.width;
            iDisplay_Height = displayResolution.height;
            m_fMfcFirstFrameHandled = 1;
            m_pMfcBuffCurr = m_pMfcBuffBase;
            m_cMfcBuffSize = 0;

            oscl_memcpy(m_pMfcBuffCurr, m_aDelimiter, 4);
            m_pMfcBuffCurr += 4;
            m_cMfcBuffSize += 4;

            oscl_memcpy(m_pMfcBuffCurr, indata, insize);
            m_pMfcBuffCurr += insize;
            m_cMfcBuffSize += insize;

            return MFC_DECODE_STATUS_DECODE_SUCCESS;
        }

        m_aTimestamp[m_iIndexTimestamp] = intime;
        SsbSipMfcDecSetConfig(m_hMfcHandle, MFC_DEC_SETCONF_FRAME_TAG, &m_iIndexTimestamp);
        m_iIndexTimestamp++;
        if (m_iIndexTimestamp >= MAX_TIMESTAMP)
            m_iIndexTimestamp = 0;

        //LOGV("SsbSipMfcDecExe() m_cMfcBuffSize is [%d]\n", m_cMfcBuffSize);
        if (SsbSipMfcDecExe(m_hMfcHandle, m_cMfcBuffSize) != MFC_RET_OK)
        {
            //LOGE("SsbSipMfcDecExe fail: m_cMfcBuffSize is [%d]\n", m_cMfcBuffSize);
            m_pMfcBuffCurr = m_pMfcBuffBase;
            m_cMfcBuffSize = 0;

            oscl_memcpy(m_pMfcBuffCurr, m_aDelimiter, 4);
            m_pMfcBuffCurr += 4;
            m_cMfcBuffSize += 4;

            oscl_memcpy(m_pMfcBuffCurr, indata, insize);
            m_pMfcBuffCurr += insize;
            m_cMfcBuffSize += insize;  

            return MFC_DECODE_STATUS_DECODE_FAIL;
        }
        GetYuv(outdata, outsize);
				
        if (SsbSipMfcDecGetConfig(m_hMfcHandle, MFC_DEC_GETCONF_FRAME_TAG, &value) != MFC_RET_OK)
        {
            //LOGE("SsbSipMfcDecGetConfig() MFC_DEC_GETCONF_FRAME_TAG fail\n");
        }

    /*    if (m_DispStatus == MFC_GETOUTBUF_DISPLAY_DECODING || m_DispStatus == MFC_GETOUTBUF_DISPLAY_ONLY)
        {
            *outtime = m_aTimestamp[value];
        }
        else
        {
            *outtime = intime;
        }
*/
        if (m_DispStatus != MFC_GETOUTBUF_DISPLAY_ONLY)
        {
            m_pMfcBuffCurr = m_pMfcBuffBase;
            m_cMfcBuffSize = 0;

            oscl_memcpy(m_pMfcBuffCurr, m_aDelimiter, 4);
            m_pMfcBuffCurr += 4;
            m_cMfcBuffSize += 4;

            oscl_memcpy(m_pMfcBuffCurr, indata, insize);
            m_pMfcBuffCurr += insize;
            m_cMfcBuffSize += insize;
        }
        	 
    }
    else
    {
        oscl_memcpy(m_pMfcBuffCurr, m_aDelimiter, 4);
        m_pMfcBuffCurr += 4;
        m_cMfcBuffSize += 4;

        oscl_memcpy(m_pMfcBuffCurr, indata, insize);
        m_pMfcBuffCurr += insize;
        m_cMfcBuffSize += insize;

        return MFC_DECODE_STATUS_NOT_PICTURE;
    }

    return MFC_DECODE_STATUS_DECODE_SUCCESS;
}

int AvcHwDecoder_OMX::MfcGetVideoFlag(uint8 *data, uint32 size)
{
    int fVideoFrame = 0; // 0 : only header  1 : I frame include
    int forbiddenZeroBit;
    char sz[256];

    if (size > 0)
    {
        forbiddenZeroBit = data[0] >> 7;
        if (forbiddenZeroBit != 0)
        {
            //LOGE("Strange Frame is delivered: Size[%u]", size);
            return -1;
        }

        if (((data[0] & 0x1F) == 1) ||
            ((data[0] & 0x1f) == 5))
        {
            if ((data[1] & 0x80) == 0x80)
                fVideoFrame = 1;
            else
                fVideoFrame = 0;
        }
        else
        {
            fVideoFrame = 0;
        }
    }

    return fVideoFrame;
}

int AvcHwDecoder_OMX::AvcDecDeinit_OMX()
{
#ifdef MFC_FPS
    //LOGE("[[ MFC_FPS ]] Decoding Time: %u, Frame Count: %d, Need Count: %d, FPS: %f\n",
       //     time, frame_cnt-1, need_cnt, (float)(frame_cnt-1)*1000/time);
#endif

    if (m_fMfcCreated == 0)
    {
        return 0;
    }

    if (SsbSipMfcDecClose(m_hMfcHandle) != MFC_RET_OK)
    {
        //LOGE("SsbSipMfcDecClose() failed\n");
        return -1;
    }

    m_pMfcBuffBase = NULL;
    m_pMfcBuffCurr = NULL;
    m_cMfcBuffSize = 0;
    m_fMfcFirstFrameHandled = 0;
    m_fMfcCreated = 0;
    m_fResized = 0;
    m_fThumbnailMode = 0;

    return 0;
}

void AvcHwDecoder_OMX::ResetDecoder(void)
{
    m_pMfcBuffCurr = m_pMfcBuffBase;
    m_cMfcBuffSize = 0;
}

int AvcHwDecoder_OMX::PVAVCDecGetNALType(uint8 *bitstream, int size,
        int *nal_type, int *nal_ref_idc)
{
    int forbidden_zero_bit;
    if (size > 0)
    {
        forbidden_zero_bit = bitstream[0] >> 7;
        if (forbidden_zero_bit != 0)
            return 0;
        *nal_ref_idc = (bitstream[0] & 0x60) >> 5;
        *nal_type = bitstream[0] & 0x1F;
        return 1;
    }

    return 0;
}

int AvcHwDecoder_OMX::PVAVCAnnexBGetNALUnit(uint8 *bitstream, uint8 **nal_unit,
        int *size)
{
    int i=0, j=0, FoundStartCode = 0;
    int end;
    char sz[256];
    i = 0;
    while (bitstream[i] == 0 && i < *size)
    {
        i++;
    }
    if (i >= *size)
    {
        *nal_unit = bitstream;
        return 0; /* cannot find any start_code_prefix. */
    }
    else if (bitstream[i] != 0x1)
    {
        i = -1;  /* start_code_prefix is not at the beginning, continue */
    }
    i++;
    *nal_unit = bitstream + i; /* point to the beginning of the NAL unit */

    j = end = i;
    while (!FoundStartCode)
    {
        while ((j + 1 < *size) && (bitstream[j] != 0 || bitstream[j+1] != 0))  /* see 2 consecutive zero bytes */
        {
            j++;
        }
        end = j;   /* stop and check for start code */
        while (j + 2 < *size && bitstream[j+2] == 0) /* keep reading for zero byte */
        {
            j++;
        }
        if (j + 2 >= *size)
        {
            *size -= i;
            return 2;  /* cannot find the second start_code_prefix */
        }
        if (bitstream[j+2] == 0x1)
        {
            FoundStartCode = 1;
        }
        else
        {
            /* could be emulation code 0x3 */
            j += 2; /* continue the search */
        }
    }
    *size = end - i;

    return 1;
}
int AvcHwDecoder_OMX::GetNextFullNAL_OMX(uint8** aNalBuffer, int32* aNalSize, uint8* aInputBuf, uint32* aInBufSize)
{
    uint32 BuffConsumed;
    uint8* pBuff = aInputBuf;
    uint32 InputSize;

    *aNalSize = *aInBufSize;
    InputSize = *aInBufSize;

    int ret_val = PVAVCAnnexBGetNALUnit(pBuff, aNalBuffer, aNalSize);

    if (ret_val == 0)
    {
        return 0;
    }

    BuffConsumed = ((*aNalSize) + (int32)(*aNalBuffer - pBuff));
    aInputBuf += BuffConsumed;
    *aInBufSize = InputSize - BuffConsumed;
    return 1;
}
#ifdef MFC_FPS
unsigned int AvcHwDecoder_OMX::measureTime(struct timeval *start, struct timeval *stop)
{
    unsigned int sec, usec, time;

    sec = stop->tv_sec - start->tv_sec;
    if(stop->tv_usec >= start->tv_usec)
    {
        usec = stop->tv_usec - start->tv_usec;
    }
    else
    {
        usec = stop->tv_usec + 1000000 - start->tv_usec;
        sec--;
    }
    time = sec*1000 + ((double)usec)/1000;
    return time;
}
#endif
