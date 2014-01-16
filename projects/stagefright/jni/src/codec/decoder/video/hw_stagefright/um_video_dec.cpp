/*!
*	\file	offer the same interface for decode.
*/

#include <binder/IPCThreadState.h>
#include <media/stagefright/MetaData.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/MetaData.h>
#include <media/stagefright/Utils.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/OMXCodec.h>
#include <media/IOMX.h>
#include <utils/List.h>
//#include <new>
//#include <map>
//#include <gui/Surface.h>
#include <utils/RefBase.h>
#include <media/stagefright/OMXClient.h>
#include <media/stagefright/MediaSource.h>
#include <media/stagefright/MediaBuffer.h>
#include <media/stagefright/MediaBufferGroup.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "um.h"
#include "um_sys_thread.h"
#include "um_sys_log.h"

using namespace android;

struct Frame {
    int status;
    size_t size;
    int64_t time;
    int key;
    uint8_t *buffer;
//    AVFrame *vframe;
};

int pix_fmt;

struct StagefrightContext {


    sp<MediaSource> *source;
    List<Frame*> *in_queue, *out_queue;
    pthread_mutex_t in_mutex, out_mutex;
    pthread_cond_t condition;
    pthread_t decode_thread_id;

    Frame *end_frame;
    bool source_done;
    volatile sig_atomic_t  thread_exited, stop_decode;
    bool thread_started;
    
//    AVFrame *prev_frame;
//    std::map<int64_t, TimeStamp> *ts_map;
    int64_t frame_index;

    uint8_t *dummy_buf;
    int dummy_bufsize;

    OMXClient *client;
    sp<MediaSource> *decoder;
    const char *decoder_component;
};

class CustomSource : public MediaSource {
public:
    CustomSource(UM_VideoDecoderCtx* thiz,sp<MetaData> meta) {
    
        s = (StagefrightContext*)thiz->priv;
        source_meta = meta;
        frame_size  = (thiz->params.width * thiz->params.height * 3) / 2;
        buf_group.add_buffer(new MediaBuffer(frame_size));
    }

    virtual sp<MetaData> getFormat() {
        return source_meta;
    }

    virtual int start(MetaData *params) {
        return OK;
    }

    virtual int stop() {
        return OK;
    }

    virtual int read(MediaBuffer **buffer,
                          const MediaSource::ReadOptions *options) {
        Frame *frame;
        int ret;
        UMLOG_ERR("===CustomSource read");
#if 1
        if (s->thread_exited)
            return -1;
        pthread_mutex_lock(&s->in_mutex);

        UMLOG_ERR("===CustomSource read 1");
        while (s->in_queue->empty()){
            UMLOG_ERR("===CustomSource read 2");
            pthread_cond_wait(&s->condition, &s->in_mutex);
        }
        UMLOG_ERR("===CustomSource read 3");
        frame = *s->in_queue->begin();
        ret = frame->status;
        UMLOG_ERR("===CustomSource read data");
        if (ret == OK) {
            ret = buf_group.acquire_buffer(buffer);
            if (ret == OK) {
                memcpy((*buffer)->data(), frame->buffer, frame->size);
                UMLOG_ERR("===CustomSource read data 1 frame->size is %d", frame->size);
                (*buffer)->set_range(0, frame->size);
                UMLOG_ERR("===CustomSource read data 2");
                (*buffer)->meta_data()->clear();
                UMLOG_ERR("===CustomSource read data 3");
                //(*buffer)->meta_data()->setInt32(kKeyIsSyncFrame,frame->key);
                (*buffer)->meta_data()->setInt64(kKeyTime, 0);
                UMLOG_ERR("===CustomSource read data 4");
            } else {
                //av_log(s->avctx, AV_LOG_ERROR, "Failed to acquire MediaBuffer\n");
            }
            free(frame->buffer);
        }
    
        s->in_queue->erase(s->in_queue->begin());
        pthread_mutex_unlock(&s->in_mutex);
        UMLOG_ERR("===CustomSource read data end");
        free(frame);
#endif
        return ret;
    }

private:
    MediaBufferGroup buf_group;
    sp<MetaData> source_meta;
    StagefrightContext *s;
    int frame_size;
};


