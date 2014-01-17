/*!
 * \file	FILE NAME: UM_MediaPlayer.cpp
 *
 * Version: 1.0,  Date: 2010-11-02
 *
 * Description: ubvideo player imply
 *
 * Platform:
 *
 */

#include <OMX_Component.h>
#include <unistd.h>
#include <pthread.h>
#include <SoftwareRenderer.h>
#include <media/stagefright/MetaData.h>
#include <media/stagefright/MediaDefs.h>
#include <ui/PixelFormat.h>
#include <binder/IPCThreadState.h>
#include <gui/Surface.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/OMXCodec.h>
#include <utils/threads.h>
#include "UM_MediaPlayer.h"
#include "um_sys_log.h"

#define LOG_TAG "ummediaplayer"
#define UM_RENDER_MIN_INTERVAL 18
#define UM_RENDER_MAX_INTERVAL 33

static unsigned int m_frame_count = 0;
static ANativeWindow* m_pNativeWindow = NULL;

/*!
*	\brief 	The render class for hardware render.
*/
struct AwesomeNativeWindowRenderer : public AwesomeRenderer 
{
    AwesomeNativeWindowRenderer(const sp<ANativeWindow> &nativeWindow,
								int32_t rotationDegrees)
		: mNativeWindow(nativeWindow) 
	{
        applyRotation(rotationDegrees);
    }

/*!
*	\brief 	The render method for hardware render.
*/
	virtual void render(MediaBuffer *buffer) 
	{	
		int64_t timeUs;
		CHECK(buffer->meta_data()->findInt64(kKeyTime, &timeUs));
		native_window_set_buffers_timestamp(mNativeWindow.get(), timeUs * 1000);
		   
		if (buffer == NULL)
		{
			UMLOG_ERR("AwesomeNativeWindowRenderer buffer == NULL");
			return;
		}
		if (buffer->graphicBuffer() == NULL)
		{
			UMLOG_ERR("AwesomeNativeWindowRenderer buffer->graphicBuffer() == NULL");
			return;
		}
		
		if (buffer->graphicBuffer().get() == NULL)
		{
			UMLOG_ERR("AwesomeNativeWindowRenderer buffer->graphicBuffer().get() == NULL");
			return;
		}
		//UMLOG_ERR("mNativeWindow queueBuffer  buffer->range_length()=%d", buffer->range_length());
		status_t err = mNativeWindow->queueBuffer(
						mNativeWindow.get(), buffer->graphicBuffer().get(), -1);
		if (err != 0) 
		{
			UMLOG_ERR("queueBuffer failed with error %s (%d)", strerror(-err),-err);
			return;
		}

		sp<MetaData> metaData = buffer->meta_data();
		metaData->setInt32(kKeyRendered, 1);
	}

protected:
    virtual ~AwesomeNativeWindowRenderer() {}

private:
    sp<ANativeWindow> mNativeWindow;

    void applyRotation(int32_t rotationDegrees) 
	{
        uint32_t transform;
        switch (rotationDegrees) 
		{
            case 0: transform = 0; break;
            case 90: transform = HAL_TRANSFORM_ROT_90; break;
            case 180: transform = HAL_TRANSFORM_ROT_180; break;
            case 270: transform = HAL_TRANSFORM_ROT_270; break;
            default: transform = 0; break;
        }

        if (transform) 
		{
            CHECK_EQ(0, native_window_set_buffers_transform(mNativeWindow.get(), transform));
        }
    }

    AwesomeNativeWindowRenderer(const AwesomeNativeWindowRenderer &);
    AwesomeNativeWindowRenderer &operator=(
            const AwesomeNativeWindowRenderer &);
};

/*!
*	\brief	The render class for software render
*/
struct AwesomeLocalRenderer : public AwesomeRenderer 
{
    AwesomeLocalRenderer(
            const sp<ANativeWindow> &nativeWindow, const sp<MetaData> &meta)
        : mTarget(new SoftwareRenderer(nativeWindow, meta)) {}

/*!
*	\brief 	The render method for software render.it factually invoke the input render target to render 
*/
    virtual void render(MediaBuffer *buffer) 
	{
		UMLOG_ERR("AwesomeLocalRenderer buffer->range_offset()=%d, size=%d", buffer->range_offset(), buffer->range_length());	
        render((const uint8_t *)buffer->data() + buffer->range_offset(),
               buffer->range_length());
    }

