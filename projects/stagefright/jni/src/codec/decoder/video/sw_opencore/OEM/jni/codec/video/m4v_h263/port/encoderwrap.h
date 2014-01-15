/***************************************************************************
 * FILE NAME: 
 *
 * Version: 1.0,  Date: 2009-11-16
 *
 * Description: encoder wraper
 *
 * Platform: 
 *
 ***************************************************************************/

#ifndef OPENCORE_ENCODER_WRAPER_H_
#define OPENCORE_ENCODER_WRAPER_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef struct _encoderwraper_st *encoderwraper_t;

struct _encoderwraper_st {
	void*				iVideoEncoder;
	void*				iColorConverter;
	int					init;
	long				iTimeStamp;
	int					iSrcFrameRate;
	int					iContentType;
	int					iSeq;
};

/*******************sound  public functions ****************************/
encoderwraper_t					encoderwraper_new();  
int								encoderwraper_init(encoderwraper_t pencoder,int infmt,int inwidth,int inheight,int framerate,int bitrate,int cotenttype,int srcbitcount,int dstwidth,int dstheight);
int								encoderwraper_encodeframe(encoderwraper_t pencoder,unsigned char* srcdata,unsigned char* dstdata,int dstlen);
int								encoderwraper_convert16toyuv420(encoderwraper_t pencoder,unsigned char* srcdata,unsigned char* dstdata);
void							encoderwraper_free(encoderwraper_t pencoder);
unsigned char*					encoderwraper_getvolheader(encoderwraper_t pencoder);
#ifdef __cplusplus
}
#endif
#endif /*OPENCORE_ENCODER_WRAPER_H_*/