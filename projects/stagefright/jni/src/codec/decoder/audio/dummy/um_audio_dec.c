#include "um_audio_dec.h"
#include "um_sys_log.h"
#include <stdlib.h>

/************************************************************************/
/* audio decoder                                                        */
/************************************************************************/
UMSint um_adec_init(UMSint8* pkgname)
{
	LOG_LINE();
	return 0;
}

UMSint um_adec_fini()
{
	LOG_LINE();
	return 0;
}

UM_AudioDecoderCtx* um_adec_create(UM_CodecParams* params)
{
	LOG_LINE();
	return NULL;
}

UMSint um_adec_destroy(UM_AudioDecoderCtx* thiz)
{
	LOG_LINE();
	return 0;
}

UMSint um_adec_decode(UM_AudioDecoderCtx* thiz, UMSint8* buf, UMSint bufLen)
{
	LOG_LINE();
	return 0;
}

UMSint um_adec_reset(UM_AudioDecoderCtx* ctx)
{
	LOG_LINE();
	return 0;
}
