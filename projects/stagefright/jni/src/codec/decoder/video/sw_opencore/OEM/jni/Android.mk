# Copyright (C) 2011 The Ubitus GameCloud Native Library Project

include config.mk

LOCAL_PATH := $(call my-dir)

UTILS_PATH := $(LOCAL_PATH)/codec/utilities/
AUDIO_PATH := $(LOCAL_PATH)/codec/audio/
VIDEO_PATH := $(LOCAL_PATH)/codec/video/

#=====================================================================================================================================

include $(CLEAR_VARS)

LOCAL_MODULE := colorconvert

LOCAL_C_INCLUDES := \
	$(UTILS_PATH)colorconvert/include/ \
	$(UTILS_PATH)colorconvert/src/ \
	$(VIDEO_PATH)m4v_h263/port/

LOCAL_SRC_FILES := \
	$(UTILS_PATH)colorconvert/src/ccrgb16toyuv420.cpp \
 	$(UTILS_PATH)colorconvert/src/ccyuv422toyuv420.cpp \
 	$(UTILS_PATH)colorconvert/src/cczoomrotation16.cpp \
 	$(UTILS_PATH)colorconvert/src/cczoomrotation32.cpp \
 	$(UTILS_PATH)colorconvert/src/cczoomrotationbase.cpp \
 	$(UTILS_PATH)colorconvert/src/ccrgb24toyuv420.cpp \
 	$(UTILS_PATH)colorconvert/src/ccrgb12toyuv420.cpp \
 	$(UTILS_PATH)colorconvert/src/ccyuv420semiplnrtoyuv420plnr.cpp \
 	$(UTILS_PATH)colorconvert/src/ccyuv420toyuv420semi.cpp \
 	$(UTILS_PATH)colorconvert/src/yuv2rgb16tab.c \
	$(UTILS_PATH)colorconvert/src/yuv420rgb565.S \
 	$(UTILS_PATH)colorconvert/src/ccyuv420toyuv422.cpp

include $(BUILD_STATIC_LIBRARY)

