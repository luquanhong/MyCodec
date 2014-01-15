#include "um_audio_dec.h"
#include "um_sys_log.h"
#include <stdlib.h>

extern "C"
{
#include "ts_player.h"
}

/************************************************************************/
/* audio decoder                                                        */
/************************************************************************/
UMSint um_adec_init(UMSint8* packetName)
{
	return 0;
}

UMSint um_adec_fini()
{
	return 0;
}

UM_AudioDecoderCtx* um_adec_create(UM_CodecParams* params)
{
	return NULL;
}

UMSint um_adec_destroy(UM_AudioDecoderCtx* thiz)
{
	return 0;
}

UMSint um_adec_decode(UM_AudioDecoderCtx* thiz, UMSint8* buf, UMSint bufLen)
{
	int ret = write_audio_buffer((unsigned char*)buf, bufLen);
	UMLOG_INFO("NETCOM, write_audio_buffer(), finish, len=%d, ret=%d", bufLen, ret);
	return 0;
}

UMSint um_adec_reset(UM_AudioDecoderCtx* ctx)
{
	return 0;
}