    void render(const void *data, size_t size) 
	{
        mTarget->render(data, size, NULL);
    }

protected:
    virtual ~AwesomeLocalRenderer() 
	{
        delete mTarget;
        mTarget = NULL;
    }

private:
    SoftwareRenderer *mTarget;

    AwesomeLocalRenderer(const AwesomeLocalRenderer &);
    AwesomeLocalRenderer &operator=(const AwesomeLocalRenderer &);;
};


using namespace android;

/*!
*	\brief 	Release the input buffer memory 
*	\param 	The target buffer that will release.
*/
static void releaseBufferIfNonNULL(MediaBuffer **buffer)
{
    if(*buffer)
    {
        (*buffer)->release();
        *buffer = NULL;
    }
}

/*!
*	\brief 	Debug function that write input information to file.
*/
void printPlayerlogToFile(char* msg)
{
	FILE *fd=NULL;
	fd = fopen("/data/data/com.ubiLive.GameCloud/ubitus/ubplayer_log.txt", "a");
	if(fd!=NULL)
	{
		fwrite(msg,sizeof( char),strlen(msg),fd);
		fclose(fd); 
	}
}
FILE *fd=NULL;

/*!
*	\brief 	Constructor function of the UMMediaPlayer 
*	\param	codectype	The decode type 
						1: MPEG4
						2: H264
	\param	width	The video width
	\param	height	The video height
*/
UMMediaPlayer::UMMediaPlayer(int codectype, int width, int height, int avccmp): 
			mInitCheck(NO_INIT),
		 	codec_type(0), 
			mVideoWidth(0), 
			mVideoHeight(0), 
			mVideoPosition(0), 
			mPlaying(false), 
			mStopStatus(false),
			mHasDspError(0),
			mVideoScalingMode(NATIVE_WINDOW_SCALING_MODE_SCALE_TO_WINDOW),
			mIsStop(false)
			

{
	
    codec_type = codectype;
    mVideoWidth = width;
    mVideoHeight = height;
    status_t err = mClient.connect();
    if (err != OK)
    {
        UMLOG_ERR("Failed to connect to OMXClient.");
        return ;
    }
    
	fd = fopen("/sdcard/ubitus/decodedata.yuv", "a");
    
   // mInitCheck = OK;
	mVideoRenderer=NULL;
	mVideoSource=NULL;
	mVideoDecoder=NULL;
	UMLOG_ERR("UM_MediaPlayer()-------UMMediaSource start");
	mVideoSource = new UMMediaSource(codec_type,mVideoWidth,mVideoHeight, avccmp);
}

/*!
*	\brief 	Destructor function of the UMMediaPlayer 
*/
UMMediaPlayer::~UMMediaPlayer()
{
    
}

/*!
*	\brief 	Release the device instance 
*/
void UMMediaPlayer::release()
{

	if (mStopStatus == true)
	{
		UMLOG_ERR("----- UMMediaPlayer releas already stop");
		return;
	}
	
	mStopStatus = true;
	stop();

}

/*!
*	\brief 	Shut down the video decoder 
*/
void UMMediaPlayer::shutdownVideoDecoder()
{
    UMLOG_ERR("shutdownVideoDecoder start");	
    mVideoDecoder->stop();

    // The following hack is necessary to ensure that the OMX
    // component is completely released by the time we may try
    // to instantiate it again.
    wp<MediaSource> tmp = mVideoDecoder;
    mVideoDecoder.clear();
	
    while(tmp.promote() != NULL) 
	{
        usleep(200*1000);
    }
    IPCThreadState::self()->flushCommands();
    UMLOG_ERR("video decoder shutdown completed");
}

