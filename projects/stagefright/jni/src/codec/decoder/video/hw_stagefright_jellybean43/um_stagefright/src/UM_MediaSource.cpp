/*!
 *	\file FILE NAME: mpeg4source.cpp
 *
 * Version: 1.0,  Date: 2010-11-02
 *
 * Description: mpeg4 container soure data
 *
 * Platform: 
 *
 */
#include <stdint.h>
#include <sys/time.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/MetaData.h>
#include <media/stagefright/Utils.h>
#include <media/IOMX.h>
#include <utils/String8.h>

#include "um.h"
#include "um_sys_log.h"
#include "um_circle_queue.h"
#include "UM_MediaSource.h"


typedef uint8_t uint8;
typedef uint16_t uint16;
typedef int16_t int16;
typedef uint32_t uint32;
typedef int32_t int32;
typedef unsigned int uint;

#define LOG_TAG "UMMediaSource"

using namespace android;

#define SAMSUNG_GALAXYS2_ANDROID40 1
#define UM_MAX_COMPONSATE_NUM      16
#define NAL_START_CODE_SIZE		   4


int32 omx_m4v_getVideoHeader(int32 layer, uint8 *buf, int32 max_size);
int32 omx_m4v_getNextVideoSample(int32 layer_id, uint8 *buf, int32
								 max_buffer_size, UMUint32 *ts);

/*!
*	\brief	The start code of the MPEG Frame.
*			It's not used in current version. prepare for later version.
*/
static uint8 OMX_VOSH_START_CODE[] =
{
	0x00, 0x00, 0x01, 0xB0
};
static uint8 OMX_VO_START_CODE[] =
{
	0x00, 0x00, 0x01, 0x00
};
static uint8 OMX_VOP_START_CODE[] =
{
	0x00, 0x00, 0x01, 0xB6
};
static uint8 OMX_H263_START_CODE[] =
{
	0x00, 0x00, 0x80
};
static uint8 OMX_MPEG4_FAKE_FRAME[] =
{
	0x00, 0x00, 0x01, 0xB6, 0x50, 0x00, 0x23, 0xFF
};

static uint8 OMX_H264_FAKE_FRAME_CMP1[] =
{
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0xFF
};

static uint8 OMX_H264_FAKE_FRAME_CMP2[] =
{
	0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0xFF
};

static uint8 OMX_H264_FAKE_FRAME_CMP3[] =
{
	0x00, 0x00, 0x00, 0x01, 0x0C, 0x00, 0x00, 0xFF
};

static uint8 OMX_H264_FAKE_FRAME_CMP4[] =
{
	0x00, 0x00, 0x00, 0x01, 0x01, 0xE0, 0x62, 0x10
};


/*!
*	\brief	The type of the NAL.
*			It's not used in current version. prepare for later version.
*/
typedef enum
{
	NALTYPE_SLICE = 1,  /* non-IDR non-data partition */
	NALTYPE_DPA = 2,  /* data partition A */
	NALTYPE_DPB = 3,  /* data partition B */
	NALTYPE_DPC = 4,  /* data partition C */
	NALTYPE_IDR = 5,  /* IDR NAL */
	NALTYPE_SEI = 6,  /* supplemental enhancement info */
	NALTYPE_SPS = 7,  /* sequence parameter set */
	NALTYPE_PPS = 8,  /* picture parameter set */
	NALTYPE_AUD = 9,  /* access unit delimiter */
	NALTYPE_EOSEQ = 10,  /* end of sequence */
	NALTYPE_EOSTREAM = 11,  /* end of stream */
	NALTYPE_FILL = 12 /* filler data */
} NalUnitType;


static bool omx_short_video_header = false;

static unsigned int m_um_sys_currTick_baseline = 0;

/*!
*	\brief	get the system tick
*/
static UMUint32 get_countTick(void)
{
	struct timeval time_val;
	struct timezone junk;

	//unsigned long long totalMilliseconds;
	unsigned short al,bl,ah;
	unsigned int tmp,totalMillisecondsHi,totalMillisecondsLo;

	gettimeofday(&time_val, &junk);

    al = (unsigned short)time_val.tv_sec;
    bl = (unsigned short)1000;
    ah = (unsigned short)(time_val.tv_sec >> 16);    
   
	totalMillisecondsLo = al * bl;	
   
    tmp = ah * bl;				
    totalMillisecondsHi = tmp >> 16; 
    tmp <<= 16;
    totalMillisecondsLo += tmp;
    if (totalMillisecondsLo < tmp)
        ++totalMillisecondsHi; 
    totalMillisecondsLo += time_val.tv_usec / 1000;
    totalMillisecondsHi += (totalMillisecondsLo < time_val.tv_usec / 1000);
	
  	return (UMUint32)(totalMillisecondsLo&0xffffffff);
}

