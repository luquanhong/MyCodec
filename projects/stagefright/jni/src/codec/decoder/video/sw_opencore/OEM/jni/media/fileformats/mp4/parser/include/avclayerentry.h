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

#ifndef __AVCLayerDescEntry_H__
#define __AVCLayerDescEntry_H__

#include "atom.h"
#include "atomutils.h"
#include "oscl_file_io.h"
#include "visualsampleentry.h"

class AVCLayerDescEntry : public VisualSampleEntry
{

    public:
        AVCLayerDescEntry(MP4_FF_FILE *fp, uint32 size, uint32 type);
        virtual ~AVCLayerDescEntry();

        uint8 getlayerNumber()
        {
            return _layerNumber;
        }

        uint8 getAccurateStatisticsFlag()
        {
            return _accurateStatisticsFlag;
        }

        uint16 getAvgBitRate()
        {
            return _avgBitRate;
        }

        uint16 getAvgFrameRate()
        {
            return _avgFrameRate;
        }


    private:
        uint8 _layerNumber;
        uint32 _duration;
        uint8 _accurateStatisticsFlag;
        uint16 _avgBitRate;
        uint16 _avgFrameRate;
};
#endif
