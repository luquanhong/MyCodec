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
#ifndef MPEG4_DEC_H_INCLUDED
#define MPEG4_DEC_H_INCLUDED

#ifndef _MP4DEC_API_H_
#include "mp4dec_api.h"
#endif

#ifndef _MP4DECLIB_H_
#include "mp4dec_lib.h"
#endif

#ifndef _M4VIO_H_
#include "m4v_io.h"
#endif




class Mpeg4Decoder_OMX
{
    public:

        Mpeg4Decoder_OMX();

        int32 InitializeVideoDecode(int32* aWidth, int32* aHeight,
                                      uint8** aBuffer, int32* aSize, int32 mode);

        int32 Mp4DecInit();

        Bool Mp4DecodeVideo(uint8* aOutBuffer, uint32* aOutputLength,
                                uint8** aInputBuf, uint32* aInBufSize,
                                PARAM_PORTDEFINITIONTYPE* aPortParam,
                                int32* aIsFirstBuffer, Bool aMarkerFlag, Bool *aResizeFlag);

        int32 Mp4DecDeinit();

        int32 Mpeg4InitFlag;

    private:
        MP4DecodingMode CodecMode;
        VideoDecControls VideoCtrl;

        uint8* pFrame0, *pFrame1;


        uint8 VO_START_CODE1[4];
        uint8 VOSH_START_CODE1[4];
        uint8 VOP_START_CODE1[4];
        uint8 H263_START_CODE1[3];
};


#endif ///#ifndef MPEG4_DEC_H_INCLUDED