/*!
*	\brief	Get the baseline tick when mediasource instance create.
*/
void um_time_init()
{
	m_um_sys_currTick_baseline = get_countTick();
}


/*!
*	\brief	Get the relative tike from baseline tick.
*/
UMUint UMMediaSource::um_util_getCurrentTick(void)
{
	return (get_countTick() - m_um_sys_currTick_baseline);
}

/*!
*	\brief	Analysiz the MPEG Frame.
*			It's not used in current version. prepare for later version.
*/
int32 omx_m4v_getVideoHeader(int32 layer, uint8 *buf, int32 max_size)
{
	int32 count = 0;
	char my_sc[4];
	uint8 *tmp_bs = buf;

	memcpy(my_sc, tmp_bs, 4);
	my_sc[3] &= 0xf0;
	if (max_size >= 4)
	{
		if (memcmp(my_sc, OMX_VOSH_START_CODE, 4) && memcmp(my_sc,
			OMX_VO_START_CODE, 4))
		{
			count = 0;
			omx_short_video_header = true;
		}
		else
		{
			count = 0;
			omx_short_video_header = false;
			while (memcmp(tmp_bs + count, OMX_VOP_START_CODE, 4))
			{
				count++;
				if (count > 1000)
				{
					omx_short_video_header = true;
					break;
				}
			}
			if (omx_short_video_header == true)
			{
				count = 0;
				while (memcmp(tmp_bs + count, OMX_H263_START_CODE, 3))
				{
					count++;
				}
			}
		}
	}
	return count;
}

/*!
*	\brief	Analysiz the MPEG Frame.
*			It's not used in current version. prepare for later version.
*/
int32 omx_m4v_getNextVideoSample(int32 layer, uint8 *buf, int32
								 max_buffer_size, UMUint32 *timestamp)
{
	uint8 *tmp_bs = buf;
	int32 nBytesRead =  - 1;
	int32 skip = 0;
	int count = 0;
	int32 i, size;
	uint8 *ptr;

	if (max_buffer_size > 0)
	{
		skip = 1;
		do
		{
			if (omx_short_video_header)
			{
				size = max_buffer_size - skip;
				ptr = tmp_bs + skip;

				i = size;
				if (size < 1)
				{
					nBytesRead = 0;
					break;
				}

				while (i--)
				{
					if ((count > 1) && ((*ptr &0xFC) == 0x80))
					{
						i += 2;
						break;
					}

					if (*ptr++)count = 0;
					else
						count++;
				}
				nBytesRead = (size - (i + 1));
			}
			else
			{
				size = max_buffer_size - skip;
				ptr = tmp_bs + skip;

				i = size;
				if (size < 1)
				{
					nBytesRead = 0;
					break;
				}

				while (i--)
				{
					if ((count > 1) && (*ptr == 0x01))
					{
						i += 2;
						break;
					}

					if (*ptr++)
						count = 0;
					else
						count++;
				}
				nBytesRead = (size - (i + 1));


			}
			if (nBytesRead == 0)
				skip++;
		}
		while (nBytesRead == 0)
			;
		if (nBytesRead < 0)
		{
			nBytesRead = max_buffer_size - skip;
		}
		if (nBytesRead > 0)
		{
			nBytesRead += skip;
			max_buffer_size -= nBytesRead;
			if (max_buffer_size < 0)
				max_buffer_size = 0;
			timestamp[0] = 0xffffffff;
		}
	}
	return nBytesRead;
}

/*!
*	\brief	Analysiz the NAL of the H263 Frame.
*			It's not used in current version. prepare for later version.
*/
int PVAVCDecGetNALType(uint8 *bitstream, int size, int *nal_type,
					   int*nal_ref_idc)
{
	int forbidden_zero_bit;
	if (size > 0)
	{
		forbidden_zero_bit = bitstream[0] >> 7;
		if (forbidden_zero_bit != 0)
			return 0;
		*nal_ref_idc = (bitstream[0] &0x60) >> 5;
		*nal_type = bitstream[0] &0x1F;
		return 1;
	}
	return 0;
}

