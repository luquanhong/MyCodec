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
#ifndef AAC_DEC_H_INCLUDED
#define AAC_DEC_H_INCLUDED


#ifndef PVMP4AUDIODECODER_API_H
#include "pvmp4audiodecoder_api.h"
#endif

#define AACDEC_PCM_FRAME_SAMPLE_SIZE 1024 // 1024 samples 
/** PCM format description */ 
typedef struct AUDIO_PARAM_PCMMODETYPE { 
    uint32 nChannels;                /**< Number of channels (e.g. 2 for stereo) */ 
    uint32 nBitPerSample;            /**< Bit per sample */ 
    uint32 nSamplingRate;            /**< Sampling rate of the source data.  Use 0 for 
                                           variable or unknown sampling rate. */
} AUDIO_PARAM_PCMMODETYPE; 

/** AAC params */
typedef struct AUDIO_PARAM_AACPROFILETYPE {
    uint32 nChannels;             /**< Number of channels */
    uint32 nSampleRate;           /**< Sampling rate of the source data.  Use 0 for
                                        variable or unknown sampling rate. */
    uint32 nBitRate;              /**< Bit rate of the input data.  Use 0 for variable
                                        rate or unknown bit rates */
} AUDIO_PARAM_AACPROFILETYPE;

class OmxAacDecoder
{
    public:
        OmxAacDecoder();

        Bool AacDecInit(uint32 aDesiredChannels);

        void AacDecDeinit();

        Int AacDecodeFrames(int16* aOutputBuffer,
                            uint32* aOutputLength, uint8** aInBuffer,
                            uint32* aInBufSize, int32* aIsFirstBuffer,
                            AUDIO_PARAM_PCMMODETYPE* aAudioPcmParam,
                            AUDIO_PARAM_AACPROFILETYPE* aAudioAacParam,
                            int32* aSamplesPerFrame, Bool* aResizeFlag);

        void UpdateAACPlusEnabled(Bool flag);

        void ResetDecoder(); // for repositioning

        int32 iAacInitFlag;
        int32 iInputUsedLength;

    private:

        Int RetrieveDecodedStreamType();

        void*    ipMem;
        tPVMP4AudioDecoderExternal  iExt;
        uint32 iNumOfChannels;
        uint32  iMemReq;

};



#endif	//#ifndef AVC_DEC_H_INCLUDED

