#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <binder/IServiceManager.h>
#include <media/IAudioVideo.h>
#include <media/IAudioVideoService.h>
#include <media/IOutputControl.h>
#include <utils/Log.h>
#include <utils/RefBase.h>
#include "um_audio_render.h"

#include <android/log.h> 
#define TAG "HW_NativeAudioMedia" 
#define NDEBUG

#ifdef NDEBUG
#define LOGV(...) 	__android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#else
#define LOGV(...)
#endif

//#define LL_AUDIO
using std::string;
using namespace android;


sp<AudioEngine>					gaudio_engine;
sp<IResourceContextService> 	agresource_context_service;
sp<IResourceContext> 			agcontext;


unsigned 	g_frequency = 44100;
unsigned 	g_sample_bits = 16;
unsigned 	g_num_channels = 2;
bool     	g_little_endian = true; //no sure  

class MyAudioSample : public DecompressedAudioChunk
{
public:
	MyAudioSample( unsigned frequency, unsigned sample_bits, unsigned num_channels, bool little_endian) : DecompressedAudioChunk(),
		m_frequency( frequency ),
		m_sample_bits( sample_bits ),
		m_num_channels( num_channels ),
		m_little_endian( little_endian )
	{
	}

	uint32_t GetSampleCount() const { return (m_size)/(m_num_channels*(m_sample_bits/8)); }
	uint8_t GetChannelCount() const { return m_num_channels; }
	uint8_t GetSampleBits() const { return m_sample_bits; }
	bool GetBigEndianSamples() const { return !m_little_endian; }
	uint32_t GetSampleRate() const { return m_frequency; }

	AVResult allocate_buffer( unsigned size )
	{
		m_size = size;
		return Allocate();
	}

	unsigned m_size;
	unsigned m_frequency;
	unsigned m_sample_bits;
	unsigned m_num_channels;
	bool     m_little_endian;

};

#if 1
class MyAVEngineNotifyTarget : public AVEngineNotifyTarget {

public:
	MyAVEngineNotifyTarget()
      : cb_num_decodecomplete_(0), cb_num_presentcomplete_(0), resources_ready_(false) { };
	
	bool NotifyDecodeComplete(
	  AVResult result,
	  const sp<CompressedSample>& encoded_sample,
	  const sp<DecompressedSample>& decoded_sample) 
	{
		cb_num_decodecomplete_++ ;
		LOGV("one frame is decoded");
		return false;
	}
	
	bool NotifyPresentComplete (
	AVResult result,
	const STCTimestamp& present_time,
	const sp<DecompressedSample>& decoded_sample,
	const sp<CompressedSample>& encoded_sample) 
	{
		cb_num_presentcomplete_++;
		LOGV("one frame is presented");
		return false;
	}
  
	void NotifyEngineReady(AVResult error) {
		if (error == kSuccess) {
			LOGV("%s:%d Got notification from the engine.", __FUNCTION__, __LINE__);
			resources_ready_ = true;
		} else {
			LOGV("%s:%d Got error notification from the engine.", __FUNCTION__, __LINE__);
			resources_ready_ = false;
		}
	}
	bool ready() { return resources_ready_; }
	uint32_t cb_num_decodecomplete_;
	uint32_t cb_num_presentcomplete_;
private:
	bool resources_ready_;
};

MyAVEngineNotifyTarget g_audio_notify_target;

#endif