/*!
*	\brief	Analysiz the NAL of the H263 Frame.
*			It's not used in current version. prepare for later version.
*/
int PVAVCAnnexBGetNALUnit(uint8 *bitstream, uint8 **nal_unit, int *size)
{
	int i = 0, j = 0, FoundStartCode = 0;
	int end;
	char sz[256];
	i = 0;
	while (bitstream[i] == 0 && i <  *size)
	{
		i++;
	}
	if (i >=  *size)
	{
		*nal_unit = bitstream;
		return 0; /* cannot find any start_code_prefix. */
	}
	else if (bitstream[i] != 0x1)
	{
		i =  - 1; /* start_code_prefix is not at the beginning, continue */
	}
	i++;
	*nal_unit = bitstream + i; /* point to the beginning of the NAL unit */

	j = end = i;
	while (!FoundStartCode)
	{
		while ((j + 1 <  *size) && (bitstream[j] != 0 || bitstream[j + 1]
		!= 0))
			/* see 2 consecutive zero bytes */
		{
			j++;
		}
		end = j; /* stop and check for start code */
		while (j + 2 <  *size && bitstream[j + 2] == 0)
			/* keep reading for zero byte */
		{
			j++;
		}
		if (j + 2 >=  *size)
		{
			*size -= i;
			return 2; /* cannot find the second start_code_prefix */
		}
		if (bitstream[j + 2] == 0x1)
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

/*!
*	\brief	Analysiz the NAL of the H263 Frame.
*			It's not used in current version. prepare for later version.
*/
int GetNextFullNAL_OMX(uint8 **aNalBuffer, int32 *aNalSize, uint8*aInputBuf,
					   uint32 *aInBufSize)
{
	int32 BuffConsumed;
	uint8 *pBuff = aInputBuf;
	int32 InputSize;

	*aNalSize =  *aInBufSize;
	InputSize =  *aInBufSize;

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
/*!
*	\brief	Debug function.
*/
void printPreDecodelogToFile(char* msg)
{
	FILE *fd=NULL;
	fd = fopen("/data/data/com.ubiLive.GameCloud/ubitus/predecode_log.txt", "a");
	if(fd!=NULL)
	{
		fwrite(msg,sizeof( char),strlen(msg),fd);
		fclose(fd); 
	}
}

/*!
*	\brief	The UMMediaSource constructor 
*			It set some initializer params when it create.
*			Create the cycle queue for input data.
*			Set the baseline tick.
*/
UMMediaSource::UMMediaSource(int codectype1, int para1, int para2, int avccmp): 
	mGroup(NULL), 
	mStarted(false), 
	mBuffer(NULL), 
	mSize(0), 
	codec_type(CODEC_NONE), 
	mpara1(0), 
	mHeaderSent(false), 
	mcurrenttimeus(0), 
	timepos(0), 
	mpara2(0),
	mCodecSpecificDataSize(0),
	mCodecSpecificData(NULL),
	mMeta(NULL)
{

	UMLOG_ERR("UMMediaSource codectype1=%d, para1=%d, para2=%d", codectype1, para1, para2);
	codec_type = (codectype)codectype1;
	mpara1 = para1;
	mpara2 = para2;

	mMeta = new MetaData;
	setMetaData();	//!< Set the MetaData instance according to the input params.
	
	
	pCQ = um_cq_create(3);	//!< create the queue then
	pCQEntry = um_cq_createEntry();

	um_time_init(); //!< get the first tick, and set it as the baseline tick;

	if (codec_type <= 3)
	{
		mSize = mpara1 * mpara2*1.5;
	}
	else
	{
		mSize = 4096;
	}
	mAvccmp = avccmp;
}

/*!
*	\brief	The UMMediaSource destructor function.
*			It	releases all kinds of UMMediaSource's resource.
*/
UMMediaSource::~UMMediaSource()
{
	if (mStarted)
	{
		stop();
	}

	um_cq_destroy(pCQ);//!< release the CQ queue.
	um_cq_destroyEntry(pCQEntry);//!< release the cq entry.

	pCQ = NULL;
	pCQEntry = NULL;
	if(mCodecSpecificData) free(mCodecSpecificData);
	mCodecSpecificData = NULL;
	mCodecSpecificDataSize = 0;
}

/*!
*	\brief	Add MetaData to MediaBufferGroup.
*			It's not used in current version. prepare for later version.
*/
status_t UMMediaSource::start(MetaData *params)
{
	UMLOG_ERR("UMMediaSource start  mSize=%d", mSize);
	if (mStarted)
	{
		UMLOG_ERR("already started ");
		return UNKNOWN_ERROR;
	}
	mGroup = new MediaBufferGroup;
	mGroup->add_buffer(new MediaBuffer(mSize));
	UMLOG_ERR("UMMediaSource::start mGroup malloc finish");
	mStarted = true;
	return OK;
}

/*!
*	\brief	Stop the circle queue that used to story input data
*/
status_t UMMediaSource::stop()
{
	UMLOG_ERR("UMMediaSource stop ----------------1");
	if (!mStarted)
	{
		UMLOG_ERR("UMMediaSource stop ----------------2 error");
		return UNKNOWN_ERROR;
	}
	mStarted = false;

	um_cq_stop(pCQ);

	if (mBuffer)
		mBuffer->release();
	mBuffer = NULL;
	delete mGroup;
	mGroup = NULL;
	UMLOG_ERR("UMMediaSource stop ----------------3 exit");
	return OK;
}
/*!
*	\brief	Set the MetaData initialize variables according to the input params.
*			Them will be used by the decoder. 
*/
void UMMediaSource::setMetaData()
{
	int isvideo = 0;
	if (mMeta != NULL)
	{
		switch (codec_type)
		{
		case CODEC_MPEG4:
			mMeta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_MPEG4);
			isvideo = 1;
			break;
		case CODEC_H264:
			mMeta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_AVC);
			isvideo = 1;
			break;
		case CODEC_H263:
			mMeta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_H263);
			isvideo = 1;
			break;
		case CODEC_AAC:
			mMeta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_AAC);
			break;
		case CODEC_AMR_WB:
			mMeta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_AMR_WB);
			break;
		case CODEC_AMR_NB:
			mMeta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_AMR_NB);
			break;
		}

		if (isvideo == 1)
		{
			mMeta->setInt32(kKeyWidth, mpara1);
			mMeta->setInt32(kKeyHeight, mpara2);
			mMeta->setInt32(kKeyMaxInputSize, mSize);
			UMLOG_ERR("UMMediaSource::getFormat() width=%d, height=%d, MaxInputSize=%d",
				mpara1, mpara2, mSize);
			
			if(mCodecSpecificData != NULL)
			{
				UMLOG_ERR("getFormat mCodecSpecificDataSize =%d ",mCodecSpecificDataSize);
				mMeta->setData(kKeyAVCC, kTypeAVCC, mCodecSpecificData, mCodecSpecificDataSize);
				UMLOG_ERR("getFormat meta->setData mCodecSpecificData");
			}
		}
		else
		{
			mMeta->setInt32(kKeyChannelCount, mpara1);
			mMeta->setInt32(kKeySampleRate, mpara2);
			mMeta->setInt32(kKeyMaxInputSize, mSize);
		}
	}
}

