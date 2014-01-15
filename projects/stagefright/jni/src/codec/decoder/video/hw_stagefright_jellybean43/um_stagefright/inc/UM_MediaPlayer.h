/*!
 *	\file  FILE NAME: UM_MediaPlayer.h
 *
 *	 Version: 1.0,  Date: 2010-11-02
 *
 * 	Description: ubvideo player soure data
 *
 * 	Platform: 
 *
 ***************************************************************************/
#ifndef		UM_MEDIAPLAYER_IMPL_H_
#define		UM_MEDIAPLAYER_IMPL_H_

#include <media/MediaPlayerInterface.h>
#include <media/stagefright/OMXClient.h>
#include <utils/RefBase.h>
#include <utils/threads.h>
#include <gui/Surface.h>
#include <media/stagefright/MediaSource.h>
#include <media/stagefright/MediaBuffer.h>
#include <media/stagefright/MetaData.h>
#include <media/AudioTrack.h>

#include "UM_MediaSource.h"


using namespace android;

/*!
*	\brief	the pure virtual class of render 
*/
struct AwesomeRenderer : public RefBase 
{
    AwesomeRenderer() {}

    virtual void render(MediaBuffer *buffer) = 0;//!<	the pure virtual function for render

private:
    AwesomeRenderer(const AwesomeRenderer &);
    AwesomeRenderer &operator=(const AwesomeRenderer &);
};


/*!
*	\brief decode class		
*/
class UMMediaPlayer {
public:
    UMMediaPlayer(int codectype,int width,int height, int avccmp);
    ~UMMediaPlayer();

	
    void play();
    void pause();
    void release();
    void stop();
    bool isPlaying() const;
	status_t initCheck() const;
    void setSurface(const sp<Surface> &surface);
    int  setInputData(char* buf,int len,int pts);

    int32_t getWidth() const { return mVideoWidth; }
    int32_t getHeight() const { return mVideoHeight; }
	
private:
    status_t mInitCheck;
    bool mStopStatus;
	bool mIsStop;
	int	codec_type;
	int mHasDspError;
	int32_t mVideoWidth, mVideoHeight;
    int64_t mVideoPosition;
    int		  mVideoStartTime;

    bool mPlaying;

    OMXClient mClient;	//!<	it's use to communication with OMXMaster.
    sp<UMMediaSource> mVideoSource;		//!< the input data type for decode 
    sp<MediaSource> mVideoDecoder;		//!< the decode 			  
    sp<Surface> mSurface;				//!< the surface for display
    sp<AwesomeRenderer> mVideoRenderer;	//!< the render data type for display
    sp<ANativeWindow> mNativeWindow;
	
	//keep the render interval as 16 or 18ms to avoid fast-move issue
	unsigned int lastRenderTS;

    Mutex mLock;
    pthread_t mVideoThread;	//!< main data decode process thread  

	int32_t mDisplayWidth;
    int32_t mDisplayHeight;
	int32_t mVideoScalingMode;

	void initDecode();
	void shutdownVideoDecoder() ; 
	void initRenderer(); 
	void finitRenderer();
	 status_t setNativeWindow(const sp<ANativeWindow> &native);
    static void *videoWrapper(void *me);
    void videoEntry();
    void setVideoDecoder(const sp<UMMediaSource> &source);
    void displayOrDiscardFrame(
            MediaBuffer **lastBuffer, MediaBuffer *buffer, int64_t pts_us);
    void sendFrameToISurface(MediaBuffer *buffer);
	status_t setVideoScalingMode(int32_t mode);

    UMMediaPlayer(const UMMediaPlayer &);
    UMMediaPlayer &operator=(const UMMediaPlayer &);
};

#endif //UB_VIDEOPLAYER_IMPL_H_