#=====================================================================================================================================

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	$(AUDIO_PATH)aac/dec/src/analysis_sub_band.cpp \
	$(AUDIO_PATH)aac/dec/src/apply_ms_synt.cpp \
	$(AUDIO_PATH)aac/dec/src/apply_tns.cpp \
	$(AUDIO_PATH)aac/dec/src/buf_getbits.cpp \
	$(AUDIO_PATH)aac/dec/src/byte_align.cpp \
	$(AUDIO_PATH)aac/dec/src/calc_auto_corr.cpp \
	$(AUDIO_PATH)aac/dec/src/calc_gsfb_table.cpp \
	$(AUDIO_PATH)aac/dec/src/calc_sbr_anafilterbank.cpp \
	$(AUDIO_PATH)aac/dec/src/calc_sbr_envelope.cpp \
	$(AUDIO_PATH)aac/dec/src/calc_sbr_synfilterbank.cpp \
	$(AUDIO_PATH)aac/dec/src/check_crc.cpp \
	$(AUDIO_PATH)aac/dec/src/dct16.cpp \
	$(AUDIO_PATH)aac/dec/src/dct64.cpp \
	$(AUDIO_PATH)aac/dec/src/decode_huff_cw_binary.cpp \
	$(AUDIO_PATH)aac/dec/src/decode_noise_floorlevels.cpp \
	$(AUDIO_PATH)aac/dec/src/decoder_aac.cpp \
	$(AUDIO_PATH)aac/dec/src/deinterleave.cpp \
	$(AUDIO_PATH)aac/dec/src/digit_reversal_tables.cpp \
	$(AUDIO_PATH)aac/dec/src/dst16.cpp \
	$(AUDIO_PATH)aac/dec/src/dst32.cpp \
	$(AUDIO_PATH)aac/dec/src/dst8.cpp \
	$(AUDIO_PATH)aac/dec/src/esc_iquant_scaling.cpp \
	$(AUDIO_PATH)aac/dec/src/extractframeinfo.cpp \
	$(AUDIO_PATH)aac/dec/src/fft_rx4_long.cpp \
	$(AUDIO_PATH)aac/dec/src/fft_rx4_short.cpp \
	$(AUDIO_PATH)aac/dec/src/fft_rx4_tables_fxp.cpp \
	$(AUDIO_PATH)aac/dec/src/find_adts_syncword.cpp \
	$(AUDIO_PATH)aac/dec/src/fwd_long_complex_rot.cpp \
	$(AUDIO_PATH)aac/dec/src/fwd_short_complex_rot.cpp \
	$(AUDIO_PATH)aac/dec/src/gen_rand_vector.cpp \
	$(AUDIO_PATH)aac/dec/src/get_adif_header.cpp \
	$(AUDIO_PATH)aac/dec/src/get_adts_header.cpp \
	$(AUDIO_PATH)aac/dec/src/get_audio_specific_config.cpp \
	$(AUDIO_PATH)aac/dec/src/get_dse.cpp \
	$(AUDIO_PATH)aac/dec/src/get_ele_list.cpp \
	$(AUDIO_PATH)aac/dec/src/get_ga_specific_config.cpp \
	$(AUDIO_PATH)aac/dec/src/get_ics_info.cpp \
	$(AUDIO_PATH)aac/dec/src/get_prog_config.cpp \
	$(AUDIO_PATH)aac/dec/src/get_pulse_data.cpp \
	$(AUDIO_PATH)aac/dec/src/get_sbr_bitstream.cpp \
	$(AUDIO_PATH)aac/dec/src/get_sbr_startfreq.cpp \
	$(AUDIO_PATH)aac/dec/src/get_sbr_stopfreq.cpp \
	$(AUDIO_PATH)aac/dec/src/get_tns.cpp \
	$(AUDIO_PATH)aac/dec/src/getfill.cpp \
	$(AUDIO_PATH)aac/dec/src/getgroup.cpp \
	$(AUDIO_PATH)aac/dec/src/getics.cpp \
	$(AUDIO_PATH)aac/dec/src/getmask.cpp \
	$(AUDIO_PATH)aac/dec/src/hcbtables_binary.cpp \
	$(AUDIO_PATH)aac/dec/src/huffcb.cpp \
	$(AUDIO_PATH)aac/dec/src/huffdecode.cpp \
	$(AUDIO_PATH)aac/dec/src/hufffac.cpp \
	$(AUDIO_PATH)aac/dec/src/huffspec_fxp.cpp \
	$(AUDIO_PATH)aac/dec/src/idct16.cpp \
	$(AUDIO_PATH)aac/dec/src/idct32.cpp \
	$(AUDIO_PATH)aac/dec/src/idct8.cpp \
	$(AUDIO_PATH)aac/dec/src/imdct_fxp.cpp \
	$(AUDIO_PATH)aac/dec/src/infoinit.cpp \
	$(AUDIO_PATH)aac/dec/src/init_sbr_dec.cpp \
	$(AUDIO_PATH)aac/dec/src/intensity_right.cpp \
	$(AUDIO_PATH)aac/dec/src/inv_long_complex_rot.cpp \
	$(AUDIO_PATH)aac/dec/src/inv_short_complex_rot.cpp \
	$(AUDIO_PATH)aac/dec/src/iquant_table.cpp \
	$(AUDIO_PATH)aac/dec/src/long_term_prediction.cpp \
	$(AUDIO_PATH)aac/dec/src/long_term_synthesis.cpp \
	$(AUDIO_PATH)aac/dec/src/lt_decode.cpp \
	$(AUDIO_PATH)aac/dec/src/mdct_fxp.cpp \
	$(AUDIO_PATH)aac/dec/src/mdct_tables_fxp.cpp \
	$(AUDIO_PATH)aac/dec/src/mdst.cpp \
	$(AUDIO_PATH)aac/dec/src/mix_radix_fft.cpp \
	$(AUDIO_PATH)aac/dec/src/ms_synt.cpp \
	$(AUDIO_PATH)aac/dec/src/pns_corr.cpp \
	$(AUDIO_PATH)aac/dec/src/pns_intensity_right.cpp \
	$(AUDIO_PATH)aac/dec/src/pns_left.cpp \
	$(AUDIO_PATH)aac/dec/src/ps_all_pass_filter_coeff.cpp \
	$(AUDIO_PATH)aac/dec/src/ps_all_pass_fract_delay_filter.cpp \
	$(AUDIO_PATH)aac/dec/src/ps_allocate_decoder.cpp \
	$(AUDIO_PATH)aac/dec/src/ps_applied.cpp \
	$(AUDIO_PATH)aac/dec/src/ps_bstr_decoding.cpp \
	$(AUDIO_PATH)aac/dec/src/ps_channel_filtering.cpp \
	$(AUDIO_PATH)aac/dec/src/ps_decode_bs_utils.cpp \
	$(AUDIO_PATH)aac/dec/src/ps_decorrelate.cpp \
	$(AUDIO_PATH)aac/dec/src/ps_fft_rx8.cpp \
	$(AUDIO_PATH)aac/dec/src/ps_hybrid_analysis.cpp \
	$(AUDIO_PATH)aac/dec/src/ps_hybrid_filter_bank_allocation.cpp \
	$(AUDIO_PATH)aac/dec/src/ps_hybrid_synthesis.cpp \
	$(AUDIO_PATH)aac/dec/src/ps_init_stereo_mixing.cpp \
	$(AUDIO_PATH)aac/dec/src/ps_pwr_transient_detection.cpp \
	$(AUDIO_PATH)aac/dec/src/ps_read_data.cpp \
	$(AUDIO_PATH)aac/dec/src/ps_stereo_processing.cpp \
	$(AUDIO_PATH)aac/dec/src/pulse_nc.cpp \
	$(AUDIO_PATH)aac/dec/src/pv_div.cpp \
	$(AUDIO_PATH)aac/dec/src/pv_log2.cpp \
	$(AUDIO_PATH)aac/dec/src/pv_normalize.cpp \
	$(AUDIO_PATH)aac/dec/src/pv_pow2.cpp \
	$(AUDIO_PATH)aac/dec/src/pv_sine.cpp \
	$(AUDIO_PATH)aac/dec/src/pv_sqrt.cpp \
	$(AUDIO_PATH)aac/dec/src/pvmp4audiodecoderconfig.cpp \
	$(AUDIO_PATH)aac/dec/src/pvmp4audiodecoderframe.cpp \
	$(AUDIO_PATH)aac/dec/src/pvmp4audiodecodergetmemrequirements.cpp \
	$(AUDIO_PATH)aac/dec/src/pvmp4audiodecoderinitlibrary.cpp \
	$(AUDIO_PATH)aac/dec/src/pvmp4audiodecoderresetbuffer.cpp \
	$(AUDIO_PATH)aac/dec/src/q_normalize.cpp \
	$(AUDIO_PATH)aac/dec/src/qmf_filterbank_coeff.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_aliasing_reduction.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_applied.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_code_book_envlevel.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_crc_check.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_create_limiter_bands.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_dec.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_decode_envelope.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_decode_huff_cw.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_downsample_lo_res.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_envelope_calc_tbl.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_envelope_unmapping.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_extract_extended_data.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_find_start_andstop_band.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_generate_high_freq.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_get_additional_data.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_get_cpe.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_get_dir_control_data.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_get_envelope.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_get_header_data.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_get_noise_floor_data.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_get_sce.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_inv_filt_levelemphasis.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_open.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_read_data.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_requantize_envelope_data.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_reset_dec.cpp \
	$(AUDIO_PATH)aac/dec/src/sbr_update_freq_scale.cpp \
	$(AUDIO_PATH)aac/dec/src/set_mc_info.cpp \
	$(AUDIO_PATH)aac/dec/src/sfb.cpp \
	$(AUDIO_PATH)aac/dec/src/shellsort.cpp \
	$(AUDIO_PATH)aac/dec/src/synthesis_sub_band.cpp \
	$(AUDIO_PATH)aac/dec/src/tns_ar_filter.cpp \
	$(AUDIO_PATH)aac/dec/src/tns_decode_coef.cpp \
	$(AUDIO_PATH)aac/dec/src/tns_inv_filter.cpp \
	$(AUDIO_PATH)aac/dec/src/trans4m_freq_2_time_fxp.cpp \
	$(AUDIO_PATH)aac/dec/src/trans4m_time_2_freq_fxp.cpp \
	$(AUDIO_PATH)aac/dec/src/unpack_idx.cpp \
	$(AUDIO_PATH)aac/dec/src/window_tables_fxp.cpp \
	$(AUDIO_PATH)aac/dec/src/pvmp4setaudioconfig.cpp \
	$(VIDEO_PATH)m4v_h263/port/aac_decode_frame.cpp

