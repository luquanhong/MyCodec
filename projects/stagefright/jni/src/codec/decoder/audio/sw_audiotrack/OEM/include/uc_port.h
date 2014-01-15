#ifndef _UC_PORT_H_
#define _UC_PORT_H_

#include "stdlib.h"
#include "stdio.h"
#include "math.h"

//typedef unsigned int uint;
typedef int Bool;
typedef int	int32;
typedef unsigned int uint32;
typedef unsigned char uint8;
typedef short	 int16;
typedef unsigned short uint16;

typedef struct PARAM_PORTDEFINITIONTYPE {
    int32 nFrameWidth;                 /**< Size of the structure in bytes */
    int32 nFrameHeight;      /**< OMX specification version information */
} PARAM_PORTDEFINITIONTYPE;
typedef signed long long OMX_TICKS;
#define oscl_memcpy(dst,src,size)	memcpy(dst,src,size)
#define oscl_memset(ptr,val,size)	memset(ptr,val,size)
#define	oscl_memcmp(dst,src,size)	memcmp(dst,src,size)
#define	oscl_memmove(dst,src,size)	memmove(dst,src,size)
#define OSCL_DELETE						delete
#define	OSCL_NEW						new
#define OSCL_STATIC_CAST				static_cast
#define OSCL_ARRAY_DELETE(ptr)			delete [] ptr
#define OSCL_ARRAY_NEW(T, count)		new T[count]
#define	OSCL_ASSERT						
#define oscl_malloc(size)			malloc(size)
#define	oscl_free(ptr)				free(ptr)
#define	oscl_sqrt(x)				sqrt(x)
#define	oscl_pow(x,y)				pow(x,y)
#define	oscl_log(x)					log(x)
#define OSCL_IMPORT_REF
#define OSCL_EXPORT_REF

#define OSCL_UNUSED_ARG(vbl) (void)(vbl)	

#endif /* _UC_PORT_H_ */