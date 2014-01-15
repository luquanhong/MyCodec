#include <media/AudioTrack.h>
#include <system/audio.h>
#include <pthread.h>
#include <sys/time.h>

//#include "um_types.h"
//#include "um_audio_dec.h"
#include "um.h"
#include "um_mm.h"
#include "um_sys_log.h"
#include "um_sys_event.h"
#include "um_sys_mutex.h"
#include "um_circle_list.h"
#include "up_sys_util.h"
#include "aac_dec.h"

typedef struct _OpenCoreADecPriv
{
	OmxAacDecoder* aac_dec;
	AUDIO_PARAM_PCMMODETYPE aPcmModeType;
	UMSint8 outBuf[4*1024];
}OpenCoreADecPriv;

typedef struct _Audio_data_entry_str_
{
    unsigned int size;
    char* data;
    unsigned int dataLen;
} Audio_data_entry_str, *PAudio_data_entry_str;

typedef enum _AudioPlayingStatus
{
    AudioRenderStatus_idle = 0,
    AudioRenderStatus_running = 1,
    AudioRenderStatus_stopping = 2,
    AudioRenderStatus_stopped = 3
} AudioRenderStatus;

#define UM_AUDIO_FRAME_BUFFER_SIZE 50
#define UM_AUDIO_10TO25_PERIOD 5000

static android::AudioTrack* m_upjni_audioTrack = NULL;
static pthread_t mAudioRenderThread;
static bool mAudioRenderThreadCreated = false;

static unsigned int m_um_audio_currTick_baseline = 0;
static unsigned int m_um_audio_10TO25_timeout = 0;

UmMutexHandle mMuxAudioPcm;
UmEventHandle mEvtAudioPcm;

pUMCircleList m_um_audio_circle_list = NULL;
pUMCircleList m_um_audio_circle_pool = NULL;

int mAudioRenderStatus = AudioRenderStatus_stopped;