/*!
*	\brief 	The main method that used to decode data.
*			When the first invoke, it will start a decode thread.
*			The thread is use to read data from decode,and put them to render.
*/
void UMMediaPlayer::play(void)
{
	if(OK != mInitCheck || true == mIsStop|| 1 == mHasDspError || mPlaying) return;
	
    mPlaying = true;
	lastRenderTS = 0;

    if(mVideoDecoder != NULL)
    {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

        pthread_create(&mVideoThread, &attr, videoWrapper, this);

        pthread_attr_destroy(&attr);
    }
}

/*!
*	\brief Stop the decoder and delete the relationship instance 
*/
void UMMediaPlayer::stop()
{
	UMLOG_ERR("UMMediaPlayer::stop begin ");	

	if ((!mPlaying || true == mIsStop) && (0 == mHasDspError))
	{
		return ;
	}
	mHasDspError = 0;
	mPlaying = false;
	mIsStop = true;
	mVideoSource->stop();
	if(mVideoDecoder != NULL)
	{
		UMLOG_ERR("UM_MediaPlayer()-------stop 3 wait mVideoThread");
		void *dummy;
		pthread_join(mVideoThread, &dummy);	//!< stop the decode thread, and release the thread resource.
    }
	usleep(200*1000);
	
	finitRenderer();

	IPCThreadState::self()->flushCommands();
	mClient.disconnect();
}

/*!
*	\brief	Set video scalling mode when return error INFO_FORMAT_CHANGED.
*/
status_t UMMediaPlayer::setVideoScalingMode(int32_t mode)
{  
	mVideoScalingMode = mode;    
	if(mNativeWindow != NULL)
	{        
		status_t err = native_window_set_scaling_mode
		(                
			mNativeWindow.get(), mVideoScalingMode);        
			if(err != OK) {
				UMLOG_ERR("Failed to set scaling mode: %d", err);        
		}    
	}    
	return OK;
}

/*!
*	\brief	Prepare render for decoder, it will be invoke whill decoder create later.
*/
void UMMediaPlayer::initRenderer() 
{  
	if (mNativeWindow == NULL) 
	{        
		return;    
	}    
	sp<MetaData> meta = mVideoDecoder->getFormat();    
	int32_t format;    
	const char *component;    
	int32_t decodedWidth, decodedHeight;    
	CHECK(meta->findInt32(kKeyColorFormat, &format));    
	CHECK(meta->findCString(kKeyDecoderComponent, &component));    
	CHECK(meta->findInt32(kKeyWidth, &decodedWidth));    
	CHECK(meta->findInt32(kKeyHeight, &decodedHeight));    
	int32_t rotationDegrees;    
	if (!mVideoSource->getFormat()->findInt32(                
		kKeyRotation, &rotationDegrees)) 
	{        
		rotationDegrees = 0;    
	}    

	mVideoRenderer.clear();    
	// Must ensure that mVideoRenderer's destructor is actually executed    
	// before creating a new one.    
	IPCThreadState::self()->flushCommands(); 

	// Even if set scaling mode fails, we will continue anyway    
	setVideoScalingMode(mVideoScalingMode); 	//!< add at version 4.1
	
	if (!strncmp(component, "OMX.", 4)           
		&& strncmp(component, "OMX.google.", 11)            
		&& strcmp(component, "OMX.Nvidia.mpeg2v.decode")) 
	{       
		// Hardware decoders avoid the CPU color conversion by decoding      
		// directly to ANativeBuffers, so we must use a renderer that       
		// just pushes those buffers to the ANativeWindow.        
		mVideoRenderer =            
				new AwesomeNativeWindowRenderer(mNativeWindow, rotationDegrees);    
	} else {        
		// Other decoders are instantiated locally and as a consequence       
		// allocate their buffers in local address space.  This renderer       
		// then performs a color conversion and copy to get the data       
		// into the ANativeBuffer.        
		mVideoRenderer = new AwesomeLocalRenderer(mNativeWindow, meta);    
	}
}

void* UMMediaPlayer::videoWrapper(void *me)
{
    ((UMMediaPlayer*)me)->videoEntry();
    return NULL;
}

