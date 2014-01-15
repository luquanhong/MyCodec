#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <dlfcn.h>
#include <unistd.h>
#include "um_load_ffmpeg.h"
#include "um_sys_log.h"

static char* um_extract_cpuinfo(char* buffer, int buflen, const char* field)
{
    int  fieldlen = strlen(field);
    char* bufend = buffer + buflen;
    char* result = NULL;
    int len, ignore;
    const char *p, *q;

    p = buffer;
    bufend = buffer + buflen;
    
	for (;;) 
	{
        p = memmem(p, bufend-p, field, fieldlen);
        if (p == NULL)
            goto EXIT;

        if (p == buffer || p[-1] == '\n')
            break;

        p += fieldlen;
    }

    p += fieldlen;
    p  = memchr(p, ':', bufend-p);
    if (p == NULL || p[1] != ' ')
        goto EXIT;

    p += 2;
    q = memchr(p, '\n', bufend-p);
    if (q == NULL)
        q = bufend;

    len = q-p;
    result = malloc(len+1);
    if (result == NULL)
        goto EXIT;

    memcpy(result, p, len);
    result[len] = '\0';

EXIT:
    return result;
}

static int um_has_item(const char* list, const char* item)
{
    const char*  p = list;
    int itemlen = strlen(item);

    if (list == NULL)
        return 0;

    while (*p) {
        const char*  q;

        while (*p == ' ' || *p == '\t')
            p++;

        q = p;
        while (*q && *q != ' ' && *q != '\t')
            q++;

        if (itemlen == q-p && !memcmp(p, item, itemlen))
            return 1;

        p = q;
    }
	
    return 0;
}

static int um_read_file(const char*  pathname, char*  buffer, size_t  buffsize)
{
    int  fd, len;

    fd = open(pathname, O_RDONLY);
    
	if (fd < 0)
        return -1;

    do 
    {
        
		len = read(fd, buffer, buffsize);
    } while (len < 0 && errno == EINTR);

    close(fd);

    return len;
}

static int um_has_neon(void)
{
	int	ret = 0;
    char cpuinfo[4096];
    int  cpuinfo_len;
	char* cpuFeatures;
	
    cpuinfo_len = um_read_file("/proc/cpuinfo", cpuinfo, sizeof(cpuinfo) );

    if (cpuinfo_len < 0)
        return 0;
	
	cpuFeatures = um_extract_cpuinfo(cpuinfo, cpuinfo_len, "Features");

	if (cpuFeatures != NULL) 
	{
		if (um_has_item(cpuFeatures, "neon")) 
		{
			ret = 1;
		}	
	}
  
	if(cpuFeatures) 
		free(cpuFeatures);
  
	return ret;
}

static int um_has_neon_lib(const char* path)
{
	int ret;
	char util[256];
	void* avutil;
	
	strcpy(util, path);
	strcpy(util + strlen(path), "/libavutil_neon_ub.so");
	
	if((avutil = dlopen(util,RTLD_NOW)) == NULL)
		return -1;
	
	if(avutil)
		dlclose(avutil);
		
	return 0;
}

/*!
*	\brief	the current path of running library
*	\param	
*		path	return the current path of running library;
*	\return	
*		if success, it will return 0;
*		if fail, it will return -1;
*/
int um_get_lib_path(char** path)
{	
	
	*path = (char *)malloc(1024);
	
	if(!*path)
	{
		 UMLOG_ERR("um_get_lib_path malloc memory fail.");
		 return -1;
	}
	
	getcwd(*path,1024);	
	UMLOG_INFO("um_get_lib_path : %s", *path);
	
	return 0;
}