static UMUint32 um_audio_countTick(void)
{
    struct timeval time_val;
    struct timezone junk;

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

static UMUint um_audio_getCurrentTick(void)
{
    return (um_audio_countTick() - m_um_audio_currTick_baseline);
}

static void um_audio_stopRender(void)
{
    if(AudioRenderStatus_stopping == mAudioRenderStatus ||
            AudioRenderStatus_stopped == mAudioRenderStatus)
    {
        return;
    }

    if(NULL == mMuxAudioPcm || NULL == mEvtAudioPcm)
    {
        return;
    }

    um_mutex_lock(mMuxAudioPcm);
    if(AudioRenderStatus_idle == mAudioRenderStatus)
    {
        um_event_notify(mEvtAudioPcm);
    }
    mAudioRenderStatus = AudioRenderStatus_stopping;
    um_mutex_unlock(mMuxAudioPcm);
}

static void* um_audio_copypcmToBuffer(void *me)
{
    PAudio_data_entry_str pentry = NULL;
    int quesize  = 0;

    while(AudioRenderStatus_running == mAudioRenderStatus) {
        if (!m_um_audio_circle_list)
        {
            usleep(1000);
            continue;
        }

        quesize = um_cl_list_count(m_um_audio_circle_list);
        if(quesize <= 0)
        {
            um_mutex_lock(mMuxAudioPcm);
            mAudioRenderStatus = AudioRenderStatus_idle;
            um_event_wait(mEvtAudioPcm, mMuxAudioPcm, UM_EVENT_WAIT_INFINITE);
            if(AudioRenderStatus_running != mAudioRenderStatus)
            {
                um_mutex_unlock(mMuxAudioPcm);
                break;
            }
            um_mutex_unlock(mMuxAudioPcm);
        }

        pentry = (PAudio_data_entry_str)um_cl_list_pop(m_um_audio_circle_list);

        if(pentry->data != NULL && pentry->dataLen > 0)
        {
            if(m_upjni_audioTrack != NULL)
            {
                if(m_upjni_audioTrack->stopped())
                {
                    m_upjni_audioTrack->start();
                }
                m_upjni_audioTrack->write(pentry->data, pentry->dataLen) ;
            }
        }
        um_cl_list_push(m_um_audio_circle_pool, (void*)pentry);

        usleep(1000);
    }

    mAudioRenderStatus = AudioRenderStatus_stopped;
    mAudioRenderThreadCreated = false;
    return NULL;
}

UMSint um_adec_init(UMSint8* pkgname)
{
    m_um_audio_10TO25_timeout = 0;

    m_um_audio_currTick_baseline = um_audio_countTick();

    if(!mMuxAudioPcm) mMuxAudioPcm = um_mutex_create();
    if(!mEvtAudioPcm) mEvtAudioPcm = um_event_create();

    if(!m_um_audio_circle_list && !m_um_audio_circle_pool)
    {
        m_um_audio_circle_list = um_cl_create(UM_AUDIO_FRAME_BUFFER_SIZE);
        m_um_audio_circle_pool = um_cl_create(UM_AUDIO_FRAME_BUFFER_SIZE);

        PAudio_data_entry_str pentry;
        int count = UM_AUDIO_FRAME_BUFFER_SIZE;
        while(--count >= 0)
        {
            pentry = (PAudio_data_entry_str)memset((void*)um_malloc(sizeof(Audio_data_entry_str)),
                    0, sizeof(Audio_data_entry_str));

            pentry->data = (char*)um_malloc(1024);
            pentry->size = 1024;
            um_cl_list_push(m_um_audio_circle_pool, (void*)pentry);
        }
    }
	return 0;
}

UMSint um_adec_fini()
{
    void *dummy;
    PAudio_data_entry_str pentry;

    um_audio_stopRender();
    while(AudioRenderStatus_stopped != mAudioRenderStatus)
    {
        usleep(5000);
    }

    if(m_um_audio_circle_list && m_um_audio_circle_pool)
    {
        while(NULL != (pentry = (PAudio_data_entry_str)um_cl_list_pop(m_um_audio_circle_list)))
        {
            um_cl_list_push(m_um_audio_circle_pool, (void*)pentry);
        }

        int count = UM_AUDIO_FRAME_BUFFER_SIZE;
        while(m_um_audio_circle_pool && --count >= 0)
        {
            pentry = (PAudio_data_entry_str)m_um_audio_circle_pool->list[count];
            um_free(pentry->data);
            um_free(pentry);
        }
        um_cl_destroy(m_um_audio_circle_pool);
        m_um_audio_circle_pool = NULL;

        um_cl_destroy(m_um_audio_circle_list);
        m_um_audio_circle_list = NULL;
    }

    if(mMuxAudioPcm)
    {
        um_mutex_destroy(mMuxAudioPcm);
        mMuxAudioPcm = NULL;
    }

    if(mEvtAudioPcm)
    {
        um_mutex_destroy(mEvtAudioPcm);
        mEvtAudioPcm = NULL;
    }

    pthread_join(mAudioRenderThread, &dummy);
    mAudioRenderStatus = AudioRenderStatus_stopped;
    mAudioRenderThreadCreated = false;

    if(m_upjni_audioTrack != NULL)
    {
        m_upjni_audioTrack->stop();
        m_upjni_audioTrack = NULL;
    }
	return 0;
}

/************************************************************************/
/* audio decoder                                                        */
/************************************************************************/
UM_AudioDecoderCtx* um_adec_create(UM_CodecParams* params)
{

	UMSint ret = -1;
	UM_AudioDecoderCtx* thiz = NULL;

	UMLOG_INFO("===audio have used the opencore audio decode module");
	if((thiz = (UM_AudioDecoderCtx*)um_calloc(1, sizeof(UM_AudioDecoderCtx) + sizeof(OpenCoreADecPriv))) != NULL)
	{
		OpenCoreADecPriv* priv = (OpenCoreADecPriv*)thiz->priv;
		
		memcpy(&thiz->params, params, sizeof(UM_CodecParams));
		switch(thiz->params.codec)
		{
			case CODEC_UID_AAC:
				if((priv->aac_dec = new OmxAacDecoder()) != NULL)
				{
					ret = (priv->aac_dec->AacDecInit(thiz->params.channels) == UM_FALSE) ? -1 : 0; //false = 0;
				}
				break;
		}
		if(ret == 0)
		{
			thiz->outData = priv->outBuf;
			memset(&priv->aPcmModeType, 0, sizeof(AUDIO_PARAM_PCMMODETYPE));
		}
		else
		{
			UMLOG_ERR("um_adec_create fail");
			um_adec_destroy(thiz);
			thiz = NULL;
		}
	}

	if(thiz != NULL)
	{
	    if(!m_upjni_audioTrack)
        {
	        UMLOG_INFO("um_adec_create, create AudioTrack, sampleRate: %d", thiz->params.sampleRate);

            m_upjni_audioTrack = new android::AudioTrack(
                                        AUDIO_STREAM_MUSIC,
                                        thiz->params.sampleRate,
                                        AUDIO_FORMAT_PCM_16_BIT,
            #if defined(ANDROID_VERSION_42_ADD)
                                        (audio_channel_mask_t)AUDIO_CHANNEL_OUT_STEREO,
            #else
                                        AUDIO_CHANNEL_OUT_STEREO,
            #endif
                                        0, 0, NULL, NULL, 0, 0);
        }

        if(!m_upjni_audioTrack)
        {
            UMLOG_ERR("Error creating AudioTrack");
            um_adec_destroy(thiz);
            return (thiz = NULL);
        }

        ret = m_upjni_audioTrack->initCheck();
        if(ret != 0)
        {
            UMLOG_ERR("Error init check AudioTrack");
            m_upjni_audioTrack->stop();
            m_upjni_audioTrack = NULL;
            um_adec_destroy(thiz);
            return (thiz = NULL);
        }
        m_upjni_audioTrack->start();

        if(!mAudioRenderThreadCreated)
        {
            UMLOG_INFO("um_adec_create, create audio render thread");

            mAudioRenderStatus = AudioRenderStatus_running;

            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
            ret = pthread_create(&mAudioRenderThread, &attr, um_audio_copypcmToBuffer, NULL);
            if(ret != 0)
            {
                UMLOG_ERR("Error create AudioTrack render thread! do something!");
            }
            pthread_attr_destroy(&attr);
            mAudioRenderThreadCreated = true;
        }
	}

	return thiz;
}

UMSint um_adec_destroy(UM_AudioDecoderCtx* thiz)
{
	OpenCoreADecPriv* priv = NULL;

	if(!thiz) return -1;
	
	priv = (OpenCoreADecPriv*)thiz->priv;
	if(priv->aac_dec)
	{
		priv->aac_dec->ResetDecoder();
		priv->aac_dec->AacDecDeinit();
		delete priv->aac_dec;
		priv->aac_dec = NULL;
	}
	um_free(thiz);

	return 0;
}

UMSint um_adec_decode(UM_AudioDecoderCtx* thiz, UMSint8* buf, UMSint bufLen)
{
	int ret, status;
	OpenCoreADecPriv* priv = NULL;
	UMSint leftLen = 0;
	static int32 aIsFirstBuffer = 0;
	int32 aSamplesPerFrame = 0;
	Bool aResizeFlag = 0;
    AUDIO_PARAM_AACPROFILETYPE aAacProfileType;

	if(!thiz || !buf || bufLen<=0) return -1;

	priv = (OpenCoreADecPriv*)thiz->priv;
	
	leftLen = bufLen;
	memset(&aAacProfileType, 0, sizeof(AUDIO_PARAM_AACPROFILETYPE));
	
	if((status = priv->aac_dec->AacDecodeFrames((int16*)thiz->outData,
									  (uint32*)&thiz->outDataLen, 
									  (uint8**)&buf, 
									  (uint32*)&leftLen,
									  &aIsFirstBuffer, 
									  &priv->aPcmModeType, 
									  &aAacProfileType, 
									  &aSamplesPerFrame, 
									  &aResizeFlag)) == 0)
	{
		thiz->outSample_rate = priv->aPcmModeType.nSamplingRate;
		thiz->outSample_fmt = priv->aPcmModeType.nBitPerSample;
		thiz->outChannels = priv->aPcmModeType.nChannels;
		thiz->outDataLen = 2*thiz->outDataLen;
		
        if (NULL == thiz->outData || thiz->outDataLen == 0)
        {
            UMLOG_ERR("um_adec_decode, decoded data is NULL or length is zero");
            return -1;
        }

        if(!m_um_audio_circle_list || !m_um_audio_circle_pool)
        {
            UMLOG_ERR("um_adec_decode, m_um_audio_circle_list is NULL or m_um_audio_circle_pool is NULL");
            return -1;
        }

        int quesize = um_cl_list_count(m_um_audio_circle_list);
        int count = 0;
        PAudio_data_entry_str pentry;
        UMLOG_ERR("um_adec_decode, m_um_audio_circle_list size: %d, decoded length: %d", quesize, thiz->outDataLen);
        if(quesize < 10)
        {
            m_um_audio_10TO25_timeout = 0;
        }
        else if(quesize >= 10 && quesize <= 25)
        {
            if(0 == m_um_audio_10TO25_timeout)
            {
                m_um_audio_10TO25_timeout = um_audio_getCurrentTick() + UM_AUDIO_10TO25_PERIOD;
            }
            else if(m_um_audio_10TO25_timeout > 0 && m_um_audio_10TO25_timeout < um_audio_getCurrentTick())
            {
                count = quesize - 5;
                while(--count >= 0)
                {
                    pentry = (PAudio_data_entry_str)um_cl_list_pop(m_um_audio_circle_list);
                    um_cl_list_push(m_um_audio_circle_pool, (void*)pentry);
                }
                m_um_audio_10TO25_timeout = 0;
            }
        }
        else if  (quesize > 25)
        {
            count = quesize - 10;
            while(--count >= 0)
            {
                pentry = (PAudio_data_entry_str)um_cl_list_pop(m_um_audio_circle_list);
                um_cl_list_push(m_um_audio_circle_pool, (void*)pentry);
            }
        }

        if(NULL != (pentry = (PAudio_data_entry_str)um_cl_list_pop(m_um_audio_circle_pool)))
        {
            if(thiz->outDataLen > pentry->size)
            {
                pentry->size = thiz->outDataLen;
                if(pentry->data) um_free(pentry->data);
                pentry->data = (char*)um_malloc(pentry->size);
            }
            memcpy((void*)pentry->data, thiz->outData, thiz->outDataLen);

            //set the data len
            pentry->dataLen = thiz->outDataLen;

            um_cl_list_push(m_um_audio_circle_list, (void*)pentry);

            if(AudioRenderStatus_idle == mAudioRenderStatus)
            {
                um_mutex_lock(mMuxAudioPcm);
                mAudioRenderStatus = AudioRenderStatus_running;
                um_event_notify(mEvtAudioPcm);
                um_mutex_unlock(mMuxAudioPcm);
            }
        }

		ret = 1;
	}
	else
	{
		UMLOG_ERR("=========AacDecodeFrames fail! decoded status: %d, aIsFirstBuffer: %d, thiz->outSample_rate: %d, thiz->outChannels: %d",
		        status, aIsFirstBuffer, thiz->outSample_rate, thiz->outChannels);
        ret = -1;
	}
	return ret;
}

UMSint um_adec_reset(UM_AudioDecoderCtx* thiz)
{
	if(!thiz) return -1;
	
	OpenCoreADecPriv* priv = NULL;
	
	priv = (OpenCoreADecPriv*)thiz->priv;
    priv->aac_dec->ResetDecoder();
	
	return 0;
}