/*!
*	\brief	Return the MetaData,it will be used when decoder create.
*	\return	MetaData 	Return the initialize meta.
*/
sp < MetaData > UMMediaSource::getFormat()
{
	return mMeta;
}

/*!
*	\brief	Get the MPEG4/H264 frame of data from circle queue.
			Add timestamp for each frame of data
*	\param	out	The output data buffer.
*	\param	options	it's not used here.
*	\return	fail:UNKNOWN_ERROR
*			success:OK
*/
status_t UMMediaSource::decoderPullVideoFrame(MediaBuffer** out, codectype codecType)
{
	status_t err;
	*out = NULL;
	
	if(timepos == 0)
	{
		timepos = um_util_getCurrentTick();//!< Get the current tick
	}

	if(mcurrenttimeus == 0)
	{
		mcurrenttimeus = um_util_getCurrentTick();
	}
	
	if(mBuffer == NULL && OK != (err = mGroup->acquire_buffer(&mBuffer)))
	{
		return err;
	}
	
	if(mStarted == false)
	{
		return UNKNOWN_ERROR;
	}

	if(um_cq_isEmpty(pCQ) && mMaxComponsateNum > 0 && codecType == CODEC_MPEG4)
	{
		//set the componsate frame then
		memcpy(mBuffer->data(), OMX_MPEG4_FAKE_FRAME, 8);
		mBuffer->set_range(0, 8);
		mcurrenttimeus = 0;
		mMaxComponsateNum--;
		//UMLOG_ERR("buffertest mCurrentNeedCompensateCount = %d", mCurrentNeedCompensateCount);
	}
	else if(um_cq_isEmpty(pCQ) && mMaxComponsateNum > 0 && codecType == CODEC_H264 && 0 < mAvccmp && 4 >= mAvccmp)
	{
		//set the componsate frame then
		switch (mAvccmp)
		{
			case 1:
				{
					memcpy(mBuffer->data(), OMX_H264_FAKE_FRAME_CMP1, 8);
				}
				break;
			case 2:
				{
					memcpy(mBuffer->data(), OMX_H264_FAKE_FRAME_CMP2, 8);
				}
				break;
			case 3:
				{
					memcpy(mBuffer->data(), OMX_H264_FAKE_FRAME_CMP3, 8);
				}
				break;
			case 4:
				{
					memcpy(mBuffer->data(), OMX_H264_FAKE_FRAME_CMP4, 8);
				}
				break;
			default:
				{
					UMLOG_ERR(" The H264 compensate frame format(%d) not support!",mAvccmp);
					return -1;
				}
				break;
				
		}
		mBuffer->set_range(0, 8);
		mcurrenttimeus = 0;
		mMaxComponsateNum--;
		//UMLOG_ERR("buffertest mCurrentNeedCompensateCount = %d", mCurrentNeedCompensateCount);
	}
	else
	{
		//PER LOG
		mMaxComponsateNum = UM_MAX_COMPONSATE_NUM;
		GetInputData();//!< Get one frame of data from circle queue.
		
		if(pCQEntry->dataLen <= 5 + pCQEntry->offset)
		{
			mBuffer->release();
			mBuffer = NULL;
			return UNKNOWN_ERROR;
		}

		mcurrenttimeus += (um_util_getCurrentTick() - timepos) * 1000;

		memcpy(mBuffer->data(), (pCQEntry->dataHeader + pCQEntry->offset), pCQEntry->dataLen - pCQEntry->offset);	//!< copy the frame of data to the data buffer of MetaData .
		mBuffer->set_range(0, pCQEntry->dataLen - pCQEntry->offset);
		pCQEntry->dataLen = pCQEntry->offset = 0;
	}
	
	//mcurrenttimeus += (um_util_getCurrentTick() - timepos) *1000;
	timepos = um_util_getCurrentTick();

	mBuffer->meta_data()->clear();
	mBuffer->meta_data()->setInt64(kKeyTime, mcurrenttimeus);	//!< Add timestap for echo frame of data.

	*out = mBuffer;	//!< Return the frame of data to the upper invoker
	mBuffer = NULL;
	
	return OK;
}