/*!
*	\brief	load the ffmpeg library and get the  function handle that will be invoked.
*	\param	
*		path	the ffmpeg library path, it's set by libjni function m_upjni_setLibDir;
*	\return	
*		if success, it will return 0;
*		if fail, it will return -1;
*/
int um_load_lib(const char* path)
{

	int i;
	int len;
	char libname[3][256];
	char* defpath = "/data/data/com.ubiLive.GameCloud.webjs/lib";
	char paths[256];
	
	if( strncmp(path, "/data/data", 10) != 0 )
	{
		//UMLOG_ERR("=====um_load_lib fail=====");
		//return -1;
		len = strlen(defpath);
		strncpy(paths, defpath, len);		
	}
	else
	{
		len = strlen(path);
		strncpy(paths, path, len);
	}
	
	for(i = 0; i< 3; i++)
		strcpy(libname[i], paths);

	strcpy(libname[0] + len, "/libavutil_ub.so");
	strcpy(libname[1] + len, "/libavcodec_ub.so");
	strcpy(libname[2] + len, "/libswscale_ub.so");

	/*
	*	if have neon instruction, but have not ibavutil_neon_ub.so, 
	* 	it will choose to load libavutil_ub.so.
	*/
	if( (um_has_neon() == 1) && (um_has_neon_lib(path) == 0))
	{
		UMLOG_INFO("um_load_lib: um_has_neon() == true ");
		for(i = 0; i < 3; i++)
		{
			memset(libname[i], 0, 256);
			strncpy(libname[i], paths, len);
		}
		
		strcpy(libname[0] + len, "/libavutil_neon_ub.so");
		strcpy(libname[1] + len, "/libavcodec_neon_ub.so");
		strcpy(libname[2] + len, "/libswscale_neon_ub.so");
	}

	um_libavutil = dlopen(libname[0], RTLD_NOW);

	if(!um_libavutil)
	{
		UMLOG_ERR("Unable to load libavutil_ub.so: %s\n", dlerror());
		return -1;
	}
	UMLOG_INFO("ffmpeg have loaded %s\n", libname[0]);

	um_libavcodec = dlopen(libname[1], RTLD_NOW);

	if(!um_libavcodec)
	{
		UMLOG_ERR("Unable to load libavcodec_ub.so: %s\n", dlerror());
		dlclose(um_libavutil);
		return -1;
	}

	um_libswscale = dlopen(libname[2], RTLD_NOW);

	if(!um_libswscale)
	{
		UMLOG_ERR("Unable to load libswscale_ub.so: %s\n", dlerror());
		dlclose(um_libavutil);
		dlclose(um_libavcodec);
		return -1;
	}

	um_avcodec_find_decoder = dlsym(um_libavcodec, "avcodec_find_decoder");
	um_avcodec_alloc_context3 = dlsym(um_libavcodec, "avcodec_alloc_context3");
	um_avcodec_alloc_frame = dlsym(um_libavcodec, "avcodec_alloc_frame");
	um_avcodec_open2 = dlsym(um_libavcodec, "avcodec_open2");
	um_av_init_packet = dlsym(um_libavcodec, "av_init_packet");
	um_avcodec_close = dlsym(um_libavcodec, "avcodec_close");
	um_avcodec_free_frame = dlsym(um_libavcodec, "avcodec_free_frame");
	um_avcodec_register_all = dlsym(um_libavcodec, "avcodec_register_all");
	um_avcodec_decode_audio4 = dlsym(um_libavcodec, "avcodec_decode_audio4");
	um_avcodec_decode_video2 = dlsym(um_libavcodec, "avcodec_decode_video2");

	um_sws_freeContext = dlsym(um_libswscale, "sws_freeContext");

	um_av_samples_get_buffer_size = dlsym(um_libavutil, "av_samples_get_buffer_size");
	um_av_malloc = dlsym(um_libavutil, "av_malloc");
	um_av_free = dlsym(um_libavutil, "av_free");
	
	return 0;
}

/*!
*	\brief	unload and release the ffmpeg library resource.
*/
void um_unload_lib(){

	if(um_libavutil)
	{
		dlclose(um_libavutil);
		um_libavutil = NULL;
	}
		
	if(um_libavcodec)
	{
		dlclose(um_libavcodec);
		um_libavcodec = NULL;
	}
	if(um_libswscale)
	{
		dlclose(um_libswscale);
		um_libswscale = NULL;
	}
}