void printPDecodeDateToFile(char* msg)
{
	
	
	if(fd!=NULL)
	{
		fwrite(msg,sizeof( char),strlen(msg),fd);
		
	}
}

static int dcount;
/*!
*	\brief	The decode thread function. 
*			The main Loop is used to read data from decoder, and put them to render. 
*/
void UMMediaPlayer::videoEntry(void)
{
	bool eof = false;
	MediaBuffer *lastBuffer = NULL;
	MediaBuffer *buffer;
	MediaSource::ReadOptions options;
	mVideoStartTime = 0;
    dcount =0;
	UMLOG_ERR("videoEntry() ---- mVideoDecoder->start() begin");
	status_t err = mVideoDecoder->start();

	if(err != OK)
	{
		UMLOG_ERR("videoEntry() ---- mVideoDecoder->start() end failed err=%d", err);
		mHasDspError = 1;
		mPlaying = false;
		if(mVideoSource->HasAnyDataSource())
		{
			mVideoSource->read(&buffer, &options);
			releaseBufferIfNonNULL(&buffer);
		}
	}

	while(mPlaying && !mVideoSource->HasAnyDataSource())
	{
	   usleep(50 * 1000);
	}

	while(mPlaying)
	{
		status_t err = mVideoDecoder->read(&buffer, &options);
		options.clearSeekTo();

        
		if(err == INFO_FORMAT_CHANGED)
		{
			UMLOG_ERR("VideoSource signalled format change.");                                       
			if(mVideoRenderer != NULL) 
			{                   
				initRenderer();                
			}
			continue;
		}

		if(err != OK && buffer == NULL)
		{
			mHasDspError = 1;
			mPlaying = false;
			continue;
		}
        
        dcount++;
        printPDecodeDateToFile((char*)buffer->data());
        
        if(dcount == 10){
             if(fd != NULL){
                fclose(fd); 
    }
        }

		if(buffer == NULL)
		{
			usleep(3000);
			eof = true;		
			continue;
		}

		CHECK((err == OK && buffer != NULL) || (err != OK && buffer == NULL));
		if(err != OK)
		{
			eof = true;
			mPlaying = false;
			continue;
		}

		if(buffer->range_length() == 0)
		{
			buffer->release();
			buffer = NULL;
			continue;  
		}

		int64_t timeUs;
		CHECK(buffer->meta_data()->findInt64(kKeyTime, &timeUs));

		if(0 == timeUs)
		{
			unsigned int currTS = mVideoSource->um_util_getCurrentTick();

			if(currTS <= lastRenderTS + UM_RENDER_MAX_INTERVAL)
			{
				//There is a frame has been pushed into render in last UM_RENDER_MAX_INTERVAL ms
				//skip the compensate frame then
				buffer->release();
				buffer = NULL;
				continue;
			}
		/* push componsate frame to render */
		}

		displayOrDiscardFrame(&lastBuffer, buffer, 0);
	}
    
   

	releaseBufferIfNonNULL(&lastBuffer);
	shutdownVideoDecoder();
}

/*!
*	\brief	Pass the decoded data to the upper surface 
*/
void UMMediaPlayer::displayOrDiscardFrame(MediaBuffer **lastBuffer, MediaBuffer
    *buffer, int64_t pts_us)
{
	unsigned int currTS, sleepDur;
    if(buffer == NULL)
    {
    	 UMLOG_ERR("displayOrDiscardFrame buffer == NULL");
		return;		 
    }
	
    if(mVideoRenderer.get() != NULL)
    {
		//check the frame interval then
		currTS = mVideoSource->um_util_getCurrentTick();
		if(currTS < lastRenderTS + UM_RENDER_MIN_INTERVAL)
		{
			sleepDur = lastRenderTS + UM_RENDER_MIN_INTERVAL - currTS;
			if(sleepDur > UM_RENDER_MIN_INTERVAL) sleepDur = UM_RENDER_MIN_INTERVAL;
			
			usleep(1000 * sleepDur);
			lastRenderTS = currTS + sleepDur;
		}
		else
		{
			lastRenderTS = currTS;
		}
		
        sendFrameToISurface(buffer);
    }
    
	releaseBufferIfNonNULL(lastBuffer);   
	*lastBuffer = buffer;
}