void* decode_thread(void *arg)
{

#if 1
    UMLOG_ERR("decode_thread create.");
    UM_VideoDecoderCtx *avctx = (UM_VideoDecoderCtx*)arg;
    StagefrightContext *priv = (StagefrightContext*)avctx->priv;
MediaSource::ReadOptions options;
    Frame* frame;
    MediaBuffer *buffer;
    int32_t w, h;
    int decode_done = 0;
    int ret;

    int64_t out_frame_index = 0;

    do {
        buffer = NULL;
        frame = (Frame*)malloc(sizeof(Frame));
        
        if (!frame) {
            frame = priv->end_frame;
            frame->status = -2;
            decode_done   = 1;
            priv->end_frame  = NULL;
            goto push_frame;
        }
        
        UMLOG_ERR("decode_thread read entery");
        frame->status = (*priv->decoder)->read(&buffer, &options);
        UMLOG_ERR("decode_thread read exit");
        if (frame->status == OK) {
            sp<MetaData> outFormat = (*priv->decoder)->getFormat();
            outFormat->findInt32(kKeyWidth , &w);
            outFormat->findInt32(kKeyHeight, &h);
            UMLOG_ERR("the data w is %d and h is %d", w, h);
            
            UMSint size = ((avctx->outWidth + 8) * (avctx->outHeight + 8) * 3) >> 1;
			frame->buffer = (uint8_t*)malloc(size);
            UMLOG_ERR("decode_thread 1");
            memcpy((uint8_t*)frame->buffer,(uint8_t*)buffer->data(), buffer->range_length());
            
            UMLOG_ERR("decode_thread 2");
            buffer->release();
        } else if (frame->status == INFO_FORMAT_CHANGED) {
            if (buffer)
                buffer->release();
            free(frame);
            continue;
        } else {
            decode_done = 1;
        }
push_frame:
        while (true) {
            pthread_mutex_lock(&priv->out_mutex);
            if (priv->out_queue->size() >= 10) {
                pthread_mutex_unlock(&priv->out_mutex);
                usleep(10000);
                continue;
            }
            break;
        }
        UMLOG_ERR("decode_thread 3");
        priv->out_queue->push_back(frame);
        pthread_mutex_unlock(&priv->out_mutex);
    } while (!decode_done && !priv->stop_decode);

    UMLOG_ERR("decode_thread read 4");
    priv->thread_exited = true;
#endif
}

UM_VideoDecoderCtx* um_vdec_create(UM_CodecParams* params)
{

    UM_VideoDecoderCtx* thiz = NULL;
//    StagefrightContext *s = (StagefrightContext*)avctx->priv_data;
    sp<MetaData> meta, outFormat;
    int32_t colorFormat = 0;
    int ret;
    int codeType;
    
    thiz = (UM_VideoDecoderCtx*)malloc(sizeof(UM_VideoDecoderCtx) + sizeof(StagefrightContext));
	
    StagefrightContext* priv = (StagefrightContext* )thiz->priv;
    memcpy(&thiz->params, params, sizeof(UM_CodecParams));
    
    UMLOG_ERR("um_vdec_create  priv is %p", priv);
    
    if(params->codec == CODEC_UID_MPEG4 )
    {
        UMLOG_INFO("the codeType is params->codec = CODEC_UID_MPEG4");
        codeType = 1;
    }
    else if(params->codec == CODEC_UID_H264)
    {
        UMLOG_INFO("the codeType is params->codec = CODEC_UID_H264");
        codeType = 2;	
        //avccmp = params->avccmp;
    }
    else
    {
        UMLOG_ERR("the codeType not support.");
        return UM_NULL;
    }

    meta = new MetaData;
    if (meta == NULL) {
        ret = -1;
        goto fail;
    }
    meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_AVC);
    meta->setInt32(kKeyWidth, params->width);
    meta->setInt32(kKeyHeight, params->height);
//    meta->setData(kKeyAVCC, kTypeAVCC, avctx->extradata, avctx->extradata_size);

    android::ProcessState::self()->startThreadPool();

    priv->source    = new sp<MediaSource>();
    *priv->source   = new CustomSource(thiz, meta);
    priv->in_queue  = new List<Frame*>;
    priv->out_queue = new List<Frame*>;
    priv->thread_started = false;
    //priv->ts_map    = new std::map<int64_t, TimeStamp>;
    priv->client    = new OMXClient;
    priv->end_frame = (Frame*)malloc(sizeof(Frame));
    
    if (priv->source == NULL || !priv->in_queue || !priv->out_queue || !priv->client ||
        !priv->end_frame) {
        ret = -1;
        goto fail;
    }

    if (priv->client->connect() !=  OK) {
        UMLOG_ERR("Cannot connect OMX client");
        ret = -1;
        goto fail;
    }

    priv->decoder  = new sp<MediaSource>();
    *priv->decoder = OMXCodec::Create(priv->client->interface(), meta,
                                  false, *priv->source, NULL,0,NULL);
                                  //OMXCodec::kClientNeedsFramebuffer);
    
    if ((*priv->decoder)->start() !=  OK) {
        UMLOG_ERR("Cannot start decoder");
        ret = -1;
        priv->client->disconnect();
        goto fail;
    }

    outFormat = (*priv->decoder)->getFormat();
    outFormat->findInt32(kKeyColorFormat, &colorFormat);

    UMLOG_ERR("kKeyColorFormat is %d", colorFormat);
