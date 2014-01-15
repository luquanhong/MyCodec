/***************************************************************************
 * FILE NAME: encoderwrap.c
 *
 * Version: 1.0,  Date: 2009-11-16
 *
 * Description: encoder wraper
 *
 * Platform: 
 *
 ***************************************************************************/

/***************************************************************************
 * Include Files                 	
 ***************************************************************************/

#include "encoderwrap.h"
#include "pvm4vencoder.h"
#include "uc_port.h"
#include "ccrgb16toyuv420.h"


encoderwraper_t	encoderwraper_new()
{
	encoderwraper_t pencoder;

	pencoder = (encoderwraper_t) malloc(sizeof(struct _encoderwraper_st));
	if(pencoder==NULL)
		return pencoder;
    memset(pencoder,0,sizeof(struct _encoderwraper_st));
	pencoder->iVideoEncoder=CPVM4VEncoder::New(0);
	if(!pencoder->iVideoEncoder)
	{	
		encoderwraper_free(pencoder);
		return NULL;
	}
	pencoder->iColorConverter=CCRGB16toYUV420::New();
    return pencoder;
}

void encoderwraper_free(encoderwraper_t pencoder)
{
	   if(pencoder == NULL)
		  return ;
	   printf("encoderwraper_free begin \n");
	   ((CPVM4VEncoder*)pencoder->iVideoEncoder)->Terminate();
        OSCL_DELETE ((CPVM4VEncoder*)pencoder->iVideoEncoder);
        pencoder->iVideoEncoder = NULL;
		delete ((CCRGB16toYUV420*)pencoder->iColorConverter);
		pencoder->iColorConverter=NULL;
		free(pencoder);
		 printf("encoderwraper_free end \n");
}

int encoderwraper_init(encoderwraper_t pencoder,int infmt,int inwidth,int inheight,int framerate,int bitrate,int cotenttype,int srcbitcount,int dstwidth,int dstheight)
{
	TPVVideoInputFormat iInputFormat;
    TPVVideoEncodeParam iEncodeParam;

	iInputFormat.iVideoFormat = (TPVVideoFormat)infmt;//ECVEI_YUV420;
    iInputFormat.iFrameWidth =  dstwidth;//DEFAULT_FRAME_WIDTH;
    iInputFormat.iFrameHeight = dstheight;//DEFAULT_FRAME_HEIGHT;
    iInputFormat.iFrameRate = (float)framerate;

    oscl_memset(&iEncodeParam, 0, sizeof(TPVVideoEncodeParam));
    iEncodeParam.iEncodeID = 0;
    iEncodeParam.iNumLayer = 1;
    iEncodeParam.iFrameWidth[0] = dstwidth;//DEFAULT_FRAME_WIDTH;
    iEncodeParam.iFrameHeight[0] =dstheight; //DEFAULT_FRAME_HEIGHT;
    iEncodeParam.iBitRate[0] = bitrate;//DEFAULT_BITRATE;
    iEncodeParam.iFrameRate[0] = (float)framerate;
    iEncodeParam.iFrameQuality = 10;
    iEncodeParam.iIFrameInterval = 256;
    iEncodeParam.iBufferDelay = (float)0.2;
    iEncodeParam.iContentType = (TPVContentType)cotenttype;//ECVEI_H263;
    iEncodeParam.iRateControlType = ECBR_1;
    iEncodeParam.iIquant[0] = 15;
    iEncodeParam.iPquant[0] = 12;
    iEncodeParam.iBquant[0] = 12;
    iEncodeParam.iSearchRange = 16;
    iEncodeParam.iMV8x8 = false;
    iEncodeParam.iPacketSize = 256;
    iEncodeParam.iNoCurrentSkip = false;
    iEncodeParam.iNoFrameSkip = false;
    iEncodeParam.iClipDuration = 0;
    iEncodeParam.iProfileLevel = ECVEI_CORE_LEVEL2;
	pencoder->iSrcFrameRate=framerate;
	pencoder->iContentType=cotenttype;
	//printf("init convert \n");
	if (((CPVM4VEncoder*)pencoder->iVideoEncoder)->Initialize(&iInputFormat, &iEncodeParam) == ECVEI_FAIL)
		 return 0;
	//printf("inwidth=%d,inheight=%d,srcbitcount=%d,dstwidth=%d,dstheight=%d\n",inwidth, inheight,srcbitcount, dstwidth, dstheight);

	((CCRGB16toYUV420*)pencoder->iColorConverter)->Init(inwidth, inheight, inwidth, dstwidth, dstheight, dstwidth, CCROTATE_NONE);
    ((CCRGB16toYUV420*)pencoder->iColorConverter)->SetMemHeight(dstheight);
    ((CCRGB16toYUV420*)pencoder->iColorConverter)->SetMode(1);
	//printf("init end \n");
	pencoder->init=1;
	 return 1;
}

int	encoderwraper_convert16toyuv420(encoderwraper_t pencoder,unsigned char* srcdata,unsigned char* dstdata)
{
	return ((CCRGB16toYUV420*)pencoder->iColorConverter)->Convert(srcdata,dstdata);
}
unsigned char* encoderwraper_getvolheader(encoderwraper_t pencoder,int* length)
{
	unsigned char* volHeader=NULL;
	if(pencoder->init <=0 )
		return NULL;
	if(((TPVContentType)pencoder->iContentType) == ECVEI_H263)
	{
		return NULL;
	}
	volHeader=(unsigned char*)malloc(32);
	memset(volHeader,0,32);
	if (((CPVM4VEncoder*)pencoder->iVideoEncoder)->GetVolHeader((uint8*)volHeader, length, /*layer*/0) == ECVEI_FAIL)
	{
		free(volHeader);
		return NULL;
	}
	if(((TPVContentType)pencoder->iContentType) == ECVEI_STREAMING)
    {
        volHeader[*length - 1] = 0x8F;
    }
    //else combined mode
    else if(((TPVContentType)pencoder->iContentType) == ECVEI_DOWNLOAD)
    {
        volHeader[*length - 1] = 0x1F;
    }

	return volHeader;
}

int	encoderwraper_encodeframe(encoderwraper_t pencoder,unsigned char* srcdata,unsigned char* dstdata,int dstlen)
{
	int length=0;
	int flag=0;
	TPVVideoInputData inputData;
	TPVVideoOutputData iEncoderOutput;
    inputData.iSource = srcdata;
    inputData.iTimeStamp = pencoder->iTimeStamp;
	pencoder->iTimeStamp+=1000/pencoder->iSrcFrameRate;
	printf("iTimeStamp=%d,dstlen=%d \n", pencoder->iTimeStamp,dstlen);
	if(pencoder->iSeq == 0)
	{
		unsigned char* volheader=NULL;
		volheader=encoderwraper_getvolheader(pencoder,&length);
		if(volheader)
		{
			memcpy(dstdata,volheader,length);
			free(volheader);
		}
		pencoder->iSeq++;
		flag=1;
	}
	iEncoderOutput.iBitStream = (uint8*)dstdata+length;
    oscl_memset(iEncoderOutput.iBitStream, 0, dstlen-length);
    iEncoderOutput.iBitStreamSize = dstlen-length;

	if (((CPVM4VEncoder*)pencoder->iVideoEncoder)->EncodeFrame(&inputData, &iEncoderOutput) == ECVEI_SUCCESS)
	{
		return iEncoderOutput.iBitStreamSize+length;
	}
	return 0;
}