int um_audio_init()
{
	LOGV("=======um_audio_init() enter 1");
	AVResult res;
	sp<android::IAVClock> m_clock;
	
	pollForService(String16("media.resourcecontextservice"), &agresource_context_service);
	agcontext = agresource_context_service->createResourceContext();

	
	LOGV("=======um_audio_init() agcontext->setPriority( eResourcePriorityUnKillable  );");
	agcontext->setPriority( eResourcePriorityUnKillable  ); // eResourcePriorityUnKillable eResourcePriorityHigh eResourcePriorityMedium );

	// Create clock
	m_clock = NULL;
	CreatePlatformAVClock( &m_clock, agcontext->getHandle() ) ;
	m_clock->SetClockRate( 0,1 );

	LOGV("=======um_audio_init() enter =3");
	CreatePlatformAudioEngineSp( &gaudio_engine, agcontext->getHandle() );
	LOGV("=======um_audio_init() enter =4");

	gaudio_engine->AssignNotifyTarget( NULL);//&g_audio_notify_target );

	//for low latency
	res = gaudio_engine->SetBoolOption(kAvapiEnableLowLatency, true );
	if( res != kSuccess )
	{
		//TODO: FIX to proper exception later
		LOGV("OnLive: Audio option 1 not supported\n" );
		//return -1;
	}
	else
	{
		LOGV("OnLive: Audio Option 1 supported\n" );
	}

	LOGV("=======um_audio_init() enter =5");
	gaudio_engine->SetUInt32Option( kAvapiAudDecContainerIndicatedSampleRate, g_frequency );
	gaudio_engine->SetUInt32Option( kAvapiAudDecPCMParamBitsPerSamp, g_sample_bits );
	gaudio_engine->SetUInt32Option( kAvapiAudDecContainerIndicatedChannels, g_num_channels );
	gaudio_engine->SetUInt32Option( kAvapiAudDecContainerIndicatedBitRate, g_frequency );
	gaudio_engine->SetUInt32Option( kAvapiAudDecWAVParamChannelCount, g_num_channels );
	
	gaudio_engine->SetUInt32Option( kAvapiAudDecPCMParamSampleRate, g_frequency );
	gaudio_engine->SetUInt32Option( kAvapiAudDecPCMParamChannelCount, g_num_channels );
	gaudio_engine->SetBoolOption( kAvapiAudDecPCMParamIsBigEndian, !g_little_endian );

	//for low latency
	gaudio_engine->SetUInt32Option( "kInputBufferSize", 15 * 1024 ); //first is 15*1024

	m_clock->SlamClockTime( 0 );
	m_clock->SetClockRate( 1, 1 );
		
	LOGV("=======um_audio_init() enter =6");
	gaudio_engine->Setup( kPCMAudio, false, true, m_clock );

	
	//m_clock->SetClockRate( 1, 1 );
	usleep(50*1000 ); // sleep for how long??
	
	//LOGV("gaudio_engine->SetOutputVolume( 100 )\n" );
	//gaudio_engine->SetOutputVolume( 100 );
	
	LOGV("=======um_audio_init() enter =7");	
	return UMSuccess;
}

int um_audio_fini()
{
	LOGV("release_audio_engine...");
	gaudio_engine->Reset();
	gaudio_engine = NULL;
	
	return UMSuccess;
}


int um_audio_render(unsigned char* buf, int bufLen)
{
	AVResult res;
//LOGV("=======um_audio_render() enter = buf is %s bufLen is %d",buf,bufLen);

	AudioSampleMetadata *md = new AudioSampleMetadata();

//LOGV("=======um_audio_render() enter =2");

	MyAudioSample *audio_sample = new MyAudioSample( g_frequency, g_sample_bits, g_num_channels, g_little_endian );
	
	while( audio_sample->allocate_buffer( bufLen ) != kSuccess )
	{
		LOGV("=======usleep( 10*1000 ); // sleep for how long??");
		usleep( 10*1000 ); // sleep for how long??
	}
//LOGV("=======um_audio_render() enter =3");
	if( audio_sample->GetSampleDataLen() != bufLen )
	{
		//TODO: THIS IS AN ERROR!
		LOGV("Audio buf size mismatch" );
		return -1;
	}
//LOGV("=======um_audio_render() enter =4");
	uint8_t *b = audio_sample->GetSampleData();
	memcpy( b, buf, bufLen );

	audio_sample->SetMetadata( md );
//LOGV("=======um_audio_render() enter =5 QueueForPresent");
	
	res = gaudio_engine->QueueForPresent( audio_sample );
	if( res != kSuccess )
	{
		LOGV( "=====queue audio failed\n" );
		return -1;
	}
//LOGV("=======um_audio_render() enter =6");
	
	return UMSuccess;
}