LOCAL_MODULE := aac_dec

LOCAL_CFLAGS := -DAAC_PLUS -DHQ_SBR -DPARAMETRICSTEREO

ifeq ($(TARGET_ARCH),arm)
	LOCAL_CFLAGS += -D_ARM_GCC
	LOCAL_ARM_MODE := arm
else
	LOCAL_CFLAGS += -DC_EQUIVALENT
endif

LOCAL_C_INCLUDES := \
	$(AUDIO_PATH)aac/dec/include/ \
	$(AUDIO_PATH)aac/dec/src/ \
	$(VIDEO_PATH)m4v_h263/port/

include $(BUILD_STATIC_LIBRARY)

#=====================================================================================================================================
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	$(VIDEO_PATH)avc_h264/dec/src/avcdec_api.cpp \
 	$(VIDEO_PATH)avc_h264/dec/src/avc_bitstream.cpp \
 	$(VIDEO_PATH)avc_h264/dec/src/header.cpp \
 	$(VIDEO_PATH)avc_h264/dec/src/itrans.cpp \
 	$(VIDEO_PATH)avc_h264/dec/src/pred_inter.cpp \
 	$(VIDEO_PATH)avc_h264/dec/src/pred_intra.cpp \
 	$(VIDEO_PATH)avc_h264/dec/src/pvavcdecoder.cpp \
 	$(VIDEO_PATH)avc_h264/dec/src/pvavcdecoder_factory.cpp \
 	$(VIDEO_PATH)avc_h264/dec/src/residual.cpp \
 	$(VIDEO_PATH)avc_h264/dec/src/slice.cpp \
 	$(VIDEO_PATH)avc_h264/dec/src/vlc.cpp \
 	$(VIDEO_PATH)m4v_h263/port/avc_dec.cpp 
 	