#if 1
    if (colorFormat == OMX_QCOM_COLOR_FormatYVU420SemiPlanar ||
        colorFormat == OMX_COLOR_FormatYUV420SemiPlanar)
        pix_fmt = 1;
    else if (colorFormat == OMX_COLOR_FormatYCbYCr)
        pix_fmt = 2;
    else if (colorFormat == OMX_COLOR_FormatCbYCrY)
        pix_fmt = 3;
    else
        pix_fmt = 4;
#endif

    outFormat->findCString(kKeyDecoderComponent, &priv->decoder_component);
    if (priv->decoder_component){
        //priv->decoder_component = av_strdup(priv->decoder_component);
        UMLOG_ERR("kKeyDecoderComponent is %s", priv->decoder_component);
    }
    pthread_mutex_init(&priv->in_mutex, NULL);
    pthread_mutex_init(&priv->out_mutex, NULL);
    pthread_cond_init(&priv->condition, NULL);
    return thiz;

fail:

    free(priv->end_frame);
    delete priv->in_queue;
    delete priv->out_queue;
    //delete priv->ts_map;
    delete priv->client;
    
    return thiz;
}

static int start = 0;

//static int Stagefright_decode_frame(AVCodecContext *avctx, void *data,
//                                    int *got_frame, AVPacket *avpkt)
UMSint um_vdec_decode(UM_VideoDecoderCtx* thiz, UMSint8* buf, UMSint bufLen)
{
    UMLOG_ERR("um_vdec_decode 1");
#if 1
    StagefrightContext* priv = (StagefrightContext*)thiz->priv;
    Frame *frame;
    int status;


    UMLOG_ERR("um_vdec_decode 2 priv is %p", priv);
    if (start == 0) {
        UMLOG_ERR("um_vdec_decode 2.1");
        pthread_create(&priv->decode_thread_id, NULL, &decode_thread, thiz);
        UMLOG_ERR("um_vdec_decode 2.2");    
        start= 1;
    }
    UMSint size = ((thiz->outWidth + 8) * (thiz->outHeight + 8) * 3) >> 1;
    UMLOG_ERR("um_vdec_decode 3");
    if (!priv->source_done) {
        if(!priv->dummy_buf) {
            priv->dummy_buf = (uint8_t*)malloc(bufLen);
            if (!priv->dummy_buf)
                return -1;
            priv->dummy_bufsize = bufLen;
            memcpy(priv->dummy_buf, buf, bufLen);
        }
        UMLOG_ERR("um_vdec_decode 4");

        frame = (Frame*)malloc(sizeof(Frame));
        if (buf) {
            frame->status  = OK;
            frame->size    = bufLen;
            //frame->key     = avpkt->flags & AV_PKT_FLAG_KEY ? 1 : 0;
            frame->buffer  = (uint8_t*)malloc(bufLen);
            if (!frame->buffer) {
                free(frame);
                return -1;
            }
            uint8_t *ptr = (uint8_t *)buf;
            UMLOG_ERR("um_vdec_decode 5");
            memcpy(frame->buffer, ptr, bufLen);
  
        } else {
            frame->status  = ERROR_END_OF_STREAM;
            priv->source_done = true;
        }
        
        UMLOG_ERR("um_vdec_decode 6");

        while (true) {
            if (priv->thread_exited) {
                priv->source_done = true;
                break;
            }
            pthread_mutex_lock(&priv->in_mutex);
            if (priv->in_queue->size() >= 10) {
                pthread_mutex_unlock(&priv->in_mutex);
                usleep(10000);
                continue;
            }
            priv->in_queue->push_back(frame);
            pthread_cond_signal(&priv->condition);
            pthread_mutex_unlock(&priv->in_mutex);
            break;
        }
        UMLOG_ERR("um_vdec_decode 7");
    }
    
    UMLOG_ERR("um_vdec_decode 8");
    while (true) {
        pthread_mutex_lock(&priv->out_mutex);
        if (!priv->out_queue->empty()) break;
        pthread_mutex_unlock(&priv->out_mutex);
        if (priv->source_done) {
            usleep(10000);
            continue;
        } else {
            //return 0;
        }
    }

    UMLOG_ERR("um_vdec_decode 9");
    frame = *priv->out_queue->begin();
    priv->out_queue->erase(priv->out_queue->begin());
    pthread_mutex_unlock(&priv->out_mutex);

    UMLOG_ERR("um_vdec_decode 10");
    //ret_frame = frame->vframe;
    memcpy(thiz->outData,frame->buffer, size);
    status  = frame->status;
    free(frame->buffer);
    free(frame);

    UMLOG_ERR("um_vdec_decode 11");
    if (status == ERROR_END_OF_STREAM)
        return 0;
    if (status != OK) {
       
        UMLOG_ERR("Decode failed: %x\n", status);
        return -1;
    }


//    *got_frame = 1;
//    *(AVFrame*)data = *ret_frame;
    return 1;
#endif

//    return 0;
}

