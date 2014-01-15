/*
============================================================================
Name		: um_ffmpeg_load.h
Author		: 
Version		: 
Copyright   :
Description : 
============================================================================
*/
#ifndef _UM_FFMPEG_LOAD_H_
#define _UM_FFMPEG_LOAD_H_

#include "libavformat/avformat.h"
#include "libswscale/swscale.h"

#ifdef __cplusplus
extern "C" {      
#endif 

//!< belong to libavcodec.so
AVCodec* (*um_avcodec_find_decoder)(enum AVCodecID id);
AVCodecContext* (*um_avcodec_alloc_context3)(const AVCodec *codec);
AVFrame* (*um_avcodec_alloc_frame)(void);
int (*um_avcodec_open2)(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options);
int (*um_avcodec_close)(AVCodecContext *avctx);
void (*um_av_init_packet)(AVPacket *pkt);
void (*um_avcodec_free_frame)(AVFrame **frame);
void (*um_avcodec_register_all)(void);
int (*um_avcodec_decode_video2)(AVCodecContext *avctx, AVFrame *picture,                         
								int *got_picture_ptr,          
								const AVPacket *avpkt);

int (*um_avcodec_decode_audio4)(AVCodecContext *avctx, AVFrame *frame,
								int *got_frame_ptr, const AVPacket *avpkt);								


//!< belong to libswcale.so
void (*um_sws_freeContext)(struct SwsContext *swsContext);  

//!< belong to libavutil.so

int (*um_av_samples_get_buffer_size)(int *linesize, int nb_channels, int nb_samples,
                               enum AVSampleFormat sample_fmt, int align);
void* (*um_av_malloc)(size_t size); 
void (*um_av_free)(void *ptr);

//!< the ffmpeg library handle.
void* um_libavutil;
void* um_libavcodec;
void* um_libswscale;
void* um_libavformat;

//!< the module interface. 
int um_get_lib_path(char** path);
int um_load_lib(const char* path);
void um_unload_lib();

#ifdef __cplusplus
};
#endif

#endif //_UM_FFMPEG_LOAD_H_