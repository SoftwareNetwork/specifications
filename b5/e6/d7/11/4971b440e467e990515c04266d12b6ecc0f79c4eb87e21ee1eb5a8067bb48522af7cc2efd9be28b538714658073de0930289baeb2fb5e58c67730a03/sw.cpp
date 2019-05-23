void build(Solution &s)
{
    auto &vpx = s.addStaticLibrary("webmproject.vpx", "1.8.0");
    vpx += Git("https://github.com/webmproject/libvpx", "v{v}");

    vpx.setExtensionProgram(".asm", "org.sw.demo.yasm-master"_dep);

    //vpx.ExportAllSymbols = true;
    vpx.setChecks("vpx");

    vpx +=
        "third_party/x86inc/x86inc.asm",
        "vp8/.*"_rr,
        "vp9/.*"_rr,
        "vpx/.*"_rr,
        "vpx_.*"_rr;

    vpx -=
        ".*/arm/.*"_rr,
        ".*/mips/.*"_rr,
        ".*/ppc/.*"_rr,
        "error_concealment.c",
        "highbd_.*"_rr,
        "vp9_denoiser.c",
        "vp9_mfqe.c",
        "vpx_ports/.*"_rr;

    vpx +=
        "vpx_ports/.*\\.asm"_rr;

    //vpx -= ".*_avx2.*"_rr;
    vpx -= "vpx_dsp/x86/highbd_.*"_rr;

    vpx.Public += "WIN32"_def;

    vpx.Variables["ARCH_X86"] = 0;
    vpx.Variables["ARCH_X86_64"] = 0;
    if (vpx.getSettings().TargetOS.Arch != ArchType::x86_64)
    {
        vpx.Variables["ARCH_X86"] = 1;
        vpx.Variables["asm"] = "win32";
    }
    else
    {
        vpx.Variables["ARCH_X86_64"] = 1;
        vpx.Variables["asm"] = "win64";
    }

    vpx.Variables["CONFIG_GCC"] = 0;
    vpx.Variables["CONFIG_MSVS"] = 0;
    vpx.Variables["CONFIG_BIG_ENDIAN"] = vpx.Variables["WORDS_BIGENDIAN"];

    if (vpx.getCompilerType() == CompilerType::GNU)
        vpx.Variables["CONFIG_GCC"] = 1;

    if (vpx.getCompilerType() == CompilerType::MSVC)
        vpx.Variables["CONFIG_MSVS"] = 1;

    vpx.writeFileOnce("vpx_config.h.in", R"(
        #pragma once

        #define INLINE inline
        #define RESTRICT

        #cmakedefine01 ARCH_X86
        #cmakedefine01 ARCH_X86_64

        #define ARCH_ARM 0
        #define ARCH_MIPS 0
        #define ARCH_PPC 0
        #define HAVE_NEON 0
        #define HAVE_NEON_ASM 0
        #define HAVE_MIPS32 0
        #define HAVE_DSPR2 0
        #define HAVE_MSA 0
        #define HAVE_MIPS64 0
        #define HAVE_MMX 1
        #define HAVE_SSE 1
        #define HAVE_SSE2 1
        #define HAVE_SSE3 1
        #define HAVE_SSSE3 1
        #define HAVE_SSE4_1 1
        #define HAVE_AVX 1
        #define HAVE_AVX2 1
        #define HAVE_AVX512 0
        #define HAVE_VSX 0
        #define HAVE_MMI 0
        #define HAVE_VPX_PORTS 1

        #cmakedefine01 CONFIG_GCC
        #cmakedefine01 CONFIG_MSVS
        #cmakedefine01 CONFIG_BIG_ENDIAN

        #define CONFIG_DEPENDENCY_TRACKING 1
        #define CONFIG_EXTERNAL_BUILD 0
        #define CONFIG_INSTALL_DOCS 0
        #define CONFIG_INSTALL_BINS 1
        #define CONFIG_INSTALL_LIBS 1
        #define CONFIG_INSTALL_SRCS 0
        #define CONFIG_DEBUG 0
        #define CONFIG_GPROF 0
        #define CONFIG_GCOV 0
        #define CONFIG_RVCT 0
        #define CONFIG_PIC 0
        #define CONFIG_CODEC_SRCS 0
        #define CONFIG_DEBUG_LIBS 0
        #define CONFIG_DEQUANT_TOKENS 0
        #define CONFIG_DC_RECON 0
        #define CONFIG_RUNTIME_CPU_DETECT 1
        #define CONFIG_POSTPROC 1
        #define CONFIG_VP9_POSTPROC 1
        #define CONFIG_MULTITHREAD 1
        #define CONFIG_INTERNAL_STATS 1
        #define CONFIG_VP8_ENCODER 1
        #define CONFIG_VP8_DECODER 1
        #define CONFIG_VP9_ENCODER 1
        #define CONFIG_VP9_DECODER 1
        #define CONFIG_VP8 1
        #define CONFIG_VP9 1
        #define CONFIG_ENCODERS 1
        #define CONFIG_DECODERS 1
        #define CONFIG_STATIC_MSVCRT 0
        #define CONFIG_SPATIAL_RESAMPLING 1
        #define CONFIG_REALTIME_ONLY 0
        #define CONFIG_ONTHEFLY_BITPACKING 0
        #define CONFIG_ERROR_CONCEALMENT 1
        #define CONFIG_SMALL 0
        #define CONFIG_POSTPROC_VISUALIZER 1
        #define CONFIG_OS_SUPPORT 1
        #define CONFIG_UNIT_TESTS 1
        #define CONFIG_WEBM_IO 1
        #define CONFIG_LIBYUV 1
        #define CONFIG_DECODE_PERF_TESTS 0
        #define CONFIG_ENCODE_PERF_TESTS 0
        #define CONFIG_MULTI_RES_ENCODING 1
        #define CONFIG_TEMPORAL_DENOISING 1
        #define CONFIG_VP9_TEMPORAL_DENOISING 1
        #define CONFIG_CONSISTENT_RECODE 0
        #define CONFIG_COEFFICIENT_RANGE_CHECKING 0
        #define CONFIG_VP9_HIGHBITDEPTH 0
        #define CONFIG_BETTER_HW_COMPATIBILITY 0
        #define CONFIG_EXPERIMENTAL 0
        #define CONFIG_SIZE_LIMIT 0
        #define CONFIG_ALWAYS_ADJUST_BPM 0
        #define CONFIG_FP_MB_STATS 0
        #define CONFIG_EMULATE_HARDWARE 0
    )");
    vpx.configureFile("vpx_config.h.in", "vpx_config.h");

    vpx.writeFileOnce("vpx_config.asm.in", R"(
        ARCH_ARM equ 0
        ARCH_MIPS equ 0
        ARCH_X86 equ ${ARCH_X86}
        ARCH_X86_64 equ ${ARCH_X86_64}
        ARCH_PPC equ 0
        HAVE_NEON equ 0
        HAVE_NEON_ASM equ 0
        HAVE_MIPS32 equ 0
        HAVE_DSPR2 equ 0
        HAVE_MSA equ 0
        HAVE_MIPS64 equ 0
        HAVE_MMX equ 1
        HAVE_SSE equ 1
        HAVE_SSE2 equ 1
        HAVE_SSE3 equ 1
        HAVE_SSSE3 equ 1
        HAVE_SSE4_1 equ 1
        HAVE_AVX equ 1
        HAVE_AVX2 equ 1
        HAVE_AVX512 equ 0
        HAVE_VSX equ 0
        HAVE_MMI equ 0
        HAVE_VPX_PORTS equ 1
        HAVE_PTHREAD_H equ ${HAVE_PTHREAD_H}
        HAVE_UNISTD_H equ ${HAVE_UNISTD_H}
        CONFIG_DEPENDENCY_TRACKING equ 1
        CONFIG_EXTERNAL_BUILD equ 0
        CONFIG_INSTALL_DOCS equ 0
        CONFIG_INSTALL_BINS equ 1
        CONFIG_INSTALL_LIBS equ 1
        CONFIG_INSTALL_SRCS equ 0
        CONFIG_DEBUG equ 0
        CONFIG_GPROF equ 0
        CONFIG_GCOV equ 0
        CONFIG_RVCT equ 0
        CONFIG_GCC equ ${CONFIG_GCC}
        CONFIG_MSVS equ ${CONFIG_MSVS}
        CONFIG_PIC equ 0
        CONFIG_BIG_ENDIAN equ ${CONFIG_BIG_ENDIAN}
        CONFIG_CODEC_SRCS equ 0
        CONFIG_DEBUG_LIBS equ 0
        CONFIG_DEQUANT_TOKENS equ 0
        CONFIG_DC_RECON equ 0
        CONFIG_RUNTIME_CPU_DETECT equ 1
        CONFIG_POSTPROC equ 1
        CONFIG_VP9_POSTPROC equ 1
        CONFIG_MULTITHREAD equ 1
        CONFIG_INTERNAL_STATS equ 1
        CONFIG_VP8_ENCODER equ 1
        CONFIG_VP8_DECODER equ 1
        CONFIG_VP9_ENCODER equ 1
        CONFIG_VP9_DECODER equ 1
        CONFIG_VP8 equ 1
        CONFIG_VP9 equ 1
        CONFIG_ENCODERS equ 1
        CONFIG_DECODERS equ 1
        CONFIG_STATIC_MSVCRT equ 0
        CONFIG_SPATIAL_RESAMPLING equ 1
        CONFIG_REALTIME_ONLY equ 0
        CONFIG_ONTHEFLY_BITPACKING equ 0
        CONFIG_ERROR_CONCEALMENT equ 1
        CONFIG_SMALL equ 0
        CONFIG_POSTPROC_VISUALIZER equ 1
        CONFIG_OS_SUPPORT equ 1
        CONFIG_UNIT_TESTS equ 1
        CONFIG_WEBM_IO equ 1
        CONFIG_LIBYUV equ 1
        CONFIG_DECODE_PERF_TESTS equ 0
        CONFIG_ENCODE_PERF_TESTS equ 0
        CONFIG_MULTI_RES_ENCODING equ 1
        CONFIG_TEMPORAL_DENOISING equ 1
        CONFIG_VP9_TEMPORAL_DENOISING equ 1
        CONFIG_CONSISTENT_RECODE equ 0
        CONFIG_COEFFICIENT_RANGE_CHECKING equ 0
        CONFIG_VP9_HIGHBITDEPTH equ 0
        CONFIG_BETTER_HW_COMPATIBILITY equ 0
        CONFIG_EXPERIMENTAL equ 0
        CONFIG_SIZE_LIMIT equ 0
        CONFIG_ALWAYS_ADJUST_BPM equ 0
        CONFIG_FP_MB_STATS equ 0
        CONFIG_EMULATE_HARDWARE equ 0
    )");
    vpx.configureFile("vpx_config.asm.in", "vpx_config.asm");
    vpx -= "vpx_config.asm";

    if (vpx.getSettings().TargetOS.Type == OSType::Windows)
    {
        if (vpx.getSettings().TargetOS.Arch != ArchType::x86_64)
            vpx -= ".*_x86_64.asm"_rr;
    }

    vpx.writeFileOnce("vpx_version.h",
        "#define VERSION_MAJOR " + vpx.Variables["PACKAGE_VERSION_MAJOR"].toString() + "\n" +
        "#define VERSION_MINOR " + vpx.Variables["PACKAGE_VERSION_MINOR"].toString() + "\n" +
        "#define VERSION_PATCH " + vpx.Variables["PACKAGE_VERSION_PATCH"].toString() + "\n" +
        R"(
        // This file is generated. Do not edit.
        #define VERSION_EXTRA  ""
        #define VERSION_PACKED ((VERSION_MAJOR<<16)|(VERSION_MINOR<<8)|(VERSION_PATCH))
        #define VERSION_STRING_NOSP ""
        #define VERSION_STRING      ""
    )");

    {
        std::ostringstream oss;
        oss <<
            R"(
        // This file is generated. Do not edit.
        #ifndef VP8_RTCD_H_
        #define VP8_RTCD_H_

        #ifdef RTCD_C
        #define RTCD_EXTERN
        #else
        #define RTCD_EXTERN extern
        #endif

        /*
         * VP8
         */

        struct blockd;
        struct macroblockd;
        struct loop_filter_info;

        /* Encoder forward decls */
        struct block;
        struct macroblock;
        struct variance_vtable;
        union int_mv;
        struct yv12_buffer_config;

        #ifdef __cplusplus
        extern "C" {
        #endif

        void vp8_bilinear_predict16x16_c(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
        void vp8_bilinear_predict16x16_sse2(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
        void vp8_bilinear_predict16x16_ssse3(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
        RTCD_EXTERN void (*vp8_bilinear_predict16x16)(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);

        void vp8_bilinear_predict4x4_c(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
        void vp8_bilinear_predict4x4_sse2(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
        #define vp8_bilinear_predict4x4 vp8_bilinear_predict4x4_sse2

        void vp8_bilinear_predict8x4_c(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
        void vp8_bilinear_predict8x4_sse2(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
        #define vp8_bilinear_predict8x4 vp8_bilinear_predict4x4_sse2

        void vp8_bilinear_predict8x8_c(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
        void vp8_bilinear_predict8x8_sse2(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
        void vp8_bilinear_predict8x8_ssse3(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
        RTCD_EXTERN void (*vp8_bilinear_predict8x8)(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);

        void vp8_blend_b_c(unsigned char *y, unsigned char *u, unsigned char *v, int y1, int u1, int v1, int alpha, int stride);
        #define vp8_blend_b vp8_blend_b_c

        void vp8_blend_mb_inner_c(unsigned char *y, unsigned char *u, unsigned char *v, int y1, int u1, int v1, int alpha, int stride);
        #define vp8_blend_mb_inner vp8_blend_mb_inner_c

        void vp8_blend_mb_outer_c(unsigned char *y, unsigned char *u, unsigned char *v, int y1, int u1, int v1, int alpha, int stride);
        #define vp8_blend_mb_outer vp8_blend_mb_outer_c

        int vp8_block_error_c(short *coeff, short *dqcoeff);
        int vp8_block_error_sse2(short *coeff, short *dqcoeff);
        #define vp8_block_error vp8_block_error_sse2

        void vp8_copy32xn_c(const unsigned char *src_ptr, int source_stride, unsigned char *dst_ptr, int dst_stride, int n);
        void vp8_copy32xn_sse2(const unsigned char *src_ptr, int source_stride, unsigned char *dst_ptr, int dst_stride, int n);
        void vp8_copy32xn_sse3(const unsigned char *src_ptr, int source_stride, unsigned char *dst_ptr, int dst_stride, int n);
        RTCD_EXTERN void (*vp8_copy32xn)(const unsigned char *src_ptr, int source_stride, unsigned char *dst_ptr, int dst_stride, int n);

        void vp8_copy_mem16x16_c(unsigned char *src, int src_pitch, unsigned char *dst, int dst_pitch);
        void vp8_copy_mem16x16_sse2(unsigned char *src, int src_pitch, unsigned char *dst, int dst_pitch);
        #define vp8_copy_mem16x16 vp8_copy_mem16x16_sse2

        void vp8_copy_mem8x4_c(unsigned char *src, int src_pitch, unsigned char *dst, int dst_pitch);
        void vp8_copy_mem8x4_mmx(unsigned char *src, int src_pitch, unsigned char *dst, int dst_pitch);
        #define vp8_copy_mem8x4 vp8_copy_mem8x4_mmx

        void vp8_copy_mem8x8_c(unsigned char *src, int src_pitch, unsigned char *dst, int dst_pitch);
        void vp8_copy_mem8x8_mmx(unsigned char *src, int src_pitch, unsigned char *dst, int dst_pitch);
        #define vp8_copy_mem8x8 vp8_copy_mem8x8_mmx

        void vp8_dc_only_idct_add_c(short input, unsigned char *pred, int pred_stride, unsigned char *dst, int dst_stride);
        void vp8_dc_only_idct_add_mmx(short input, unsigned char *pred, int pred_stride, unsigned char *dst, int dst_stride);
        #define vp8_dc_only_idct_add vp8_dc_only_idct_add_mmx

        int vp8_denoiser_filter_c(unsigned char *mc_running_avg_y, int mc_avg_y_stride, unsigned char *running_avg_y, int avg_y_stride, unsigned char *sig, int sig_stride, unsigned int motion_magnitude, int increase_denoising);
        int vp8_denoiser_filter_sse2(unsigned char *mc_running_avg_y, int mc_avg_y_stride, unsigned char *running_avg_y, int avg_y_stride, unsigned char *sig, int sig_stride, unsigned int motion_magnitude, int increase_denoising);
        #define vp8_denoiser_filter vp8_denoiser_filter_sse2

        int vp8_denoiser_filter_uv_c(unsigned char *mc_running_avg, int mc_avg_stride, unsigned char *running_avg, int avg_stride, unsigned char *sig, int sig_stride, unsigned int motion_magnitude, int increase_denoising);
        int vp8_denoiser_filter_uv_sse2(unsigned char *mc_running_avg, int mc_avg_stride, unsigned char *running_avg, int avg_stride, unsigned char *sig, int sig_stride, unsigned int motion_magnitude, int increase_denoising);
        #define vp8_denoiser_filter_uv vp8_denoiser_filter_uv_sse2

        void vp8_dequant_idct_add_c(short *input, short *dq, unsigned char *output, int stride);
        void vp8_dequant_idct_add_mmx(short *input, short *dq, unsigned char *output, int stride);
        #define vp8_dequant_idct_add vp8_dequant_idct_add_mmx

        void vp8_dequant_idct_add_uv_block_c(short *q, short *dq, unsigned char *dst_u, unsigned char *dst_v, int stride, char *eobs);
        void vp8_dequant_idct_add_uv_block_sse2(short *q, short *dq, unsigned char *dst_u, unsigned char *dst_v, int stride, char *eobs);
        #define vp8_dequant_idct_add_uv_block vp8_dequant_idct_add_uv_block_sse2

        void vp8_dequant_idct_add_y_block_c(short *q, short *dq, unsigned char *dst, int stride, char *eobs);
        void vp8_dequant_idct_add_y_block_sse2(short *q, short *dq, unsigned char *dst, int stride, char *eobs);
        #define vp8_dequant_idct_add_y_block vp8_dequant_idct_add_y_block_sse2

        void vp8_dequantize_b_c(struct blockd*, short *dqc);
        void vp8_dequantize_b_mmx(struct blockd*, short *dqc);
        #define vp8_dequantize_b vp8_dequantize_b_mmx

        int vp8_diamond_search_sad_c(struct macroblock *x, struct block *b, struct blockd *d, union int_mv *ref_mv, union int_mv *best_mv, int search_param, int sad_per_bit, int *num00, struct variance_vtable *fn_ptr, int *mvcost[2], union int_mv *center_mv);
        int vp8_diamond_search_sadx4(struct macroblock *x, struct block *b, struct blockd *d, union int_mv *ref_mv, union int_mv *best_mv, int search_param, int sad_per_bit, int *num00, struct variance_vtable *fn_ptr, int *mvcost[2], union int_mv *center_mv);
        #define vp8_diamond_search_sad vp8_diamond_search_sadx4

        void vp8_fast_quantize_b_c(struct block *, struct blockd *);
        void vp8_fast_quantize_b_sse2(struct block *, struct blockd *);
        void vp8_fast_quantize_b_ssse3(struct block *, struct blockd *);
        RTCD_EXTERN void (*vp8_fast_quantize_b)(struct block *, struct blockd *);

        void vp8_filter_by_weight16x16_c(unsigned char *src, int src_stride, unsigned char *dst, int dst_stride, int src_weight);
        void vp8_filter_by_weight16x16_sse2(unsigned char *src, int src_stride, unsigned char *dst, int dst_stride, int src_weight);
        #define vp8_filter_by_weight16x16 vp8_filter_by_weight16x16_sse2

        void vp8_filter_by_weight4x4_c(unsigned char *src, int src_stride, unsigned char *dst, int dst_stride, int src_weight);
        #define vp8_filter_by_weight4x4 vp8_filter_by_weight4x4_c

        void vp8_filter_by_weight8x8_c(unsigned char *src, int src_stride, unsigned char *dst, int dst_stride, int src_weight);
        void vp8_filter_by_weight8x8_sse2(unsigned char *src, int src_stride, unsigned char *dst, int dst_stride, int src_weight);
        #define vp8_filter_by_weight8x8 vp8_filter_by_weight8x8_sse2

        int vp8_full_search_sad_c(struct macroblock *x, struct block *b, struct blockd *d, union int_mv *ref_mv, int sad_per_bit, int distance, struct variance_vtable *fn_ptr, int *mvcost[2], union int_mv *center_mv);
        int vp8_full_search_sadx3(struct macroblock *x, struct block *b, struct blockd *d, union int_mv *ref_mv, int sad_per_bit, int distance, struct variance_vtable *fn_ptr, int *mvcost[2], union int_mv *center_mv);
        int vp8_full_search_sadx8(struct macroblock *x, struct block *b, struct blockd *d, union int_mv *ref_mv, int sad_per_bit, int distance, struct variance_vtable *fn_ptr, int *mvcost[2], union int_mv *center_mv);
        RTCD_EXTERN int (*vp8_full_search_sad)(struct macroblock *x, struct block *b, struct blockd *d, union int_mv *ref_mv, int sad_per_bit, int distance, struct variance_vtable *fn_ptr, int *mvcost[2], union int_mv *center_mv);

        void vp8_loop_filter_bh_c(unsigned char *y, unsigned char *u, unsigned char *v, int ystride, int uv_stride, struct loop_filter_info *lfi);
        void vp8_loop_filter_bh_sse2(unsigned char *y, unsigned char *u, unsigned char *v, int ystride, int uv_stride, struct loop_filter_info *lfi);
        #define vp8_loop_filter_bh vp8_loop_filter_bh_sse2

        void vp8_loop_filter_bv_c(unsigned char *y, unsigned char *u, unsigned char *v, int ystride, int uv_stride, struct loop_filter_info *lfi);
        void vp8_loop_filter_bv_sse2(unsigned char *y, unsigned char *u, unsigned char *v, int ystride, int uv_stride, struct loop_filter_info *lfi);
        #define vp8_loop_filter_bv vp8_loop_filter_bv_sse2

        void vp8_loop_filter_mbh_c(unsigned char *y, unsigned char *u, unsigned char *v, int ystride, int uv_stride, struct loop_filter_info *lfi);
        void vp8_loop_filter_mbh_sse2(unsigned char *y, unsigned char *u, unsigned char *v, int ystride, int uv_stride, struct loop_filter_info *lfi);
        #define vp8_loop_filter_mbh vp8_loop_filter_mbh_sse2

        void vp8_loop_filter_mbv_c(unsigned char *y, unsigned char *u, unsigned char *v, int ystride, int uv_stride, struct loop_filter_info *lfi);
        void vp8_loop_filter_mbv_sse2(unsigned char *y, unsigned char *u, unsigned char *v, int ystride, int uv_stride, struct loop_filter_info *lfi);
        #define vp8_loop_filter_mbv vp8_loop_filter_mbv_sse2

        void vp8_loop_filter_bhs_c(unsigned char *y, int ystride, const unsigned char *blimit);
        void vp8_loop_filter_bhs_sse2(unsigned char *y, int ystride, const unsigned char *blimit);
        #define vp8_loop_filter_simple_bh vp8_loop_filter_bhs_sse2

        void vp8_loop_filter_bvs_c(unsigned char *y, int ystride, const unsigned char *blimit);
        void vp8_loop_filter_bvs_sse2(unsigned char *y, int ystride, const unsigned char *blimit);
        #define vp8_loop_filter_simple_bv vp8_loop_filter_bvs_sse2

        void vp8_loop_filter_simple_horizontal_edge_c(unsigned char *y, int ystride, const unsigned char *blimit);
        void vp8_loop_filter_simple_horizontal_edge_sse2(unsigned char *y, int ystride, const unsigned char *blimit);
        #define vp8_loop_filter_simple_mbh vp8_loop_filter_simple_horizontal_edge_sse2

        void vp8_loop_filter_simple_vertical_edge_c(unsigned char *y, int ystride, const unsigned char *blimit);
        void vp8_loop_filter_simple_vertical_edge_sse2(unsigned char *y, int ystride, const unsigned char *blimit);
        #define vp8_loop_filter_simple_mbv vp8_loop_filter_simple_vertical_edge_sse2

        int vp8_mbblock_error_c(struct macroblock *mb, int dc);
        int vp8_mbblock_error_sse2(struct macroblock *mb, int dc);
        #define vp8_mbblock_error vp8_mbblock_error_sse2

        int vp8_mbuverror_c(struct macroblock *mb);
        int vp8_mbuverror_sse2(struct macroblock *mb);
        #define vp8_mbuverror vp8_mbuverror_sse2

        int vp8_refining_search_sad_c(struct macroblock *x, struct block *b, struct blockd *d, union int_mv *ref_mv, int sad_per_bit, int distance, struct variance_vtable *fn_ptr, int *mvcost[2], union int_mv *center_mv);
        int vp8_refining_search_sadx4(struct macroblock *x, struct block *b, struct blockd *d, union int_mv *ref_mv, int sad_per_bit, int distance, struct variance_vtable *fn_ptr, int *mvcost[2], union int_mv *center_mv);
        #define vp8_refining_search_sad vp8_refining_search_sadx4

        void vp8_regular_quantize_b_c(struct block *, struct blockd *);
        void vp8_regular_quantize_b_sse2(struct block *, struct blockd *);
        void vp8_regular_quantize_b_sse4_1(struct block *, struct blockd *);
        RTCD_EXTERN void (*vp8_regular_quantize_b)(struct block *, struct blockd *);

        void vp8_short_fdct4x4_c(short *input, short *output, int pitch);
        void vp8_short_fdct4x4_sse2(short *input, short *output, int pitch);
        #define vp8_short_fdct4x4 vp8_short_fdct4x4_sse2

        void vp8_short_fdct8x4_c(short *input, short *output, int pitch);
        void vp8_short_fdct8x4_sse2(short *input, short *output, int pitch);
        #define vp8_short_fdct8x4 vp8_short_fdct8x4_sse2

        void vp8_short_idct4x4llm_c(short *input, unsigned char *pred, int pitch, unsigned char *dst, int dst_stride);
        void vp8_short_idct4x4llm_mmx(short *input, unsigned char *pred, int pitch, unsigned char *dst, int dst_stride);
        #define vp8_short_idct4x4llm vp8_short_idct4x4llm_mmx

        void vp8_short_inv_walsh4x4_c(short *input, short *output);
        void vp8_short_inv_walsh4x4_sse2(short *input, short *output);
        #define vp8_short_inv_walsh4x4 vp8_short_inv_walsh4x4_sse2

)"
<<
R"(

        void vp8_short_inv_walsh4x4_1_c(short *input, short *output);
        #define vp8_short_inv_walsh4x4_1 vp8_short_inv_walsh4x4_1_c

        void vp8_short_walsh4x4_c(short *input, short *output, int pitch);
        void vp8_short_walsh4x4_sse2(short *input, short *output, int pitch);
        #define vp8_short_walsh4x4 vp8_short_walsh4x4_sse2

        void vp8_sixtap_predict16x16_c(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
        void vp8_sixtap_predict16x16_sse2(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
        void vp8_sixtap_predict16x16_ssse3(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
        RTCD_EXTERN void (*vp8_sixtap_predict16x16)(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);

        void vp8_sixtap_predict4x4_c(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
        void vp8_sixtap_predict4x4_mmx(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
        void vp8_sixtap_predict4x4_ssse3(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
        RTCD_EXTERN void (*vp8_sixtap_predict4x4)(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);

        void vp8_sixtap_predict8x4_c(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
        void vp8_sixtap_predict8x4_sse2(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
        void vp8_sixtap_predict8x4_ssse3(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
        RTCD_EXTERN void (*vp8_sixtap_predict8x4)(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);

        void vp8_sixtap_predict8x8_c(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
        void vp8_sixtap_predict8x8_sse2(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
        void vp8_sixtap_predict8x8_ssse3(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
        RTCD_EXTERN void (*vp8_sixtap_predict8x8)(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);

        void vp8_temporal_filter_apply_c(unsigned char *frame1, unsigned int stride, unsigned char *frame2, unsigned int block_size, int strength, int filter_weight, unsigned int *accumulator, unsigned short *count);
        void vp8_temporal_filter_apply_sse2(unsigned char *frame1, unsigned int stride, unsigned char *frame2, unsigned int block_size, int strength, int filter_weight, unsigned int *accumulator, unsigned short *count);
        #define vp8_temporal_filter_apply vp8_temporal_filter_apply_sse2

        void vp8_rtcd(void);

        #ifdef RTCD_C
        #include "vpx_ports/x86.h"
        static void setup_rtcd_internal(void)
        {
            int flags = x86_simd_caps();

            (void)flags;

            vp8_bilinear_predict16x16 = vp8_bilinear_predict16x16_sse2;
            if (flags & HAS_SSSE3) vp8_bilinear_predict16x16 = vp8_bilinear_predict16x16_ssse3;
            vp8_bilinear_predict8x8 = vp8_bilinear_predict8x8_sse2;
            if (flags & HAS_SSSE3) vp8_bilinear_predict8x8 = vp8_bilinear_predict8x8_ssse3;
            vp8_copy32xn = vp8_copy32xn_sse2;
            if (flags & HAS_SSE3) vp8_copy32xn = vp8_copy32xn_sse3;
            vp8_fast_quantize_b = vp8_fast_quantize_b_sse2;
            if (flags & HAS_SSSE3) vp8_fast_quantize_b = vp8_fast_quantize_b_ssse3;
            vp8_full_search_sad = vp8_full_search_sad_c;
            if (flags & HAS_SSE3) vp8_full_search_sad = vp8_full_search_sadx3;
            if (flags & HAS_SSE4_1) vp8_full_search_sad = vp8_full_search_sadx8;
            vp8_regular_quantize_b = vp8_regular_quantize_b_sse2;
            if (flags & HAS_SSE4_1) vp8_regular_quantize_b = vp8_regular_quantize_b_sse4_1;
            vp8_sixtap_predict16x16 = vp8_sixtap_predict16x16_sse2;
            if (flags & HAS_SSSE3) vp8_sixtap_predict16x16 = vp8_sixtap_predict16x16_ssse3;
            vp8_sixtap_predict4x4 = vp8_sixtap_predict4x4_mmx;
            if (flags & HAS_SSSE3) vp8_sixtap_predict4x4 = vp8_sixtap_predict4x4_ssse3;
            vp8_sixtap_predict8x4 = vp8_sixtap_predict8x4_sse2;
            if (flags & HAS_SSSE3) vp8_sixtap_predict8x4 = vp8_sixtap_predict8x4_ssse3;
            vp8_sixtap_predict8x8 = vp8_sixtap_predict8x8_sse2;
            if (flags & HAS_SSSE3) vp8_sixtap_predict8x8 = vp8_sixtap_predict8x8_ssse3;
        }
        #endif

        #ifdef __cplusplus
        }  // extern "C"
        #endif

        #endif
    )";

        vpx.writeFileOnce("vp8_rtcd.h", oss.str());
    }

    vpx.writeFileOnce("vp9_rtcd.h", R"(
        // This file is generated. Do not edit.
        #ifndef VP9_RTCD_H_
        #define VP9_RTCD_H_

        #ifdef RTCD_C
        #define RTCD_EXTERN
        #else
        #define RTCD_EXTERN extern
        #endif

        /*
         * VP9
         */

        #include "vpx/vpx_integer.h"
        #include "vp9/common/vp9_common.h"
        #include "vp9/common/vp9_enums.h"
        #include "vp9/common/vp9_filter.h"

        struct macroblockd;

        /* Encoder forward decls */
        struct macroblock;
        struct vp9_variance_vtable;
        struct search_site_config;
        struct mv;
        union int_mv;
        struct yv12_buffer_config;

        #ifdef __cplusplus
        extern "C" {
        #endif

        int vp9_denoiser_filter_c(unsigned char *mc_running_avg_y, int mc_avg_y_stride, unsigned char *running_avg_y, int avg_y_stride, unsigned char *sig, int sig_stride, unsigned int motion_magnitude, int increase_denoising);
        int vp9_denoiser_filter_sse2(unsigned char *mc_running_avg_y, int mc_avg_y_stride, unsigned char *running_avg_y, int avg_y_stride, unsigned char *sig, int sig_stride, unsigned int motion_magnitude, int increase_denoising);
        #define vp9_denoiser_filter vp9_denoiser_filter_sse2

        void vp9_filter_by_weight16x16_c(unsigned char *src, int src_stride, unsigned char *dst, int dst_stride, int src_weight);
        void vp9_filter_by_weight16x16_sse2(unsigned char *src, int src_stride, unsigned char *dst, int dst_stride, int src_weight);
        #define vp9_filter_by_weight16x16 vp9_filter_by_weight16x16_sse2

        void vp9_filter_by_weight8x8_c(unsigned char *src, int src_stride, unsigned char *dst, int dst_stride, int src_weight);
        void vp9_filter_by_weight8x8_sse2(unsigned char *src, int src_stride, unsigned char *dst, int dst_stride, int src_weight);
        #define vp9_filter_by_weight8x8 vp9_filter_by_weight8x8_sse2

        int64_t vp9_block_error_c(const tran_low_t *coeff, const tran_low_t *dqcoeff, intptr_t block_size, int64_t *ssz);
        int64_t vp9_block_error_sse2(const tran_low_t *coeff, const tran_low_t *dqcoeff, intptr_t block_size, int64_t *ssz);
        int64_t vp9_block_error_avx2(const tran_low_t *coeff, const tran_low_t *dqcoeff, intptr_t block_size, int64_t *ssz);
        RTCD_EXTERN int64_t (*vp9_block_error)(const tran_low_t *coeff, const tran_low_t *dqcoeff, intptr_t block_size, int64_t *ssz);

        int64_t vp9_block_error_fp_c(const tran_low_t *coeff, const tran_low_t *dqcoeff, int block_size);
        int64_t vp9_block_error_fp_sse2(const tran_low_t *coeff, const tran_low_t *dqcoeff, int block_size);
        int64_t vp9_block_error_fp_avx2(const tran_low_t *coeff, const tran_low_t *dqcoeff, int block_size);
        RTCD_EXTERN int64_t (*vp9_block_error_fp)(const tran_low_t *coeff, const tran_low_t *dqcoeff, int block_size);

        int vp9_diamond_search_sad_c(const struct macroblock *x, const struct search_site_config *cfg,  struct mv *ref_mv, struct mv *best_mv, int search_param, int sad_per_bit, int *num00, const struct vp9_variance_vtable *fn_ptr, const struct mv *center_mv);
        int vp9_diamond_search_sad_avx(const struct macroblock *x, const struct search_site_config *cfg,  struct mv *ref_mv, struct mv *best_mv, int search_param, int sad_per_bit, int *num00, const struct vp9_variance_vtable *fn_ptr, const struct mv *center_mv);
        RTCD_EXTERN int (*vp9_diamond_search_sad)(const struct macroblock *x, const struct search_site_config *cfg,  struct mv *ref_mv, struct mv *best_mv, int search_param, int sad_per_bit, int *num00, const struct vp9_variance_vtable *fn_ptr, const struct mv *center_mv);

        void vp9_fdct8x8_quant_c(const int16_t *input, int stride, tran_low_t *coeff_ptr, intptr_t n_coeffs, int skip_block, const int16_t *round_ptr, const int16_t *quant_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
        void vp9_fdct8x8_quant_sse2(const int16_t *input, int stride, tran_low_t *coeff_ptr, intptr_t n_coeffs, int skip_block, const int16_t *round_ptr, const int16_t *quant_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
        void vp9_fdct8x8_quant_ssse3(const int16_t *input, int stride, tran_low_t *coeff_ptr, intptr_t n_coeffs, int skip_block, const int16_t *round_ptr, const int16_t *quant_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
        RTCD_EXTERN void (*vp9_fdct8x8_quant)(const int16_t *input, int stride, tran_low_t *coeff_ptr, intptr_t n_coeffs, int skip_block, const int16_t *round_ptr, const int16_t *quant_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);

        void vp9_fht16x16_c(const int16_t *input, tran_low_t *output, int stride, int tx_type);
        void vp9_fht16x16_sse2(const int16_t *input, tran_low_t *output, int stride, int tx_type);
        #define vp9_fht16x16 vp9_fht16x16_sse2

        void vp9_fht4x4_c(const int16_t *input, tran_low_t *output, int stride, int tx_type);
        void vp9_fht4x4_sse2(const int16_t *input, tran_low_t *output, int stride, int tx_type);
        #define vp9_fht4x4 vp9_fht4x4_sse2

        void vp9_fht8x8_c(const int16_t *input, tran_low_t *output, int stride, int tx_type);
        void vp9_fht8x8_sse2(const int16_t *input, tran_low_t *output, int stride, int tx_type);
        #define vp9_fht8x8 vp9_fht8x8_sse2

        void vp9_fwht4x4_c(const int16_t *input, tran_low_t *output, int stride);
        void vp9_fwht4x4_sse2(const int16_t *input, tran_low_t *output, int stride);
        #define vp9_fwht4x4 vp9_fwht4x4_sse2

        void vp9_iht16x16_256_add_c(const tran_low_t *input, uint8_t *output, int pitch, int tx_type);
        void vp9_iht16x16_256_add_sse2(const tran_low_t *input, uint8_t *output, int pitch, int tx_type);
        #define vp9_iht16x16_256_add vp9_iht16x16_256_add_sse2

        void vp9_iht4x4_16_add_c(const tran_low_t *input, uint8_t *dest, int stride, int tx_type);
        void vp9_iht4x4_16_add_sse2(const tran_low_t *input, uint8_t *dest, int stride, int tx_type);
        #define vp9_iht4x4_16_add vp9_iht4x4_16_add_sse2

        void vp9_iht8x8_64_add_c(const tran_low_t *input, uint8_t *dest, int stride, int tx_type);
        void vp9_iht8x8_64_add_sse2(const tran_low_t *input, uint8_t *dest, int stride, int tx_type);
        #define vp9_iht8x8_64_add vp9_iht8x8_64_add_sse2

        void vp9_quantize_fp_c(const tran_low_t *coeff_ptr, intptr_t n_coeffs, int skip_block, const int16_t *round_ptr, const int16_t *quant_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
        void vp9_quantize_fp_sse2(const tran_low_t *coeff_ptr, intptr_t n_coeffs, int skip_block, const int16_t *round_ptr, const int16_t *quant_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
        //void vp9_quantize_fp_ssse3(const tran_low_t *coeff_ptr, intptr_t n_coeffs, int skip_block, const int16_t *round_ptr, const int16_t *quant_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
        //void vp9_quantize_fp_avx2(const tran_low_t *coeff_ptr, intptr_t n_coeffs, int skip_block, const int16_t *round_ptr, const int16_t *quant_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
        RTCD_EXTERN void (*vp9_quantize_fp)(const tran_low_t *coeff_ptr, intptr_t n_coeffs, int skip_block, const int16_t *round_ptr, const int16_t *quant_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);

        void vp9_quantize_fp_32x32_c(const tran_low_t *coeff_ptr, intptr_t n_coeffs, int skip_block, const int16_t *round_ptr, const int16_t *quant_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
        //void vp9_quantize_fp_32x32_ssse3(const tran_low_t *coeff_ptr, intptr_t n_coeffs, int skip_block, const int16_t *round_ptr, const int16_t *quant_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
        RTCD_EXTERN void (*vp9_quantize_fp_32x32)(const tran_low_t *coeff_ptr, intptr_t n_coeffs, int skip_block, const int16_t *round_ptr, const int16_t *quant_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);

        void vp9_scale_and_extend_frame_c(const struct yv12_buffer_config *src, struct yv12_buffer_config *dst, INTERP_FILTER filter_type, int phase_scaler);
        void vp9_scale_and_extend_frame_ssse3(const struct yv12_buffer_config *src, struct yv12_buffer_config *dst, INTERP_FILTER filter_type, int phase_scaler);
        RTCD_EXTERN void (*vp9_scale_and_extend_frame)(const struct yv12_buffer_config *src, struct yv12_buffer_config *dst, INTERP_FILTER filter_type, int phase_scaler);

        void vp9_temporal_filter_apply_c(const uint8_t *frame1, unsigned int stride, const uint8_t *frame2, unsigned int block_width, unsigned int block_height, int strength, int filter_weight, uint32_t *accumulator, uint16_t *count);
        void vp9_temporal_filter_apply_sse4_1(const uint8_t *frame1, unsigned int stride, const uint8_t *frame2, unsigned int block_width, unsigned int block_height, int strength, int filter_weight, uint32_t *accumulator, uint16_t *count);
        RTCD_EXTERN void (*vp9_temporal_filter_apply)(const uint8_t *frame1, unsigned int stride, const uint8_t *frame2, unsigned int block_width, unsigned int block_height, int strength, int filter_weight, uint32_t *accumulator, uint16_t *count);

        void vp9_rtcd(void);

        #ifdef RTCD_C
        #include "vpx_ports/x86.h"
        static void setup_rtcd_internal(void)
        {
            int flags = x86_simd_caps();

            (void)flags;

            vp9_block_error = vp9_block_error_sse2;
            if (flags & HAS_AVX2) vp9_block_error = vp9_block_error_avx2;
            vp9_block_error_fp = vp9_block_error_fp_sse2;
            if (flags & HAS_AVX2) vp9_block_error_fp = vp9_block_error_fp_avx2;
            vp9_diamond_search_sad = vp9_diamond_search_sad_c;
            if (flags & HAS_AVX) vp9_diamond_search_sad = vp9_diamond_search_sad_avx;
            vp9_fdct8x8_quant = vp9_fdct8x8_quant_sse2;
            if (flags & HAS_SSSE3) vp9_fdct8x8_quant = vp9_fdct8x8_quant_ssse3;
            vp9_quantize_fp = vp9_quantize_fp_sse2;
            //if (flags & HAS_SSSE3) vp9_quantize_fp = vp9_quantize_fp_ssse3;
            //if (flags & HAS_AVX2) vp9_quantize_fp = vp9_quantize_fp_avx2;
            vp9_quantize_fp_32x32 = vp9_quantize_fp_32x32_c;
            //if (flags & HAS_SSSE3) vp9_quantize_fp_32x32 = vp9_quantize_fp_32x32_ssse3;
            vp9_scale_and_extend_frame = vp9_scale_and_extend_frame_c;
            if (flags & HAS_SSSE3) vp9_scale_and_extend_frame = vp9_scale_and_extend_frame_ssse3;
            vp9_temporal_filter_apply = vp9_temporal_filter_apply_c;
            if (flags & HAS_SSE4_1) vp9_temporal_filter_apply = vp9_temporal_filter_apply_sse4_1;
        }
        #endif

        #ifdef __cplusplus
        }  // extern "C"
        #endif

        #endif
    )");

    vpx.writeFileOnce("vpx_scale_rtcd.h", R"(
        // This file is generated. Do not edit.
        #ifndef VPX_SCALE_RTCD_H_
        #define VPX_SCALE_RTCD_H_

        #ifdef RTCD_C
        #define RTCD_EXTERN
        #else
        #define RTCD_EXTERN extern
        #endif

        struct yv12_buffer_config;

        #ifdef __cplusplus
        extern "C" {
        #endif

        void vp8_horizontal_line_2_1_scale_c(const unsigned char *source, unsigned int source_width, unsigned char *dest, unsigned int dest_width);
        #define vp8_horizontal_line_2_1_scale vp8_horizontal_line_2_1_scale_c

        void vp8_horizontal_line_5_3_scale_c(const unsigned char *source, unsigned int source_width, unsigned char *dest, unsigned int dest_width);
        #define vp8_horizontal_line_5_3_scale vp8_horizontal_line_5_3_scale_c

        void vp8_horizontal_line_5_4_scale_c(const unsigned char *source, unsigned int source_width, unsigned char *dest, unsigned int dest_width);
        #define vp8_horizontal_line_5_4_scale vp8_horizontal_line_5_4_scale_c

        void vp8_vertical_band_2_1_scale_c(unsigned char *source, unsigned int src_pitch, unsigned char *dest, unsigned int dest_pitch, unsigned int dest_width);
        #define vp8_vertical_band_2_1_scale vp8_vertical_band_2_1_scale_c

        void vp8_vertical_band_2_1_scale_i_c(unsigned char *source, unsigned int src_pitch, unsigned char *dest, unsigned int dest_pitch, unsigned int dest_width);
        #define vp8_vertical_band_2_1_scale_i vp8_vertical_band_2_1_scale_i_c

        void vp8_vertical_band_5_3_scale_c(unsigned char *source, unsigned int src_pitch, unsigned char *dest, unsigned int dest_pitch, unsigned int dest_width);
        #define vp8_vertical_band_5_3_scale vp8_vertical_band_5_3_scale_c

        void vp8_vertical_band_5_4_scale_c(unsigned char *source, unsigned int src_pitch, unsigned char *dest, unsigned int dest_pitch, unsigned int dest_width);
        #define vp8_vertical_band_5_4_scale vp8_vertical_band_5_4_scale_c

        void vp8_yv12_copy_frame_c(const struct yv12_buffer_config *src_ybc, struct yv12_buffer_config *dst_ybc);
        #define vp8_yv12_copy_frame vp8_yv12_copy_frame_c

        void vp8_yv12_extend_frame_borders_c(struct yv12_buffer_config *ybf);
        #define vp8_yv12_extend_frame_borders vp8_yv12_extend_frame_borders_c

        void vpx_extend_frame_borders_c(struct yv12_buffer_config *ybf);
        #define vpx_extend_frame_borders vpx_extend_frame_borders_c

        void vpx_extend_frame_inner_borders_c(struct yv12_buffer_config *ybf);
        #define vpx_extend_frame_inner_borders vpx_extend_frame_inner_borders_c

        void vpx_yv12_copy_frame_c(const struct yv12_buffer_config *src_ybc, struct yv12_buffer_config *dst_ybc);
        #define vpx_yv12_copy_frame vpx_yv12_copy_frame_c

        void vpx_yv12_copy_y_c(const struct yv12_buffer_config *src_ybc, struct yv12_buffer_config *dst_ybc);
        #define vpx_yv12_copy_y vpx_yv12_copy_y_c

        void vpx_scale_rtcd(void);

        #ifdef RTCD_C
        #include "vpx_ports/x86.h"
        static void setup_rtcd_internal(void)
        {
            int flags = x86_simd_caps();

            (void)flags;

        }
        #endif

        #ifdef __cplusplus
        }  // extern "C"
        #endif

        #endif
    )");

    {
        std::ostringstream oss;
        oss <<
            R"(
        // This file is generated. Do not edit.
        #ifndef VPX_DSP_RTCD_H_
        #define VPX_DSP_RTCD_H_

        #ifdef RTCD_C
        #define RTCD_EXTERN
        #else
        #define RTCD_EXTERN extern
        #endif

        /*
         * DSP
         */

        #include "vpx/vpx_integer.h"
        #include "vpx_dsp/vpx_dsp_common.h"
        #include "vpx_dsp/vpx_filter.h"


        #ifdef __cplusplus
        extern "C" {
        #endif

        void vpx_ssim_parms_8x8_c(const uint8_t *s, int sp, const uint8_t *r, int rp, uint32_t *sum_s, uint32_t *sum_r, uint32_t *sum_sq_s, uint32_t *sum_sq_r, uint32_t *sum_sxr);
        void vpx_ssim_parms_8x8_sse2(const uint8_t *s, int sp, const uint8_t *r, int rp, uint32_t *sum_s, uint32_t *sum_r, uint32_t *sum_sq_s, uint32_t *sum_sq_r, uint32_t *sum_sxr);
        #define vpx_ssim_parms_8x8 vpx_ssim_parms_8x8_c

        unsigned int vpx_avg_4x4_c(const uint8_t *, int p);
        unsigned int vpx_avg_4x4_sse2(const uint8_t *, int p);
        #define vpx_avg_4x4 vpx_avg_4x4_sse2

        unsigned int vpx_avg_8x8_c(const uint8_t *, int p);
        unsigned int vpx_avg_8x8_sse2(const uint8_t *, int p);
        #define vpx_avg_8x8 vpx_avg_8x8_sse2

        void vpx_comp_avg_pred_c(uint8_t *comp_pred, const uint8_t *pred, int width, int height, const uint8_t *ref, int ref_stride);
        void vpx_comp_avg_pred_sse2(uint8_t *comp_pred, const uint8_t *pred, int width, int height, const uint8_t *ref, int ref_stride);
        #define vpx_comp_avg_pred vpx_comp_avg_pred_sse2

        void vpx_convolve8_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        void vpx_convolve8_sse2(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        void vpx_convolve8_ssse3(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        //void vpx_convolve8_avx2(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        RTCD_EXTERN void (*vpx_convolve8)(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);

        void vpx_convolve8_avg_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        void vpx_convolve8_avg_sse2(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        void vpx_convolve8_avg_ssse3(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        //void vpx_convolve8_avg_avx2(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        RTCD_EXTERN void (*vpx_convolve8_avg)(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);

        void vpx_convolve8_avg_horiz_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        void vpx_convolve8_avg_horiz_sse2(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        void vpx_convolve8_avg_horiz_ssse3(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        //void vpx_convolve8_avg_horiz_avx2(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        RTCD_EXTERN void (*vpx_convolve8_avg_horiz)(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);

        void vpx_convolve8_avg_vert_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        void vpx_convolve8_avg_vert_sse2(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        void vpx_convolve8_avg_vert_ssse3(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        //void vpx_convolve8_avg_vert_avx2(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        RTCD_EXTERN void (*vpx_convolve8_avg_vert)(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);

        void vpx_convolve8_horiz_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        void vpx_convolve8_horiz_sse2(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        void vpx_convolve8_horiz_ssse3(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        //void vpx_convolve8_horiz_avx2(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        RTCD_EXTERN void (*vpx_convolve8_horiz)(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);

        void vpx_convolve8_vert_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        void vpx_convolve8_vert_sse2(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        void vpx_convolve8_vert_ssse3(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        //void vpx_convolve8_vert_avx2(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        RTCD_EXTERN void (*vpx_convolve8_vert)(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);

        void vpx_convolve_avg_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        void vpx_convolve_avg_sse2(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        #define vpx_convolve_avg vpx_convolve_avg_sse2

        void vpx_convolve_copy_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        void vpx_convolve_copy_sse2(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        #define vpx_convolve_copy vpx_convolve_copy_sse2

        void vpx_d117_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_d117_predictor_16x16 vpx_d117_predictor_16x16_c

        void vpx_d117_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_d117_predictor_32x32 vpx_d117_predictor_32x32_c

        void vpx_d117_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_d117_predictor_4x4 vpx_d117_predictor_4x4_c

        void vpx_d117_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_d117_predictor_8x8 vpx_d117_predictor_8x8_c

        void vpx_d135_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_d135_predictor_16x16 vpx_d135_predictor_16x16_c

        void vpx_d135_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_d135_predictor_32x32 vpx_d135_predictor_32x32_c


)" <<
R"(

        void vpx_d135_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_d135_predictor_4x4 vpx_d135_predictor_4x4_c

        void vpx_d135_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_d135_predictor_8x8 vpx_d135_predictor_8x8_c

        void vpx_d153_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_d153_predictor_16x16_ssse3(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        RTCD_EXTERN void (*vpx_d153_predictor_16x16)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

        void vpx_d153_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_d153_predictor_32x32_ssse3(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        RTCD_EXTERN void (*vpx_d153_predictor_32x32)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

        void vpx_d153_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_d153_predictor_4x4_ssse3(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        RTCD_EXTERN void (*vpx_d153_predictor_4x4)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

        void vpx_d153_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_d153_predictor_8x8_ssse3(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        RTCD_EXTERN void (*vpx_d153_predictor_8x8)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

        void vpx_d207_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_d207_predictor_16x16_ssse3(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        RTCD_EXTERN void (*vpx_d207_predictor_16x16)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

        void vpx_d207_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_d207_predictor_32x32_ssse3(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        RTCD_EXTERN void (*vpx_d207_predictor_32x32)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

        void vpx_d207_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_d207_predictor_4x4_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_d207_predictor_4x4 vpx_d207_predictor_4x4_sse2

        void vpx_d207_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_d207_predictor_8x8_ssse3(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        RTCD_EXTERN void (*vpx_d207_predictor_8x8)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

        void vpx_d45_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_d45_predictor_16x16_ssse3(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        RTCD_EXTERN void (*vpx_d45_predictor_16x16)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

        void vpx_d45_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_d45_predictor_32x32_ssse3(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        RTCD_EXTERN void (*vpx_d45_predictor_32x32)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

        void vpx_d45_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_d45_predictor_4x4_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_d45_predictor_4x4 vpx_d45_predictor_4x4_sse2

        void vpx_d45_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_d45_predictor_8x8_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_d45_predictor_8x8 vpx_d45_predictor_8x8_sse2

        void vpx_d45e_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_d45e_predictor_4x4 vpx_d45e_predictor_4x4_c

        void vpx_d63_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_d63_predictor_16x16_ssse3(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        RTCD_EXTERN void (*vpx_d63_predictor_16x16)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

        void vpx_d63_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_d63_predictor_32x32_ssse3(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        RTCD_EXTERN void (*vpx_d63_predictor_32x32)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

        void vpx_d63_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_d63_predictor_4x4_ssse3(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        RTCD_EXTERN void (*vpx_d63_predictor_4x4)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

        void vpx_d63_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_d63_predictor_8x8_ssse3(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        RTCD_EXTERN void (*vpx_d63_predictor_8x8)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

        void vpx_d63e_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_d63e_predictor_4x4 vpx_d63e_predictor_4x4_c

        void vpx_dc_128_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_dc_128_predictor_16x16_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_dc_128_predictor_16x16 vpx_dc_128_predictor_16x16_sse2

        void vpx_dc_128_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_dc_128_predictor_32x32_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_dc_128_predictor_32x32 vpx_dc_128_predictor_32x32_sse2

        void vpx_dc_128_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_dc_128_predictor_4x4_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_dc_128_predictor_4x4 vpx_dc_128_predictor_4x4_sse2

        void vpx_dc_128_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_dc_128_predictor_8x8_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_dc_128_predictor_8x8 vpx_dc_128_predictor_8x8_sse2

        void vpx_dc_left_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_dc_left_predictor_16x16_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_dc_left_predictor_16x16 vpx_dc_left_predictor_16x16_sse2

        void vpx_dc_left_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_dc_left_predictor_32x32_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_dc_left_predictor_32x32 vpx_dc_left_predictor_32x32_sse2

        void vpx_dc_left_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_dc_left_predictor_4x4_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_dc_left_predictor_4x4 vpx_dc_left_predictor_4x4_sse2

        void vpx_dc_left_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_dc_left_predictor_8x8_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_dc_left_predictor_8x8 vpx_dc_left_predictor_8x8_sse2

        void vpx_dc_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_dc_predictor_16x16_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_dc_predictor_16x16 vpx_dc_predictor_16x16_sse2

        void vpx_dc_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_dc_predictor_32x32_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_dc_predictor_32x32 vpx_dc_predictor_32x32_sse2

        void vpx_dc_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_dc_predictor_4x4_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_dc_predictor_4x4 vpx_dc_predictor_4x4_sse2


)" <<
R"(

        void vpx_dc_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_dc_predictor_8x8_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_dc_predictor_8x8 vpx_dc_predictor_8x8_sse2

        void vpx_dc_top_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_dc_top_predictor_16x16_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_dc_top_predictor_16x16 vpx_dc_top_predictor_16x16_sse2

        void vpx_dc_top_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_dc_top_predictor_32x32_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_dc_top_predictor_32x32 vpx_dc_top_predictor_32x32_sse2

        void vpx_dc_top_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_dc_top_predictor_4x4_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_dc_top_predictor_4x4 vpx_dc_top_predictor_4x4_sse2

        void vpx_dc_top_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_dc_top_predictor_8x8_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_dc_top_predictor_8x8 vpx_dc_top_predictor_8x8_sse2

        void vpx_fdct16x16_c(const int16_t *input, tran_low_t *output, int stride);
        void vpx_fdct16x16_sse2(const int16_t *input, tran_low_t *output, int stride);
        #define vpx_fdct16x16 vpx_fdct16x16_sse2

        void vpx_fdct16x16_1_c(const int16_t *input, tran_low_t *output, int stride);
        void vpx_fdct16x16_1_sse2(const int16_t *input, tran_low_t *output, int stride);
        #define vpx_fdct16x16_1 vpx_fdct16x16_1_sse2

        void vpx_fdct32x32_c(const int16_t *input, tran_low_t *output, int stride);
        void vpx_fdct32x32_sse2(const int16_t *input, tran_low_t *output, int stride);
        void vpx_fdct32x32_avx2(const int16_t *input, tran_low_t *output, int stride);
        RTCD_EXTERN void (*vpx_fdct32x32)(const int16_t *input, tran_low_t *output, int stride);

        void vpx_fdct32x32_1_c(const int16_t *input, tran_low_t *output, int stride);
        void vpx_fdct32x32_1_sse2(const int16_t *input, tran_low_t *output, int stride);
        #define vpx_fdct32x32_1 vpx_fdct32x32_1_sse2

        void vpx_fdct32x32_rd_c(const int16_t *input, tran_low_t *output, int stride);
        void vpx_fdct32x32_rd_sse2(const int16_t *input, tran_low_t *output, int stride);
        void vpx_fdct32x32_rd_avx2(const int16_t *input, tran_low_t *output, int stride);
        RTCD_EXTERN void (*vpx_fdct32x32_rd)(const int16_t *input, tran_low_t *output, int stride);

        void vpx_fdct4x4_c(const int16_t *input, tran_low_t *output, int stride);
        void vpx_fdct4x4_sse2(const int16_t *input, tran_low_t *output, int stride);
        #define vpx_fdct4x4 vpx_fdct4x4_sse2

        void vpx_fdct4x4_1_c(const int16_t *input, tran_low_t *output, int stride);
        void vpx_fdct4x4_1_sse2(const int16_t *input, tran_low_t *output, int stride);
        #define vpx_fdct4x4_1 vpx_fdct4x4_1_sse2

        void vpx_fdct8x8_c(const int16_t *input, tran_low_t *output, int stride);
        void vpx_fdct8x8_sse2(const int16_t *input, tran_low_t *output, int stride);
        //void vpx_fdct8x8_ssse3(const int16_t *input, tran_low_t *output, int stride);
        RTCD_EXTERN void (*vpx_fdct8x8)(const int16_t *input, tran_low_t *output, int stride);

        void vpx_fdct8x8_1_c(const int16_t *input, tran_low_t *output, int stride);
        void vpx_fdct8x8_1_sse2(const int16_t *input, tran_low_t *output, int stride);
        #define vpx_fdct8x8_1 vpx_fdct8x8_1_sse2

        void vpx_get16x16var_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, int *sum);
        void vpx_get16x16var_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, int *sum);
        void vpx_get16x16var_avx2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, int *sum);
        RTCD_EXTERN void (*vpx_get16x16var)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, int *sum);

        unsigned int vpx_get4x4sse_cs_c(const unsigned char *src_ptr, int source_stride, const unsigned char *ref_ptr, int  ref_stride);
        #define vpx_get4x4sse_cs vpx_get4x4sse_cs_c

        void vpx_get8x8var_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, int *sum);
        void vpx_get8x8var_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, int *sum);
        #define vpx_get8x8var vpx_get8x8var_sse2

        unsigned int vpx_get_mb_ss_c(const int16_t *);
        unsigned int vpx_get_mb_ss_sse2(const int16_t *);
        #define vpx_get_mb_ss vpx_get_mb_ss_sse2

        void vpx_h_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_h_predictor_16x16_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_h_predictor_16x16 vpx_h_predictor_16x16_sse2

        void vpx_h_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_h_predictor_32x32_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_h_predictor_32x32 vpx_h_predictor_32x32_sse2

        void vpx_h_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_h_predictor_4x4_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_h_predictor_4x4 vpx_h_predictor_4x4_sse2

        void vpx_h_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_h_predictor_8x8_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_h_predictor_8x8 vpx_h_predictor_8x8_sse2

        void vpx_hadamard_32x32_c(const int16_t *src_diff, ptrdiff_t src_stride, int16_t *coeff);
        void vpx_hadamard_32x32_sse2(const int16_t *src_diff, ptrdiff_t src_stride, int16_t *coeff);
        void vpx_hadamard_32x32_avx2(const int16_t *src_diff, ptrdiff_t src_stride, int16_t *coeff);
        RTCD_EXTERN void (*vpx_hadamard_32x32)(const int16_t *src_diff, ptrdiff_t src_stride, int16_t *coeff);

        void vpx_hadamard_16x16_c(const int16_t *src_diff, ptrdiff_t src_stride, int16_t *coeff);
        void vpx_hadamard_16x16_sse2(const int16_t *src_diff, ptrdiff_t src_stride, int16_t *coeff);
        void vpx_hadamard_16x16_avx2(const int16_t *src_diff, ptrdiff_t src_stride, int16_t *coeff);
        RTCD_EXTERN void (*vpx_hadamard_16x16)(const int16_t *src_diff, ptrdiff_t src_stride, int16_t *coeff);

        void vpx_hadamard_8x8_c(const int16_t *src_diff, ptrdiff_t src_stride, int16_t *coeff);
        void vpx_hadamard_8x8_sse2(const int16_t *src_diff, ptrdiff_t src_stride, int16_t *coeff);
        //void vpx_hadamard_8x8_ssse3(const int16_t *src_diff, ptrdiff_t src_stride, int16_t *coeff);
        RTCD_EXTERN void (*vpx_hadamard_8x8)(const int16_t *src_diff, ptrdiff_t src_stride, int16_t *coeff);

        void vpx_he_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_he_predictor_4x4 vpx_he_predictor_4x4_c

        void vpx_idct16x16_10_add_c(const tran_low_t *input, uint8_t *dest, int stride);
        void vpx_idct16x16_10_add_sse2(const tran_low_t *input, uint8_t *dest, int stride);
        #define vpx_idct16x16_10_add vpx_idct16x16_10_add_sse2


)" <<
R"(

        void vpx_idct16x16_1_add_c(const tran_low_t *input, uint8_t *dest, int stride);
        void vpx_idct16x16_1_add_sse2(const tran_low_t *input, uint8_t *dest, int stride);
        #define vpx_idct16x16_1_add vpx_idct16x16_1_add_sse2

        void vpx_idct16x16_256_add_c(const tran_low_t *input, uint8_t *dest, int stride);
        void vpx_idct16x16_256_add_sse2(const tran_low_t *input, uint8_t *dest, int stride);
        #define vpx_idct16x16_256_add vpx_idct16x16_256_add_sse2

        void vpx_idct16x16_38_add_c(const tran_low_t *input, uint8_t *dest, int stride);
        void vpx_idct16x16_38_add_sse2(const tran_low_t *input, uint8_t *dest, int stride);
        #define vpx_idct16x16_38_add vpx_idct16x16_38_add_sse2

        void vpx_idct32x32_1024_add_c(const tran_low_t *input, uint8_t *dest, int stride);
        void vpx_idct32x32_1024_add_sse2(const tran_low_t *input, uint8_t *dest, int stride);
        #define vpx_idct32x32_1024_add vpx_idct32x32_1024_add_sse2

        void vpx_idct32x32_135_add_c(const tran_low_t *input, uint8_t *dest, int stride);
        void vpx_idct32x32_135_add_sse2(const tran_low_t *input, uint8_t *dest, int stride);
        void vpx_idct32x32_135_add_ssse3(const tran_low_t *input, uint8_t *dest, int stride);
        RTCD_EXTERN void (*vpx_idct32x32_135_add)(const tran_low_t *input, uint8_t *dest, int stride);

        void vpx_idct32x32_1_add_c(const tran_low_t *input, uint8_t *dest, int stride);
        void vpx_idct32x32_1_add_sse2(const tran_low_t *input, uint8_t *dest, int stride);
        #define vpx_idct32x32_1_add vpx_idct32x32_1_add_sse2

        void vpx_idct32x32_34_add_c(const tran_low_t *input, uint8_t *dest, int stride);
        void vpx_idct32x32_34_add_sse2(const tran_low_t *input, uint8_t *dest, int stride);
        void vpx_idct32x32_34_add_ssse3(const tran_low_t *input, uint8_t *dest, int stride);
        RTCD_EXTERN void (*vpx_idct32x32_34_add)(const tran_low_t *input, uint8_t *dest, int stride);

        void vpx_idct4x4_16_add_c(const tran_low_t *input, uint8_t *dest, int stride);
        void vpx_idct4x4_16_add_sse2(const tran_low_t *input, uint8_t *dest, int stride);
        #define vpx_idct4x4_16_add vpx_idct4x4_16_add_sse2

        void vpx_idct4x4_1_add_c(const tran_low_t *input, uint8_t *dest, int stride);
        void vpx_idct4x4_1_add_sse2(const tran_low_t *input, uint8_t *dest, int stride);
        #define vpx_idct4x4_1_add vpx_idct4x4_1_add_sse2

        void vpx_idct8x8_12_add_c(const tran_low_t *input, uint8_t *dest, int stride);
        void vpx_idct8x8_12_add_sse2(const tran_low_t *input, uint8_t *dest, int stride);
        void vpx_idct8x8_12_add_ssse3(const tran_low_t *input, uint8_t *dest, int stride);
        RTCD_EXTERN void (*vpx_idct8x8_12_add)(const tran_low_t *input, uint8_t *dest, int stride);

        void vpx_idct8x8_1_add_c(const tran_low_t *input, uint8_t *dest, int stride);
        void vpx_idct8x8_1_add_sse2(const tran_low_t *input, uint8_t *dest, int stride);
        #define vpx_idct8x8_1_add vpx_idct8x8_1_add_sse2

        void vpx_idct8x8_64_add_c(const tran_low_t *input, uint8_t *dest, int stride);
        void vpx_idct8x8_64_add_sse2(const tran_low_t *input, uint8_t *dest, int stride);
        #define vpx_idct8x8_64_add vpx_idct8x8_64_add_sse2

        int16_t vpx_int_pro_col_c(const uint8_t *ref, const int width);
        int16_t vpx_int_pro_col_sse2(const uint8_t *ref, const int width);
        #define vpx_int_pro_col vpx_int_pro_col_sse2

        void vpx_int_pro_row_c(int16_t *hbuf, const uint8_t *ref, const int ref_stride, const int height);
        void vpx_int_pro_row_sse2(int16_t *hbuf, const uint8_t *ref, const int ref_stride, const int height);
        #define vpx_int_pro_row vpx_int_pro_row_sse2

        void vpx_iwht4x4_16_add_c(const tran_low_t *input, uint8_t *dest, int stride);
        void vpx_iwht4x4_16_add_sse2(const tran_low_t *input, uint8_t *dest, int stride);
        #define vpx_iwht4x4_16_add vpx_iwht4x4_16_add_sse2

        void vpx_iwht4x4_1_add_c(const tran_low_t *input, uint8_t *dest, int stride);
        #define vpx_iwht4x4_1_add vpx_iwht4x4_1_add_c

        void vpx_lpf_horizontal_16_c(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh);
        void vpx_lpf_horizontal_16_sse2(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh);
        void vpx_lpf_horizontal_16_avx2(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh);
        RTCD_EXTERN void (*vpx_lpf_horizontal_16)(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh);

        void vpx_lpf_horizontal_16_dual_c(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh);
        void vpx_lpf_horizontal_16_dual_sse2(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh);
        void vpx_lpf_horizontal_16_dual_avx2(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh);
        RTCD_EXTERN void (*vpx_lpf_horizontal_16_dual)(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh);

        void vpx_lpf_horizontal_4_c(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh);
        void vpx_lpf_horizontal_4_sse2(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh);
        #define vpx_lpf_horizontal_4 vpx_lpf_horizontal_4_sse2

        void vpx_lpf_horizontal_4_dual_c(uint8_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0, const uint8_t *blimit1, const uint8_t *limit1, const uint8_t *thresh1);
        void vpx_lpf_horizontal_4_dual_sse2(uint8_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0, const uint8_t *blimit1, const uint8_t *limit1, const uint8_t *thresh1);
        #define vpx_lpf_horizontal_4_dual vpx_lpf_horizontal_4_dual_sse2

        void vpx_lpf_horizontal_8_c(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh);
        void vpx_lpf_horizontal_8_sse2(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh);
        #define vpx_lpf_horizontal_8 vpx_lpf_horizontal_8_sse2

        void vpx_lpf_horizontal_8_dual_c(uint8_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0, const uint8_t *blimit1, const uint8_t *limit1, const uint8_t *thresh1);
        void vpx_lpf_horizontal_8_dual_sse2(uint8_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0, const uint8_t *blimit1, const uint8_t *limit1, const uint8_t *thresh1);
        #define vpx_lpf_horizontal_8_dual vpx_lpf_horizontal_8_dual_sse2

        void vpx_lpf_vertical_16_c(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh);
        void vpx_lpf_vertical_16_sse2(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh);
        #define vpx_lpf_vertical_16 vpx_lpf_vertical_16_sse2

        void vpx_lpf_vertical_16_dual_c(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh);
        void vpx_lpf_vertical_16_dual_sse2(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh);
        #define vpx_lpf_vertical_16_dual vpx_lpf_vertical_16_dual_sse2

        void vpx_lpf_vertical_4_c(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh);
        void vpx_lpf_vertical_4_sse2(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh);
        #define vpx_lpf_vertical_4 vpx_lpf_vertical_4_sse2

        void vpx_lpf_vertical_4_dual_c(uint8_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0, const uint8_t *blimit1, const uint8_t *limit1, const uint8_t *thresh1);
        void vpx_lpf_vertical_4_dual_sse2(uint8_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0, const uint8_t *blimit1, const uint8_t *limit1, const uint8_t *thresh1);
        #define vpx_lpf_vertical_4_dual vpx_lpf_vertical_4_dual_sse2

        void vpx_lpf_vertical_8_c(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh);
        void vpx_lpf_vertical_8_sse2(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh);
        #define vpx_lpf_vertical_8 vpx_lpf_vertical_8_sse2


)" <<
R"(

        void vpx_lpf_vertical_8_dual_c(uint8_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0, const uint8_t *blimit1, const uint8_t *limit1, const uint8_t *thresh1);
        void vpx_lpf_vertical_8_dual_sse2(uint8_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0, const uint8_t *blimit1, const uint8_t *limit1, const uint8_t *thresh1);
        #define vpx_lpf_vertical_8_dual vpx_lpf_vertical_8_dual_sse2

        void vpx_mbpost_proc_across_ip_c(unsigned char *dst, int pitch, int rows, int cols,int flimit);
        void vpx_mbpost_proc_across_ip_sse2(unsigned char *dst, int pitch, int rows, int cols,int flimit);
        #define vpx_mbpost_proc_across_ip vpx_mbpost_proc_across_ip_sse2

        void vpx_mbpost_proc_down_c(unsigned char *dst, int pitch, int rows, int cols,int flimit);
        void vpx_mbpost_proc_down_sse2(unsigned char *dst, int pitch, int rows, int cols,int flimit);
        #define vpx_mbpost_proc_down vpx_mbpost_proc_down_sse2

        void vpx_minmax_8x8_c(const uint8_t *s, int p, const uint8_t *d, int dp, int *min, int *max);
        void vpx_minmax_8x8_sse2(const uint8_t *s, int p, const uint8_t *d, int dp, int *min, int *max);
        #define vpx_minmax_8x8 vpx_minmax_8x8_sse2

        unsigned int vpx_mse16x16_c(const uint8_t *src_ptr, int  source_stride, const uint8_t *ref_ptr, int  recon_stride, unsigned int *sse);
        unsigned int vpx_mse16x16_sse2(const uint8_t *src_ptr, int  source_stride, const uint8_t *ref_ptr, int  recon_stride, unsigned int *sse);
        unsigned int vpx_mse16x16_avx2(const uint8_t *src_ptr, int  source_stride, const uint8_t *ref_ptr, int  recon_stride, unsigned int *sse);
        RTCD_EXTERN unsigned int (*vpx_mse16x16)(const uint8_t *src_ptr, int  source_stride, const uint8_t *ref_ptr, int  recon_stride, unsigned int *sse);

        unsigned int vpx_mse16x8_c(const uint8_t *src_ptr, int  source_stride, const uint8_t *ref_ptr, int  recon_stride, unsigned int *sse);
        unsigned int vpx_mse16x8_sse2(const uint8_t *src_ptr, int  source_stride, const uint8_t *ref_ptr, int  recon_stride, unsigned int *sse);
        //unsigned int vpx_mse16x8_avx2(const uint8_t *src_ptr, int  source_stride, const uint8_t *ref_ptr, int  recon_stride, unsigned int *sse);
        RTCD_EXTERN unsigned int (*vpx_mse16x8)(const uint8_t *src_ptr, int  source_stride, const uint8_t *ref_ptr, int  recon_stride, unsigned int *sse);

        unsigned int vpx_mse8x16_c(const uint8_t *src_ptr, int  source_stride, const uint8_t *ref_ptr, int  recon_stride, unsigned int *sse);
        unsigned int vpx_mse8x16_sse2(const uint8_t *src_ptr, int  source_stride, const uint8_t *ref_ptr, int  recon_stride, unsigned int *sse);
        #define vpx_mse8x16 vpx_mse8x16_sse2

        unsigned int vpx_mse8x8_c(const uint8_t *src_ptr, int  source_stride, const uint8_t *ref_ptr, int  recon_stride, unsigned int *sse);
        unsigned int vpx_mse8x8_sse2(const uint8_t *src_ptr, int  source_stride, const uint8_t *ref_ptr, int  recon_stride, unsigned int *sse);
        #define vpx_mse8x8 vpx_mse8x8_sse2

        void vpx_plane_add_noise_c(uint8_t *start, const int8_t *noise, int blackclamp, int whiteclamp, int width, int height, int pitch);
        void vpx_plane_add_noise_sse2(uint8_t *start, const int8_t *noise, int blackclamp, int whiteclamp, int width, int height, int pitch);
        #define vpx_plane_add_noise vpx_plane_add_noise_sse2

        void vpx_post_proc_down_and_across_mb_row_c(unsigned char *src, unsigned char *dst, int src_pitch, int dst_pitch, int cols, unsigned char *flimits, int size);
        void vpx_post_proc_down_and_across_mb_row_sse2(unsigned char *src, unsigned char *dst, int src_pitch, int dst_pitch, int cols, unsigned char *flimits, int size);
        #define vpx_post_proc_down_and_across_mb_row vpx_post_proc_down_and_across_mb_row_sse2

        void vpx_quantize_b_c(const tran_low_t *coeff_ptr, intptr_t n_coeffs, int skip_block, const int16_t *zbin_ptr, const int16_t *round_ptr, const int16_t *quant_ptr, const int16_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
        void vpx_quantize_b_sse2(const tran_low_t *coeff_ptr, intptr_t n_coeffs, int skip_block, const int16_t *zbin_ptr, const int16_t *round_ptr, const int16_t *quant_ptr, const int16_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
        void vpx_quantize_b_ssse3(const tran_low_t *coeff_ptr, intptr_t n_coeffs, int skip_block, const int16_t *zbin_ptr, const int16_t *round_ptr, const int16_t *quant_ptr, const int16_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
        void vpx_quantize_b_avx(const tran_low_t *coeff_ptr, intptr_t n_coeffs, int skip_block, const int16_t *zbin_ptr, const int16_t *round_ptr, const int16_t *quant_ptr, const int16_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
        RTCD_EXTERN void (*vpx_quantize_b)(const tran_low_t *coeff_ptr, intptr_t n_coeffs, int skip_block, const int16_t *zbin_ptr, const int16_t *round_ptr, const int16_t *quant_ptr, const int16_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);

        void vpx_quantize_b_32x32_c(const tran_low_t *coeff_ptr, intptr_t n_coeffs, int skip_block, const int16_t *zbin_ptr, const int16_t *round_ptr, const int16_t *quant_ptr, const int16_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
        void vpx_quantize_b_32x32_ssse3(const tran_low_t *coeff_ptr, intptr_t n_coeffs, int skip_block, const int16_t *zbin_ptr, const int16_t *round_ptr, const int16_t *quant_ptr, const int16_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
        void vpx_quantize_b_32x32_avx(const tran_low_t *coeff_ptr, intptr_t n_coeffs, int skip_block, const int16_t *zbin_ptr, const int16_t *round_ptr, const int16_t *quant_ptr, const int16_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
        RTCD_EXTERN void (*vpx_quantize_b_32x32)(const tran_low_t *coeff_ptr, intptr_t n_coeffs, int skip_block, const int16_t *zbin_ptr, const int16_t *round_ptr, const int16_t *quant_ptr, const int16_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);

        unsigned int vpx_sad16x16_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        unsigned int vpx_sad16x16_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        #define vpx_sad16x16 vpx_sad16x16_sse2

        unsigned int vpx_sad16x16_avg_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        unsigned int vpx_sad16x16_avg_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        #define vpx_sad16x16_avg vpx_sad16x16_avg_sse2

        void vpx_sad16x16x3_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);
        void vpx_sad16x16x3_sse3(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);
        void vpx_sad16x16x3_ssse3(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);
        RTCD_EXTERN void (*vpx_sad16x16x3)(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);

        void vpx_sad16x16x4d_c(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        void vpx_sad16x16x4d_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        #define vpx_sad16x16x4d vpx_sad16x16x4d_sse2

        void vpx_sad16x16x8_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);
        void vpx_sad16x16x8_sse4_1(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);
        RTCD_EXTERN void (*vpx_sad16x16x8)(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);

        unsigned int vpx_sad16x32_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        unsigned int vpx_sad16x32_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        #define vpx_sad16x32 vpx_sad16x32_sse2

        unsigned int vpx_sad16x32_avg_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        unsigned int vpx_sad16x32_avg_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        #define vpx_sad16x32_avg vpx_sad16x32_avg_sse2

        void vpx_sad16x32x4d_c(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        void vpx_sad16x32x4d_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        #define vpx_sad16x32x4d vpx_sad16x32x4d_sse2

        unsigned int vpx_sad16x8_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        unsigned int vpx_sad16x8_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        #define vpx_sad16x8 vpx_sad16x8_sse2

        unsigned int vpx_sad16x8_avg_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        unsigned int vpx_sad16x8_avg_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        #define vpx_sad16x8_avg vpx_sad16x8_avg_sse2


)" <<
R"(

        void vpx_sad16x8x3_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);
        void vpx_sad16x8x3_sse3(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);
        void vpx_sad16x8x3_ssse3(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);
        RTCD_EXTERN void (*vpx_sad16x8x3)(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);

        void vpx_sad16x8x4d_c(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        void vpx_sad16x8x4d_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        #define vpx_sad16x8x4d vpx_sad16x8x4d_sse2

        void vpx_sad16x8x8_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);
        void vpx_sad16x8x8_sse4_1(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);
        RTCD_EXTERN void (*vpx_sad16x8x8)(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);

        unsigned int vpx_sad32x16_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        unsigned int vpx_sad32x16_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        unsigned int vpx_sad32x16_avx2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        RTCD_EXTERN unsigned int (*vpx_sad32x16)(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);

        unsigned int vpx_sad32x16_avg_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        unsigned int vpx_sad32x16_avg_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        unsigned int vpx_sad32x16_avg_avx2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        RTCD_EXTERN unsigned int (*vpx_sad32x16_avg)(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);

        void vpx_sad32x16x4d_c(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        void vpx_sad32x16x4d_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        #define vpx_sad32x16x4d vpx_sad32x16x4d_sse2

        unsigned int vpx_sad32x32_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        unsigned int vpx_sad32x32_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        unsigned int vpx_sad32x32_avx2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        RTCD_EXTERN unsigned int (*vpx_sad32x32)(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);

        unsigned int vpx_sad32x32_avg_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        unsigned int vpx_sad32x32_avg_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        unsigned int vpx_sad32x32_avg_avx2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        RTCD_EXTERN unsigned int (*vpx_sad32x32_avg)(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);

        void vpx_sad32x32x4d_c(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        void vpx_sad32x32x4d_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        void vpx_sad32x32x4d_avx2(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        RTCD_EXTERN void (*vpx_sad32x32x4d)(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);

        unsigned int vpx_sad32x64_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        unsigned int vpx_sad32x64_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        unsigned int vpx_sad32x64_avx2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        RTCD_EXTERN unsigned int (*vpx_sad32x64)(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);

        unsigned int vpx_sad32x64_avg_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        unsigned int vpx_sad32x64_avg_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        unsigned int vpx_sad32x64_avg_avx2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        RTCD_EXTERN unsigned int (*vpx_sad32x64_avg)(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);

        void vpx_sad32x64x4d_c(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        void vpx_sad32x64x4d_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        #define vpx_sad32x64x4d vpx_sad32x64x4d_sse2

        unsigned int vpx_sad4x4_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        unsigned int vpx_sad4x4_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        #define vpx_sad4x4 vpx_sad4x4_sse2

        unsigned int vpx_sad4x4_avg_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        unsigned int vpx_sad4x4_avg_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        #define vpx_sad4x4_avg vpx_sad4x4_avg_sse2

        void vpx_sad4x4x3_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);
        void vpx_sad4x4x3_sse3(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);
        RTCD_EXTERN void (*vpx_sad4x4x3)(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);

        void vpx_sad4x4x4d_c(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        void vpx_sad4x4x4d_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        #define vpx_sad4x4x4d vpx_sad4x4x4d_sse2

        void vpx_sad4x4x8_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);
        void vpx_sad4x4x8_sse4_1(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);
        RTCD_EXTERN void (*vpx_sad4x4x8)(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);

        unsigned int vpx_sad4x8_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        unsigned int vpx_sad4x8_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        #define vpx_sad4x8 vpx_sad4x8_sse2

        unsigned int vpx_sad4x8_avg_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        unsigned int vpx_sad4x8_avg_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        #define vpx_sad4x8_avg vpx_sad4x8_avg_sse2

        void vpx_sad4x8x4d_c(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        void vpx_sad4x8x4d_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        #define vpx_sad4x8x4d vpx_sad4x8x4d_sse2


)" <<
R"(

        unsigned int vpx_sad64x32_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        unsigned int vpx_sad64x32_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        unsigned int vpx_sad64x32_avx2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        RTCD_EXTERN unsigned int (*vpx_sad64x32)(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);

        unsigned int vpx_sad64x32_avg_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        unsigned int vpx_sad64x32_avg_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        unsigned int vpx_sad64x32_avg_avx2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        RTCD_EXTERN unsigned int (*vpx_sad64x32_avg)(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);

        void vpx_sad64x32x4d_c(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        void vpx_sad64x32x4d_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        #define vpx_sad64x32x4d vpx_sad64x32x4d_sse2

        unsigned int vpx_sad64x64_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        unsigned int vpx_sad64x64_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        unsigned int vpx_sad64x64_avx2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        RTCD_EXTERN unsigned int (*vpx_sad64x64)(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);

        unsigned int vpx_sad64x64_avg_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        unsigned int vpx_sad64x64_avg_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        unsigned int vpx_sad64x64_avg_avx2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        RTCD_EXTERN unsigned int (*vpx_sad64x64_avg)(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);

        void vpx_sad64x64x4d_c(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        void vpx_sad64x64x4d_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        void vpx_sad64x64x4d_avx2(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        void vpx_sad64x64x4d_avx512(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        RTCD_EXTERN void (*vpx_sad64x64x4d)(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);

        unsigned int vpx_sad8x16_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        unsigned int vpx_sad8x16_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        #define vpx_sad8x16 vpx_sad8x16_sse2

        unsigned int vpx_sad8x16_avg_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        unsigned int vpx_sad8x16_avg_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        #define vpx_sad8x16_avg vpx_sad8x16_avg_sse2

        void vpx_sad8x16x3_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);
        void vpx_sad8x16x3_sse3(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);
        RTCD_EXTERN void (*vpx_sad8x16x3)(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);

        void vpx_sad8x16x4d_c(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        void vpx_sad8x16x4d_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        #define vpx_sad8x16x4d vpx_sad8x16x4d_sse2

        void vpx_sad8x16x8_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);
        void vpx_sad8x16x8_sse4_1(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);
        RTCD_EXTERN void (*vpx_sad8x16x8)(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);

        unsigned int vpx_sad8x4_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        unsigned int vpx_sad8x4_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        #define vpx_sad8x4 vpx_sad8x4_sse2

        unsigned int vpx_sad8x4_avg_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        unsigned int vpx_sad8x4_avg_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        #define vpx_sad8x4_avg vpx_sad8x4_avg_sse2

        void vpx_sad8x4x4d_c(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        void vpx_sad8x4x4d_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        #define vpx_sad8x4x4d vpx_sad8x4x4d_sse2

        unsigned int vpx_sad8x8_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        unsigned int vpx_sad8x8_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride);
        #define vpx_sad8x8 vpx_sad8x8_sse2

        unsigned int vpx_sad8x8_avg_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        unsigned int vpx_sad8x8_avg_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred);
        #define vpx_sad8x8_avg vpx_sad8x8_avg_sse2

        void vpx_sad8x8x3_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);
        void vpx_sad8x8x3_sse3(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);
        RTCD_EXTERN void (*vpx_sad8x8x3)(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);

        void vpx_sad8x8x4d_c(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        void vpx_sad8x8x4d_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
        #define vpx_sad8x8x4d vpx_sad8x8x4d_sse2

        void vpx_sad8x8x8_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);
        void vpx_sad8x8x8_sse4_1(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);
        RTCD_EXTERN void (*vpx_sad8x8x8)(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);

        int vpx_satd_c(const int16_t *coeff, int length);
        int vpx_satd_sse2(const int16_t *coeff, int length);
        int vpx_satd_avx2(const int16_t *coeff, int length);
        RTCD_EXTERN int (*vpx_satd)(const int16_t *coeff, int length);

        void vpx_scaled_2d_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        void vpx_scaled_2d_ssse3(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        RTCD_EXTERN void (*vpx_scaled_2d)(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);

        void vpx_scaled_avg_2d_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        #define vpx_scaled_avg_2d vpx_scaled_avg_2d_c


)" <<
R"(

        void vpx_scaled_avg_horiz_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        #define vpx_scaled_avg_horiz vpx_scaled_avg_horiz_c

        void vpx_scaled_avg_vert_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        #define vpx_scaled_avg_vert vpx_scaled_avg_vert_c

        void vpx_scaled_horiz_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        #define vpx_scaled_horiz vpx_scaled_horiz_c

        void vpx_scaled_vert_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
        #define vpx_scaled_vert vpx_scaled_vert_c

        uint32_t vpx_sub_pixel_avg_variance16x16_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance16x16_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance16x16_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_avg_variance16x16)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);

        uint32_t vpx_sub_pixel_avg_variance16x32_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance16x32_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance16x32_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_avg_variance16x32)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);

        uint32_t vpx_sub_pixel_avg_variance16x8_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance16x8_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance16x8_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_avg_variance16x8)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);

        uint32_t vpx_sub_pixel_avg_variance32x16_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance32x16_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance32x16_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_avg_variance32x16)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);

        uint32_t vpx_sub_pixel_avg_variance32x32_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance32x32_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance32x32_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance32x32_avx2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_avg_variance32x32)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);

        uint32_t vpx_sub_pixel_avg_variance32x64_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance32x64_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance32x64_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_avg_variance32x64)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);

        uint32_t vpx_sub_pixel_avg_variance4x4_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance4x4_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance4x4_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_avg_variance4x4)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);

        uint32_t vpx_sub_pixel_avg_variance4x8_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance4x8_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance4x8_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_avg_variance4x8)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);

        uint32_t vpx_sub_pixel_avg_variance64x32_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance64x32_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance64x32_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_avg_variance64x32)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);

        uint32_t vpx_sub_pixel_avg_variance64x64_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance64x64_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance64x64_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance64x64_avx2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_avg_variance64x64)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);

        uint32_t vpx_sub_pixel_avg_variance8x16_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance8x16_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance8x16_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_avg_variance8x16)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);

        uint32_t vpx_sub_pixel_avg_variance8x4_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance8x4_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance8x4_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_avg_variance8x4)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);

        uint32_t vpx_sub_pixel_avg_variance8x8_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance8x8_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        uint32_t vpx_sub_pixel_avg_variance8x8_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_avg_variance8x8)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse, const uint8_t *second_pred);

        uint32_t vpx_sub_pixel_variance16x16_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance16x16_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance16x16_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_variance16x16)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);

        uint32_t vpx_sub_pixel_variance16x32_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance16x32_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance16x32_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_variance16x32)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);

        uint32_t vpx_sub_pixel_variance16x8_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance16x8_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance16x8_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_variance16x8)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);

        uint32_t vpx_sub_pixel_variance32x16_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance32x16_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance32x16_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_variance32x16)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);

        uint32_t vpx_sub_pixel_variance32x32_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance32x32_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance32x32_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance32x32_avx2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_variance32x32)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);


)" <<
R"(

        uint32_t vpx_sub_pixel_variance32x64_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance32x64_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance32x64_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_variance32x64)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);

        uint32_t vpx_sub_pixel_variance4x4_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance4x4_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance4x4_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_variance4x4)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);

        uint32_t vpx_sub_pixel_variance4x8_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance4x8_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance4x8_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_variance4x8)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);

        uint32_t vpx_sub_pixel_variance64x32_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance64x32_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance64x32_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_variance64x32)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);

        uint32_t vpx_sub_pixel_variance64x64_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance64x64_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance64x64_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance64x64_avx2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_variance64x64)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);

        uint32_t vpx_sub_pixel_variance8x16_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance8x16_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance8x16_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_variance8x16)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);

        uint32_t vpx_sub_pixel_variance8x4_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance8x4_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance8x4_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_variance8x4)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);

        uint32_t vpx_sub_pixel_variance8x8_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance8x8_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        uint32_t vpx_sub_pixel_variance8x8_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);
        RTCD_EXTERN uint32_t (*vpx_sub_pixel_variance8x8)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, uint32_t *sse);

        void vpx_subtract_block_c(int rows, int cols, int16_t *diff_ptr, ptrdiff_t diff_stride, const uint8_t *src_ptr, ptrdiff_t src_stride, const uint8_t *pred_ptr, ptrdiff_t pred_stride);
        void vpx_subtract_block_sse2(int rows, int cols, int16_t *diff_ptr, ptrdiff_t diff_stride, const uint8_t *src_ptr, ptrdiff_t src_stride, const uint8_t *pred_ptr, ptrdiff_t pred_stride);
        #define vpx_subtract_block vpx_subtract_block_sse2

        uint64_t vpx_sum_squares_2d_i16_c(const int16_t *src, int stride, int size);
        uint64_t vpx_sum_squares_2d_i16_sse2(const int16_t *src, int stride, int size);
        #define vpx_sum_squares_2d_i16 vpx_sum_squares_2d_i16_sse2

        void vpx_tm_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_tm_predictor_16x16_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_tm_predictor_16x16 vpx_tm_predictor_16x16_sse2

        void vpx_tm_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_tm_predictor_32x32_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_tm_predictor_32x32 vpx_tm_predictor_32x32_sse2

        void vpx_tm_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_tm_predictor_4x4_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_tm_predictor_4x4 vpx_tm_predictor_4x4_sse2

        void vpx_tm_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_tm_predictor_8x8_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_tm_predictor_8x8 vpx_tm_predictor_8x8_sse2

        void vpx_v_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_v_predictor_16x16_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_v_predictor_16x16 vpx_v_predictor_16x16_sse2

        void vpx_v_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_v_predictor_32x32_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_v_predictor_32x32 vpx_v_predictor_32x32_sse2

        void vpx_v_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_v_predictor_4x4_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_v_predictor_4x4 vpx_v_predictor_4x4_sse2

        void vpx_v_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        void vpx_v_predictor_8x8_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_v_predictor_8x8 vpx_v_predictor_8x8_sse2

        unsigned int vpx_variance16x16_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        unsigned int vpx_variance16x16_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        unsigned int vpx_variance16x16_avx2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        RTCD_EXTERN unsigned int (*vpx_variance16x16)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

        unsigned int vpx_variance16x32_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        unsigned int vpx_variance16x32_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        //unsigned int vpx_variance16x32_avx2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        RTCD_EXTERN unsigned int (*vpx_variance16x32)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

        unsigned int vpx_variance16x8_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        unsigned int vpx_variance16x8_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        //unsigned int vpx_variance16x8_avx2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        RTCD_EXTERN unsigned int (*vpx_variance16x8)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

        unsigned int vpx_variance32x16_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        unsigned int vpx_variance32x16_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        unsigned int vpx_variance32x16_avx2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        RTCD_EXTERN unsigned int (*vpx_variance32x16)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

        unsigned int vpx_variance32x32_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        unsigned int vpx_variance32x32_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        unsigned int vpx_variance32x32_avx2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        RTCD_EXTERN unsigned int (*vpx_variance32x32)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);


)" <<
R"(

        unsigned int vpx_variance32x64_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        unsigned int vpx_variance32x64_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        //unsigned int vpx_variance32x64_avx2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        RTCD_EXTERN unsigned int (*vpx_variance32x64)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

        unsigned int vpx_variance4x4_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        unsigned int vpx_variance4x4_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        #define vpx_variance4x4 vpx_variance4x4_sse2

        unsigned int vpx_variance4x8_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        unsigned int vpx_variance4x8_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        #define vpx_variance4x8 vpx_variance4x8_sse2

        unsigned int vpx_variance64x32_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        unsigned int vpx_variance64x32_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        unsigned int vpx_variance64x32_avx2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        RTCD_EXTERN unsigned int (*vpx_variance64x32)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

        unsigned int vpx_variance64x64_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        unsigned int vpx_variance64x64_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        unsigned int vpx_variance64x64_avx2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        RTCD_EXTERN unsigned int (*vpx_variance64x64)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

        unsigned int vpx_variance8x16_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        unsigned int vpx_variance8x16_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        #define vpx_variance8x16 vpx_variance8x16_sse2

        unsigned int vpx_variance8x4_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        unsigned int vpx_variance8x4_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        #define vpx_variance8x4 vpx_variance8x4_sse2

        unsigned int vpx_variance8x8_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        unsigned int vpx_variance8x8_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
        #define vpx_variance8x8 vpx_variance8x8_sse2

        void vpx_ve_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
        #define vpx_ve_predictor_4x4 vpx_ve_predictor_4x4_c

        int vpx_vector_var_c(const int16_t *ref, const int16_t *src, const int bwl);
        int vpx_vector_var_sse2(const int16_t *ref, const int16_t *src, const int bwl);
        #define vpx_vector_var vpx_vector_var_sse2

        void vpx_dsp_rtcd(void);

        #ifdef RTCD_C
        #include "vpx_ports/x86.h"
        static void setup_rtcd_internal(void)
        {
            int flags = x86_simd_caps();

            (void)flags;

            vpx_convolve8 = vpx_convolve8_sse2;
            if (flags & HAS_SSSE3) vpx_convolve8 = vpx_convolve8_ssse3;
            //if (flags & HAS_AVX2) vpx_convolve8 = vpx_convolve8_avx2;
            vpx_convolve8_avg = vpx_convolve8_avg_sse2;
            if (flags & HAS_SSSE3) vpx_convolve8_avg = vpx_convolve8_avg_ssse3;
            //if (flags & HAS_AVX2) vpx_convolve8_avg = vpx_convolve8_avg_avx2;
            vpx_convolve8_avg_horiz = vpx_convolve8_avg_horiz_sse2;
            if (flags & HAS_SSSE3) vpx_convolve8_avg_horiz = vpx_convolve8_avg_horiz_ssse3;
            //if (flags & HAS_AVX2) vpx_convolve8_avg_horiz = vpx_convolve8_avg_horiz_avx2;
            vpx_convolve8_avg_vert = vpx_convolve8_avg_vert_sse2;
            if (flags & HAS_SSSE3) vpx_convolve8_avg_vert = vpx_convolve8_avg_vert_ssse3;
            //if (flags & HAS_AVX2) vpx_convolve8_avg_vert = vpx_convolve8_avg_vert_avx2;
            vpx_convolve8_horiz = vpx_convolve8_horiz_sse2;
            if (flags & HAS_SSSE3) vpx_convolve8_horiz = vpx_convolve8_horiz_ssse3;
            //if (flags & HAS_AVX2) vpx_convolve8_horiz = vpx_convolve8_horiz_avx2;
            vpx_convolve8_vert = vpx_convolve8_vert_sse2;
            if (flags & HAS_SSSE3) vpx_convolve8_vert = vpx_convolve8_vert_ssse3;
            //if (flags & HAS_AVX2) vpx_convolve8_vert = vpx_convolve8_vert_avx2;
            vpx_d153_predictor_16x16 = vpx_d153_predictor_16x16_c;
            if (flags & HAS_SSSE3) vpx_d153_predictor_16x16 = vpx_d153_predictor_16x16_ssse3;
            vpx_d153_predictor_32x32 = vpx_d153_predictor_32x32_c;
            if (flags & HAS_SSSE3) vpx_d153_predictor_32x32 = vpx_d153_predictor_32x32_ssse3;
            vpx_d153_predictor_4x4 = vpx_d153_predictor_4x4_c;
            if (flags & HAS_SSSE3) vpx_d153_predictor_4x4 = vpx_d153_predictor_4x4_ssse3;
            vpx_d153_predictor_8x8 = vpx_d153_predictor_8x8_c;
            if (flags & HAS_SSSE3) vpx_d153_predictor_8x8 = vpx_d153_predictor_8x8_ssse3;
            vpx_d207_predictor_16x16 = vpx_d207_predictor_16x16_c;
            if (flags & HAS_SSSE3) vpx_d207_predictor_16x16 = vpx_d207_predictor_16x16_ssse3;
            vpx_d207_predictor_32x32 = vpx_d207_predictor_32x32_c;
            if (flags & HAS_SSSE3) vpx_d207_predictor_32x32 = vpx_d207_predictor_32x32_ssse3;
            vpx_d207_predictor_8x8 = vpx_d207_predictor_8x8_c;
            if (flags & HAS_SSSE3) vpx_d207_predictor_8x8 = vpx_d207_predictor_8x8_ssse3;
            vpx_d45_predictor_16x16 = vpx_d45_predictor_16x16_c;
            if (flags & HAS_SSSE3) vpx_d45_predictor_16x16 = vpx_d45_predictor_16x16_ssse3;
            vpx_d45_predictor_32x32 = vpx_d45_predictor_32x32_c;
            if (flags & HAS_SSSE3) vpx_d45_predictor_32x32 = vpx_d45_predictor_32x32_ssse3;
            vpx_d63_predictor_16x16 = vpx_d63_predictor_16x16_c;

)" <<
R"(

            if (flags & HAS_SSSE3) vpx_d63_predictor_16x16 = vpx_d63_predictor_16x16_ssse3;
            vpx_d63_predictor_32x32 = vpx_d63_predictor_32x32_c;
            if (flags & HAS_SSSE3) vpx_d63_predictor_32x32 = vpx_d63_predictor_32x32_ssse3;
            vpx_d63_predictor_4x4 = vpx_d63_predictor_4x4_c;
            if (flags & HAS_SSSE3) vpx_d63_predictor_4x4 = vpx_d63_predictor_4x4_ssse3;
            vpx_d63_predictor_8x8 = vpx_d63_predictor_8x8_c;
            if (flags & HAS_SSSE3) vpx_d63_predictor_8x8 = vpx_d63_predictor_8x8_ssse3;
            vpx_fdct32x32 = vpx_fdct32x32_sse2;
            if (flags & HAS_AVX2) vpx_fdct32x32 = vpx_fdct32x32_avx2;
            vpx_fdct32x32_rd = vpx_fdct32x32_rd_sse2;
            if (flags & HAS_AVX2) vpx_fdct32x32_rd = vpx_fdct32x32_rd_avx2;
            vpx_fdct8x8 = vpx_fdct8x8_sse2;
            //if (flags & HAS_SSSE3) vpx_fdct8x8 = vpx_fdct8x8_ssse3;
            vpx_get16x16var = vpx_get16x16var_sse2;
            if (flags & HAS_AVX2) vpx_get16x16var = vpx_get16x16var_avx2;
            vpx_hadamard_32x32 = vpx_hadamard_32x32_sse2;
            if (flags & HAS_AVX2) vpx_hadamard_32x32 = vpx_hadamard_32x32_avx2;
            vpx_hadamard_16x16 = vpx_hadamard_16x16_sse2;
            if (flags & HAS_AVX2) vpx_hadamard_16x16 = vpx_hadamard_16x16_avx2;
            vpx_hadamard_8x8 = vpx_hadamard_8x8_sse2;
            //if (flags & HAS_SSSE3) vpx_hadamard_8x8 = vpx_hadamard_8x8_ssse3;
            vpx_idct32x32_135_add = vpx_idct32x32_135_add_sse2;
            if (flags & HAS_SSSE3) vpx_idct32x32_135_add = vpx_idct32x32_135_add_ssse3;
            vpx_idct32x32_34_add = vpx_idct32x32_34_add_sse2;
            if (flags & HAS_SSSE3) vpx_idct32x32_34_add = vpx_idct32x32_34_add_ssse3;
            vpx_idct8x8_12_add = vpx_idct8x8_12_add_sse2;
            if (flags & HAS_SSSE3) vpx_idct8x8_12_add = vpx_idct8x8_12_add_ssse3;
            vpx_lpf_horizontal_16 = vpx_lpf_horizontal_16_sse2;
            if (flags & HAS_AVX2) vpx_lpf_horizontal_16 = vpx_lpf_horizontal_16_avx2;
            vpx_lpf_horizontal_16_dual = vpx_lpf_horizontal_16_dual_sse2;
            if (flags & HAS_AVX2) vpx_lpf_horizontal_16_dual = vpx_lpf_horizontal_16_dual_avx2;
            vpx_mse16x16 = vpx_mse16x16_sse2;
            if (flags & HAS_AVX2) vpx_mse16x16 = vpx_mse16x16_avx2;
            vpx_mse16x8 = vpx_mse16x8_sse2;
            //if (flags & HAS_AVX2) vpx_mse16x8 = vpx_mse16x8_avx2;
            vpx_quantize_b = vpx_quantize_b_sse2;
            if (flags & HAS_SSSE3) vpx_quantize_b = vpx_quantize_b_ssse3;
            if (flags & HAS_AVX) vpx_quantize_b = vpx_quantize_b_avx;
            vpx_quantize_b_32x32 = vpx_quantize_b_32x32_c;
            if (flags & HAS_SSSE3) vpx_quantize_b_32x32 = vpx_quantize_b_32x32_ssse3;
            if (flags & HAS_AVX) vpx_quantize_b_32x32 = vpx_quantize_b_32x32_avx;
            vpx_sad16x16x3 = vpx_sad16x16x3_c;
            if (flags & HAS_SSE3) vpx_sad16x16x3 = vpx_sad16x16x3_sse3;
            if (flags & HAS_SSSE3) vpx_sad16x16x3 = vpx_sad16x16x3_ssse3;
            vpx_sad16x16x8 = vpx_sad16x16x8_c;
            if (flags & HAS_SSE4_1) vpx_sad16x16x8 = vpx_sad16x16x8_sse4_1;
            vpx_sad16x8x3 = vpx_sad16x8x3_c;
            if (flags & HAS_SSE3) vpx_sad16x8x3 = vpx_sad16x8x3_sse3;
            if (flags & HAS_SSSE3) vpx_sad16x8x3 = vpx_sad16x8x3_ssse3;
            vpx_sad16x8x8 = vpx_sad16x8x8_c;
            if (flags & HAS_SSE4_1) vpx_sad16x8x8 = vpx_sad16x8x8_sse4_1;
            vpx_sad32x16 = vpx_sad32x16_sse2;
            if (flags & HAS_AVX2) vpx_sad32x16 = vpx_sad32x16_avx2;
            vpx_sad32x16_avg = vpx_sad32x16_avg_sse2;
            if (flags & HAS_AVX2) vpx_sad32x16_avg = vpx_sad32x16_avg_avx2;
            vpx_sad32x32 = vpx_sad32x32_sse2;
            if (flags & HAS_AVX2) vpx_sad32x32 = vpx_sad32x32_avx2;
            vpx_sad32x32_avg = vpx_sad32x32_avg_sse2;
            if (flags & HAS_AVX2) vpx_sad32x32_avg = vpx_sad32x32_avg_avx2;
            vpx_sad32x32x4d = vpx_sad32x32x4d_sse2;
            if (flags & HAS_AVX2) vpx_sad32x32x4d = vpx_sad32x32x4d_avx2;
            vpx_sad32x64 = vpx_sad32x64_sse2;
            if (flags & HAS_AVX2) vpx_sad32x64 = vpx_sad32x64_avx2;
            vpx_sad32x64_avg = vpx_sad32x64_avg_sse2;
            if (flags & HAS_AVX2) vpx_sad32x64_avg = vpx_sad32x64_avg_avx2;
            vpx_sad4x4x3 = vpx_sad4x4x3_c;
            if (flags & HAS_SSE3) vpx_sad4x4x3 = vpx_sad4x4x3_sse3;
            vpx_sad4x4x8 = vpx_sad4x4x8_c;
            if (flags & HAS_SSE4_1) vpx_sad4x4x8 = vpx_sad4x4x8_sse4_1;
            vpx_sad64x32 = vpx_sad64x32_sse2;
            if (flags & HAS_AVX2) vpx_sad64x32 = vpx_sad64x32_avx2;
            vpx_sad64x32_avg = vpx_sad64x32_avg_sse2;
            if (flags & HAS_AVX2) vpx_sad64x32_avg = vpx_sad64x32_avg_avx2;
            vpx_sad64x64 = vpx_sad64x64_sse2;
            if (flags & HAS_AVX2) vpx_sad64x64 = vpx_sad64x64_avx2;
            vpx_sad64x64_avg = vpx_sad64x64_avg_sse2;
            if (flags & HAS_AVX2) vpx_sad64x64_avg = vpx_sad64x64_avg_avx2;
            vpx_sad64x64x4d = vpx_sad64x64x4d_sse2;
            if (flags & HAS_AVX2) vpx_sad64x64x4d = vpx_sad64x64x4d_avx2;
            if (flags & HAS_AVX512) vpx_sad64x64x4d = vpx_sad64x64x4d_avx512;
            vpx_sad8x16x3 = vpx_sad8x16x3_c;
            if (flags & HAS_SSE3) vpx_sad8x16x3 = vpx_sad8x16x3_sse3;
            vpx_sad8x16x8 = vpx_sad8x16x8_c;
            if (flags & HAS_SSE4_1) vpx_sad8x16x8 = vpx_sad8x16x8_sse4_1;
            vpx_sad8x8x3 = vpx_sad8x8x3_c;
            if (flags & HAS_SSE3) vpx_sad8x8x3 = vpx_sad8x8x3_sse3;
            vpx_sad8x8x8 = vpx_sad8x8x8_c;
            if (flags & HAS_SSE4_1) vpx_sad8x8x8 = vpx_sad8x8x8_sse4_1;
            vpx_satd = vpx_satd_sse2;
            if (flags & HAS_AVX2) vpx_satd = vpx_satd_avx2;
            vpx_scaled_2d = vpx_scaled_2d_c;
            if (flags & HAS_SSSE3) vpx_scaled_2d = vpx_scaled_2d_ssse3;
            vpx_sub_pixel_avg_variance16x16 = vpx_sub_pixel_avg_variance16x16_sse2;
            if (flags & HAS_SSSE3) vpx_sub_pixel_avg_variance16x16 = vpx_sub_pixel_avg_variance16x16_ssse3;
            vpx_sub_pixel_avg_variance16x32 = vpx_sub_pixel_avg_variance16x32_sse2;
            if (flags & HAS_SSSE3) vpx_sub_pixel_avg_variance16x32 = vpx_sub_pixel_avg_variance16x32_ssse3;
            vpx_sub_pixel_avg_variance16x8 = vpx_sub_pixel_avg_variance16x8_sse2;
            if (flags & HAS_SSSE3) vpx_sub_pixel_avg_variance16x8 = vpx_sub_pixel_avg_variance16x8_ssse3;
            vpx_sub_pixel_avg_variance32x16 = vpx_sub_pixel_avg_variance32x16_sse2;
            if (flags & HAS_SSSE3) vpx_sub_pixel_avg_variance32x16 = vpx_sub_pixel_avg_variance32x16_ssse3;
            vpx_sub_pixel_avg_variance32x32 = vpx_sub_pixel_avg_variance32x32_sse2;
            if (flags & HAS_SSSE3) vpx_sub_pixel_avg_variance32x32 = vpx_sub_pixel_avg_variance32x32_ssse3;
            if (flags & HAS_AVX2) vpx_sub_pixel_avg_variance32x32 = vpx_sub_pixel_avg_variance32x32_avx2;
            vpx_sub_pixel_avg_variance32x64 = vpx_sub_pixel_avg_variance32x64_sse2;
            if (flags & HAS_SSSE3) vpx_sub_pixel_avg_variance32x64 = vpx_sub_pixel_avg_variance32x64_ssse3;
            vpx_sub_pixel_avg_variance4x4 = vpx_sub_pixel_avg_variance4x4_sse2;
            if (flags & HAS_SSSE3) vpx_sub_pixel_avg_variance4x4 = vpx_sub_pixel_avg_variance4x4_ssse3;
            vpx_sub_pixel_avg_variance4x8 = vpx_sub_pixel_avg_variance4x8_sse2;

)" <<
R"(

            if (flags & HAS_SSSE3) vpx_sub_pixel_avg_variance4x8 = vpx_sub_pixel_avg_variance4x8_ssse3;
            vpx_sub_pixel_avg_variance64x32 = vpx_sub_pixel_avg_variance64x32_sse2;
            if (flags & HAS_SSSE3) vpx_sub_pixel_avg_variance64x32 = vpx_sub_pixel_avg_variance64x32_ssse3;
            vpx_sub_pixel_avg_variance64x64 = vpx_sub_pixel_avg_variance64x64_sse2;
            if (flags & HAS_SSSE3) vpx_sub_pixel_avg_variance64x64 = vpx_sub_pixel_avg_variance64x64_ssse3;
            if (flags & HAS_AVX2) vpx_sub_pixel_avg_variance64x64 = vpx_sub_pixel_avg_variance64x64_avx2;
            vpx_sub_pixel_avg_variance8x16 = vpx_sub_pixel_avg_variance8x16_sse2;
            if (flags & HAS_SSSE3) vpx_sub_pixel_avg_variance8x16 = vpx_sub_pixel_avg_variance8x16_ssse3;
            vpx_sub_pixel_avg_variance8x4 = vpx_sub_pixel_avg_variance8x4_sse2;
            if (flags & HAS_SSSE3) vpx_sub_pixel_avg_variance8x4 = vpx_sub_pixel_avg_variance8x4_ssse3;
            vpx_sub_pixel_avg_variance8x8 = vpx_sub_pixel_avg_variance8x8_sse2;
            if (flags & HAS_SSSE3) vpx_sub_pixel_avg_variance8x8 = vpx_sub_pixel_avg_variance8x8_ssse3;
            vpx_sub_pixel_variance16x16 = vpx_sub_pixel_variance16x16_sse2;
            if (flags & HAS_SSSE3) vpx_sub_pixel_variance16x16 = vpx_sub_pixel_variance16x16_ssse3;
            vpx_sub_pixel_variance16x32 = vpx_sub_pixel_variance16x32_sse2;
            if (flags & HAS_SSSE3) vpx_sub_pixel_variance16x32 = vpx_sub_pixel_variance16x32_ssse3;
            vpx_sub_pixel_variance16x8 = vpx_sub_pixel_variance16x8_sse2;
            if (flags & HAS_SSSE3) vpx_sub_pixel_variance16x8 = vpx_sub_pixel_variance16x8_ssse3;
            vpx_sub_pixel_variance32x16 = vpx_sub_pixel_variance32x16_sse2;
            if (flags & HAS_SSSE3) vpx_sub_pixel_variance32x16 = vpx_sub_pixel_variance32x16_ssse3;
            vpx_sub_pixel_variance32x32 = vpx_sub_pixel_variance32x32_sse2;
            if (flags & HAS_SSSE3) vpx_sub_pixel_variance32x32 = vpx_sub_pixel_variance32x32_ssse3;
            if (flags & HAS_AVX2) vpx_sub_pixel_variance32x32 = vpx_sub_pixel_variance32x32_avx2;
            vpx_sub_pixel_variance32x64 = vpx_sub_pixel_variance32x64_sse2;
            if (flags & HAS_SSSE3) vpx_sub_pixel_variance32x64 = vpx_sub_pixel_variance32x64_ssse3;
            vpx_sub_pixel_variance4x4 = vpx_sub_pixel_variance4x4_sse2;
            if (flags & HAS_SSSE3) vpx_sub_pixel_variance4x4 = vpx_sub_pixel_variance4x4_ssse3;
            vpx_sub_pixel_variance4x8 = vpx_sub_pixel_variance4x8_sse2;
            if (flags & HAS_SSSE3) vpx_sub_pixel_variance4x8 = vpx_sub_pixel_variance4x8_ssse3;
            vpx_sub_pixel_variance64x32 = vpx_sub_pixel_variance64x32_sse2;
            if (flags & HAS_SSSE3) vpx_sub_pixel_variance64x32 = vpx_sub_pixel_variance64x32_ssse3;
            vpx_sub_pixel_variance64x64 = vpx_sub_pixel_variance64x64_sse2;
            if (flags & HAS_SSSE3) vpx_sub_pixel_variance64x64 = vpx_sub_pixel_variance64x64_ssse3;
            if (flags & HAS_AVX2) vpx_sub_pixel_variance64x64 = vpx_sub_pixel_variance64x64_avx2;
            vpx_sub_pixel_variance8x16 = vpx_sub_pixel_variance8x16_sse2;
            if (flags & HAS_SSSE3) vpx_sub_pixel_variance8x16 = vpx_sub_pixel_variance8x16_ssse3;
            vpx_sub_pixel_variance8x4 = vpx_sub_pixel_variance8x4_sse2;
            if (flags & HAS_SSSE3) vpx_sub_pixel_variance8x4 = vpx_sub_pixel_variance8x4_ssse3;
            vpx_sub_pixel_variance8x8 = vpx_sub_pixel_variance8x8_sse2;
            if (flags & HAS_SSSE3) vpx_sub_pixel_variance8x8 = vpx_sub_pixel_variance8x8_ssse3;
            vpx_variance16x16 = vpx_variance16x16_sse2;
            if (flags & HAS_AVX2) vpx_variance16x16 = vpx_variance16x16_avx2;
            vpx_variance16x32 = vpx_variance16x32_sse2;
            //if (flags & HAS_AVX2) vpx_variance16x32 = vpx_variance16x32_avx2;
            vpx_variance16x8 = vpx_variance16x8_sse2;
            //if (flags & HAS_AVX2) vpx_variance16x8 = vpx_variance16x8_avx2;
            vpx_variance32x16 = vpx_variance32x16_sse2;
            if (flags & HAS_AVX2) vpx_variance32x16 = vpx_variance32x16_avx2;
            vpx_variance32x32 = vpx_variance32x32_sse2;
            if (flags & HAS_AVX2) vpx_variance32x32 = vpx_variance32x32_avx2;
            vpx_variance32x64 = vpx_variance32x64_sse2;
            //if (flags & HAS_AVX2) vpx_variance32x64 = vpx_variance32x64_avx2;
            vpx_variance64x32 = vpx_variance64x32_sse2;
            if (flags & HAS_AVX2) vpx_variance64x32 = vpx_variance64x32_avx2;
            vpx_variance64x64 = vpx_variance64x64_sse2;
            if (flags & HAS_AVX2) vpx_variance64x64 = vpx_variance64x64_avx2;
        }
        #endif

        #ifdef __cplusplus
        }  // extern "C"
        #endif

        #endif
    )";

        vpx.writeFileOnce("vpx_dsp_rtcd.h", oss.str());
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("vpx");
    s.checkIncludeExists("pthread.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("sys/mman.h");
    s.checkIncludeExists("unistd.h");
    s.checkTypeSize("size_t");
    s.checkTypeSize("void *");
}