//int Stagefright_close(AVCodecContext *avctx)
UMSint um_vdec_destroy(UM_VideoDecoderCtx* thiz)
{

#if 0
    StagefrightContext *s = (StagefrightContext*)avctx->priv_data;
    Frame *frame;

    if (s->thread_started) {
        if (!s->thread_exited) {
            s->stop_decode = 1;

            // Make sure decode_thread() doesn't get stuck
            pthread_mutex_lock(&s->out_mutex);
            while (!s->out_queue->empty()) {
                frame = *s->out_queue->begin();
                s->out_queue->erase(s->out_queue->begin());
                if (frame->vframe)
                    av_frame_free(&frame->vframe);
                av_freep(&frame);
            }
            pthread_mutex_unlock(&s->out_mutex);

            // Feed a dummy frame prior to signalling EOF.
            // This is required to terminate the decoder(OMX.SEC)
            // when only one frame is read during stream info detection.
            if (s->dummy_buf && (frame = (Frame*)av_mallocz(sizeof(Frame)))) {
                frame->status = OK;
                frame->size   = s->dummy_bufsize;
                frame->key    = 1;
                frame->buffer = s->dummy_buf;
                pthread_mutex_lock(&s->in_mutex);
                s->in_queue->push_back(frame);
                pthread_cond_signal(&s->condition);
                pthread_mutex_unlock(&s->in_mutex);
                s->dummy_buf = NULL;
            }

            pthread_mutex_lock(&s->in_mutex);
            s->end_frame->status = ERROR_END_OF_STREAM;
            s->in_queue->push_back(s->end_frame);
            pthread_cond_signal(&s->condition);
            pthread_mutex_unlock(&s->in_mutex);
            s->end_frame = NULL;
        }

        pthread_join(s->decode_thread_id, NULL);

        if (s->prev_frame)
            av_frame_free(&s->prev_frame);

        s->thread_started = false;
    }

    while (!s->in_queue->empty()) {
        frame = *s->in_queue->begin();
        s->in_queue->erase(s->in_queue->begin());
        if (frame->size)
            av_freep(&frame->buffer);
        av_freep(&frame);
    }

    while (!s->out_queue->empty()) {
        frame = *s->out_queue->begin();
        s->out_queue->erase(s->out_queue->begin());
        if (frame->vframe)
            av_frame_free(&frame->vframe);
        av_freep(&frame);
    }

    (*s->decoder)->stop();
    s->client->disconnect();

    if (s->decoder_component)
        av_freep(&s->decoder_component);
    av_freep(&s->dummy_buf);
    av_freep(&s->end_frame);

    // Reset the extradata back to the original mp4 format, so that
    // the next invocation (both when decoding and when called from
    // av_find_stream_info) get the original mp4 format extradata.
    av_freep(&avctx->extradata);
    avctx->extradata = s->orig_extradata;
    avctx->extradata_size = s->orig_extradata_size;

    delete s->in_queue;
    delete s->out_queue;
    delete s->ts_map;
    delete s->client;
    delete s->decoder;
    delete s->source;

    pthread_mutex_destroy(&s->in_mutex);
    pthread_mutex_destroy(&s->out_mutex);
    pthread_cond_destroy(&s->condition);
    av_bitstream_filter_close(s->bsfc);
#endif
    return 0;
}

/*!
*	\brief	video init.
*/
UMSint um_vdec_init(UMSint8* packetName)
{
	
	return UMSuccess;
}

/*!
*	\brief	video finit.
*/
UMSint um_vdec_fini()
{
	return UMSuccess;
}