LOCAL_MODULE := h264decoder

LOCAL_CFLAGS := -DPV_CPU_ARCH_VERSION=0 

LOCAL_C_INCLUDES := \
	$(VIDEO_PATH)avc_h264/dec/include/ \
	$(VIDEO_PATH)avc_h264/dec/src/ \
	$(VIDEO_PATH)m4v_h263/port/ \
	$(VIDEO_PATH)avc_h264/common/include/

include $(BUILD_STATIC_LIBRARY)

#=====================================================================================================================================

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	$(VIDEO_PATH)avc_h264/hwdec/src/SsbSipMfcDecAPI.c \
	$(VIDEO_PATH)m4v_h263/port/avc_hwdec.cpp 
 	
LOCAL_MODULE := hwh264decoder

LOCAL_CFLAGS := -DPV_CPU_ARCH_VERSION=0

LOCAL_C_INCLUDES := \
	$(VIDEO_PATH)avc_h264/hwdec/include/ \
	$(VIDEO_PATH)avc_h264/hwdec/src/ \
	$(VIDEO_PATH)avc_h264/common/include/ \
	$(VIDEO_PATH)m4v_h263/port/

include $(BUILD_STATIC_LIBRARY)

#=====================================================================================================================================

include $(CLEAR_VARS)

CODEC_H264COMMON_PATH := ./codec/video/avc_h264/common