/*!
*	\brief	Render the data that have been decoded.
*/
void UMMediaPlayer::sendFrameToISurface(MediaBuffer *buffer)
{
	if(mVideoRenderer == NULL)
	{
		UMLOG_ERR("sendFrameToISurface mVideoRenderer == NULL");	
	}
	mVideoRenderer->render(buffer);	
}

//static int testFrameCount = 0;
//char H264BlankFrame[] = {0x00, 0x00, 0x01, 0x0A};
// 0x00, 0x00, 0x01, 0x0A maybe
// 0x00, 0x00, 0x01, 0x2A maybe
// 0x00, 0x00, 0x01, 0x6A can't work
/*!
*	\brief 	Input the date that wanted to decode. 
*			when the first invoke, it will create decoder according to the buf param. 
*/
int UMMediaPlayer::setInputData(char* buf, int len, int pts)
{
	int ret;

    if(1 == mHasDspError)
	{
		UMLOG_ERR("UM_MediaPlayer::SetInputData() 1 == mHasDspError");
		return -1;
	}

	if(len < 8)
	{
		UMLOG_ERR("UM_MediaPlayer::SetInputData() len = %d", len);
		return 1;
	}

    ret = mVideoSource->SetInputData(buf, len, 0);

	if(mInitCheck == NO_INIT)
	{
		initDecode();
		mInitCheck = OK;
	}
	
    return ret;
}

/*!
*	\brief	It will be invoke when the first input decode data.
*/
void UMMediaPlayer::initDecode()
{
	setVideoDecoder(mVideoSource);
}

/*!
*	\brief	It will be invoke when the first input decode data.
*/
void UMMediaPlayer::setVideoDecoder(const sp <UMMediaSource >  &source)
{
	UMLOG_ERR("setVideoSource entry point");
	Mutex::Autolock autoLock(mLock);
	mVideoSource = source;

	UMLOG_ERR("setVideoSource source->getFormat();");
	sp < MetaData > meta = source->getFormat();

	bool success = meta->findInt32(kKeyWidth, &mVideoWidth);
	CHECK(success);

	success = meta->findInt32(kKeyHeight, &mVideoHeight);
	CHECK(success);
	UMLOG_ERR("setVideoSource width=%d,height=%d", mVideoWidth, mVideoHeight);


	mVideoDecoder = OMXCodec::Create(
            mClient.interface(), meta, false, //!< using the stagefright's OMXIL
            source,
            NULL, 0, mNativeWindow);
 	

	if (mVideoDecoder == NULL)
	{
		UMLOG_ERR("UM_MediaPlayer::setVideoSource is NULL");
		mInitCheck = NO_INIT;
		return ;
	}
	UMLOG_ERR("setVideoSource OMXCodec::Create successful!");
	
	if (mNativeWindow != NULL)
	{
		initRenderer();
	}

}

status_t UMMediaPlayer::setNativeWindow(const sp<ANativeWindow> &native) 
{
    mNativeWindow = NULL;//native;
    return OK;
}

/*!
*	\brief	Set the surface 
*	\param	surface	The surface Passed from upper 
*/
void UMMediaPlayer::setSurface(const sp <Surface>  &surface)
{
	setNativeWindow(surface);
}

/*!
*	\brief	Judget if the decoder is running. 
*/
bool UMMediaPlayer::isPlaying()const
{
    return mPlaying;
}

status_t UMMediaPlayer::initCheck()const
{
    return mInitCheck;
}

/*!
*	\brief	Release the Render resource. 
*/
void UMMediaPlayer::finitRenderer()
{
	if(mVideoRenderer!= NULL&&mVideoRenderer.get() != NULL)
	{
		UMLOG_ERR("mVideoRenderer.clear()");
		mVideoRenderer.clear();
	}	
}