/*!
*	\brief	Release the buffer
*/
status_t UMMediaSource::release_mBuffer(void)
{
	if (mBuffer)
	{
		mBuffer->release();
		mBuffer = NULL;
	}
	return OK;
}

/*!
*	\brief	Return the data to be decoded in the form of the Meta class
*/
status_t UMMediaSource::read(MediaBuffer **out, const ReadOptions *options)
{
	if(!mStarted) return -1;
	
	return decoderPullVideoFrame(out, codec_type);
}

/*!
*	\brief	Get one frame of data from circle queue.
*/
void UMMediaSource::GetInputData(void)
{
	pUmCQEntry pentry;

	//UMLOG_ERR("[UbMediaSource::GetInputData] enter \r\n");
	if(pCQEntry->dataLen - pCQEntry->offset <= 5)
		//not a valid frame then
	{
		//to get a valid frame
		if(UM_NULL == (pentry = um_cq_popFilledEntry(pCQ)))
		{
			//something wrong here
			pCQEntry->dataLen = pCQEntry->offset = 0;
			return;
		}
		else
		{
			//swap buffer for performance concern
			char *tempBuffer;
			unsigned int bufferLen;

			//swap the buffer
			tempBuffer = pentry->dataHeader;
			pentry->dataHeader = pCQEntry->dataHeader;
			pCQEntry->dataHeader = tempBuffer;

			//swap the buffer size
			bufferLen = pentry->bufferSize;
			pentry->bufferSize = pCQEntry->bufferSize;
			pCQEntry->bufferSize = bufferLen;

			//for the other parameters
			pCQEntry->dataLen = pentry->dataLen;
			pCQEntry->offset = pentry->offset;
			pCQEntry->pts = pentry->pts;
			
		#if defined(UBGC_ENABLE_TIME_LOG)
			if(pentry->pts >0 )
			{
				char szDecodeBefore[128];
				sprintf(szDecodeBefore,"%d,\n",(upui_gc_getDelta()+um_util_getCurrentTick()));
				printPreDecodelogToFile(szDecodeBefore);
			}
		#endif
		
			//then push the entry back
			pentry->dataLen = 0;
			um_cq_pushEmptyEntry(pCQ, pentry);
		}
	}

	return ;
}

/*!
*	\brief	Add one frame of data to circle queue.
*/
int UMMediaSource::SetInputData(char *buf, int len, int pts)
{
	um_cq_pushData(pCQ, buf, (unsigned int)len, (unsigned int)pts);
	return 1;
}

int UMMediaSource::HasAnyDataSource(void)
{
    return !um_cq_isEmpty(pCQ);
}