LOCAL_SRC_FILES := \
	$(VIDEO_PATH)avc_h264/common/src/deblock.cpp \
 	$(VIDEO_PATH)avc_h264/common/src/dpb.cpp \
 	$(VIDEO_PATH)avc_h264/common/src/fmo.cpp \
 	$(VIDEO_PATH)avc_h264/common/src/mb_access.cpp \
 	$(VIDEO_PATH)avc_h264/common/src/reflist.cpp

LOCAL_MODULE := avc_common_lib

LOCAL_CFLAGS := -DPV_CPU_ARCH_VERSION=0

LOCAL_C_INCLUDES :=  \
	$(VIDEO_PATH)avc_h264/common/include/ \
	$(VIDEO_PATH)avc_h264/common/src/ \
	$(VIDEO_PATH)m4v_h263/port/

include $(BUILD_STATIC_LIBRARY)

#=====================================================================================================================================

include $(CLEAR_VARS)

CODEC_UTILM4V_PATH := ./codec/utilities/m4v_config_parser

LOCAL_SRC_FILES := \
	$(UTILS_PATH)m4v_config_parser/src/m4v_config_parser.cpp

LOCAL_MODULE := m4v_config

LOCAL_C_INCLUDES := \
	$(UTILS_PATH)m4v_config_parser/include/ \
	$(UTILS_PATH)m4v_config_parser/src/ \
	$(VIDEO_PATH)m4v_h263/port/

include $(BUILD_STATIC_LIBRARY)

#=====================================================================================================================================

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	$(VIDEO_PATH)m4v_h263/dec/src/adaptive_smooth_no_mmx.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/bitstream.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/block_idct.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/cal_dc_scaler.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/chvr_filter.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/chv_filter.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/combined_decode.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/conceal.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/datapart_decode.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/dcac_prediction.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/dec_pred_intra_dc.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/deringing_chroma.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/deringing_luma.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/find_min_max.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/get_pred_adv_b_add.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/get_pred_outside.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/idct.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/idct_vca.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/mb_motion_comp.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/mb_utils.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/packet_util.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/post_filter.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/post_proc_semaphore.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/pp_semaphore_chroma_inter.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/pp_semaphore_luma.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/pvdec_api.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/scaling_tab.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/vlc_decode.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/vlc_dequant.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/vlc_tab.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/vop.cpp \
	$(VIDEO_PATH)m4v_h263/dec/src/zigzag_tab.cpp \
	$(VIDEO_PATH)m4v_h263/port/mpeg4_dec.cpp \
	$(VIDEO_PATH)m4v_h263/port/m4v_io.cpp

LOCAL_MODULE := pvmp4decoder

LOCAL_C_INCLUDES := \
	$(VIDEO_PATH)m4v_h263/dec/include/ \
	$(VIDEO_PATH)m4v_h263/dec/src/ \
	$(VIDEO_PATH)m4v_h263/port

include $(BUILD_STATIC_LIBRARY)

#=====================================================================================================================================

include $(CLEAR_VARS)

LOCAL_MODULE    := test

LOCAL_STATIC_LIBRARIES := pvmp4decoder m4v_config colorconvert h264decoder hwh264decoder avc_common_lib aac_dec

include $(BUILD_SHARED_LIBRARY)

#=====================================================================================================================================
