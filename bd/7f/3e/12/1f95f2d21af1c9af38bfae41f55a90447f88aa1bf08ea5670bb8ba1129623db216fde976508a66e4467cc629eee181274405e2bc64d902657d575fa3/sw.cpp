void build(Solution &s)
{
    auto &ffmpeg = s.addProject("ffmpeg", "7.0.1");
    ffmpeg += Git("https://github.com/FFmpeg/FFmpeg", "n{M}.{m}{po}");

    auto setup = [&s](auto &t, const path &d)
    {
        t.PackageDefinitions = true;
        t.ExportAllSymbols = true;
        //t.Public += IncludeDirectory(t.SourceDir);

        SwapAndRestore sr(t.SourceDir, t.SourceDir / d);
        t.AllowEmptyRegexes = true;
        t -=
            "aarch64/.*"_rr,
            "alpha/.*"_rr,
            "arm/.*"_rr,
            "avr32/.*"_rr,
            "bfin/.*"_rr,
            "loongarch/.*"_rr,
            "neon/.*"_rr,
            "mips/.*"_rr,
            "ppc/.*"_rr,
            "sh4/.*"_rr,
            "sparc/.*"_rr,
            "tomi/.*"_rr,
            "tests/.*"_rr,
            "v4l2_.*"_rr,
            ".*vulkan.*"_rr
            ;

        t -= ".*android.*"_rr;
        t -= ".*_template.*"_rr;
        t -= "x86/w64xmmtest.c"_rr;

        if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            t -= ".*qsv.*"_rr;
            t -= ".*vaapi.*"_rr;
            t -= ".*vdpau.*"_rr;
            t -= ".*videotoolbox.*"_rr;
        }
        t.AllowEmptyRegexes = false;

        t.Public += sw::Shared, "CONFIG_SHARED"_def;
        t += Definition("BUILDING_" + t.getPackage().getPath().back());

        //s.Settings.Native.ConfigurationType = ConfigurationType::Release;

        t.writeFileOnce("config_components.h");
    };

    // fwd decls
    auto &avcodec = ffmpeg.addTarget<LibraryTarget>("avcodec");
    auto &avfilter = ffmpeg.addTarget<LibraryTarget>("avfilter");

    auto setup_asm = [](auto &t) {
        if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            if (t.getBuildSettings().TargetOS.Arch == ArchType::x86_64)
            {
                //for (auto &[p, f] : avutil["libavutil/.*\\.asm"_rr])
                    //f->args.push_back("-fwin64");
            }

            t += "HAVE_CPUNOP=1"_def;
            t += "HAVE_FMA3_EXTERNAL=1"_def;
            t += "HAVE_FMA4_EXTERNAL=0"_def;
            t += "HAVE_AVX_EXTERNAL=1"_def;
            t += "HAVE_AVX2_EXTERNAL=1"_def;
            t += "HAVE_XOP_EXTERNAL=0"_def;
            t += "HAVE_MMX_INLINE=0"_def;
            t += "HAVE_MMX_EXTERNAL=1"_def;
            t += "HAVE_MMXEXT_EXTERNAL=1"_def;
            t += "HAVE_SSE2_EXTERNAL=1"_def;
            t += "HAVE_SSE3_EXTERNAL=1"_def;
            t += "HAVE_SSSE3_EXTERNAL=1"_def;
        }
    };

    auto &avutil = ffmpeg.addTarget<LibraryTarget>("avutil");
    {
        avutil.setChecks("avutil");

        avutil.setExtensionProgram(".asm", "org.sw.demo.yasm"_dep);

        avutil +=
            "compat/.*\\.h"_rr,
            "libavutil/.*\\.S"_rr,
            "libavutil/.*\\.c"_rr,
            "libavutil/.*\\.h"_rr;
        avutil +=
            "libavutil/.*\\.asm"_rr;
        setup(avutil, "libavutil");

        avutil.Public += "."_idir;

        if (avutil.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            setup_asm(avutil);

            avutil -=
                "libavutil/hwcontext_vdpau.c",
                "libavutil/hwcontext_qsv.c",
                "libavutil/hwcontext_cuda.c",
                "libavutil/hwcontext_vaapi.c",
                "libavutil/hwcontext_drm.c",
                "libavutil/hwcontext_videotoolbox.c",
                "libavutil/hwcontext_opencl.c",
                "libavutil/macos_kperf.c"
                ;
            avutil.Protected += "compat/atomics/win32"_idir;
            avutil += "User32.lib"_slib;
            avutil.Public += "HAVE_STRUCT_ADDRINFO"_def;
        }

        avutil.Public += "ARCH_AARCH64=0"_d;
        avutil.Public += "ARCH_ALPHA=0"_d;
        avutil.Public += "ARCH_ARM=0"_d;
        avutil.Public += "ARCH_MIPS=0"_d;
        avutil.Public += "ARCH_PPC=0"_d;
        avutil.Public += "ARCH_X86=1"_d;
        avutil.Public += "CONFIG_BZLIB=1"_d;
        avutil.Public += "CONFIG_FTRAPV=0"_d;
        avutil.Public += "CONFIG_LIBMP3LAME=0"_d;
        avutil.Public += "CONFIG_LIBVORBIS=1"_d;
        avutil.Public += "CONFIG_MEMORY_POISONING=0"_d;
        avutil.Public += "CONFIG_OPENSSL=1"_d;
        avutil.Public += "CONFIG_SWSCALE_ALPHA=1"_d;
        avutil.Public += "CONFIG_ZLIB=1"_d;
        avutil.Public += "FFMPEG_CONFIGURATION=\"\""_d;
        avutil.Public += "FFMPEG_LICENSE=\"\""_d;
        avutil.Public += "FFMPEG_VERSION=PACKAGE_VERSION"_d;
        avutil.Public += "HAVE_ALIGNED_STACK=1"_d;
        avutil.Public += "HAVE_ALTIVEC=0"_d;
        avutil.Public += "HAVE_ARMV8=0"_d;
        avutil.Public += "HAVE_AV_CONFIG_H"_d;
        avutil.Public += "HAVE_BIGENDIAN=0"_d;
        avutil.Public += "HAVE_FAST_UNALIGNED=1"_d;
        avutil.Public += "HAVE_INTRINSICS_NEON=0"_d;
        avutil.Public += "HAVE_MIPSDSP=0"_d;
        avutil.Public += "HAVE_MIPSFPU=0"_d;
        avutil.Public += "HAVE_MMX2=1"_d;
        avutil.Public += "HAVE_MMX=1"_d;
        avutil.Public += "HAVE_MMXEXT=1"_d;
        avutil.Public += "HAVE_NEON=0"_d;
        avutil.Public += "HAVE_OPENJPEG_2_1_OPENJPEG_H=1"_d;
        avutil.Public += "HAVE_POWER8=0"_d;
        avutil.Public += "HAVE_THREADS=1"_d;
        avutil.Public += "HAVE_VFP=0"_d;
        avutil.Public += "HAVE_VFPV3=0"_d;
        avutil.Public += "HAVE_VSX=0"_d;
        avutil.Public += "HAVE_X86ASM=0"_d;
        avutil.Public += "HAVE_YASM=0"_d;
        avutil.Public += "av_restrict="_d;
        if (avutil.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            avutil.Public += "HAVE_ATOMICS_NATIVE"_d;
            avutil.Public += "HAVE_ATOMICS_WIN32"_d;
            avutil.Public += "HAVE_DXVA_H=1"_d;
            avutil.Public += "HAVE_OS2THREADS=0"_d;
            avutil.Public += "HAVE_PTHREADS=0"_d;
            avutil.Public += "HAVE_W32THREADS=1"_d;
        }
        avutil.Public += sw::Shared, "CONFIG_SHARED"_d;

        avutil.Public += "HAVE_ISNAN"_d;
        avutil.Public += "HAVE_LRINT"_d;
        avutil.Public += "HAVE_LRINTF"_d;

        avutil.writeFileOnce("libavutil/avconfig.h");
        avutil.writeFileOnce("libavutil/ffversion.h");

        std::ostringstream oss;
        {
            oss <<
                R"(
        #define CONFIG_CBS_MPEG2 1
        #define CONFIG_H264_NVDEC_HWACCEL 0
        #define CONFIG_HEVC_NVDEC_HWACCEL 0

        #define CONFIG_H264_D3D11VA2_HWACCEL 1
        #define CONFIG_HEVC_D3D11VA2_HWACCEL 1
        #define CONFIG_HEVC_VIDEOTOOLBOX_HWACCEL 0
        #define CONFIG_MPEG2_D3D11VA2_HWACCEL 1
        #define CONFIG_MPEG2_MEDIACODEC_HWACCEL 0
        #define CONFIG_VC1_D3D11VA2_HWACCEL 1
        #define CONFIG_VP8_QSV_HWACCEL 0
        #define CONFIG_VP9_D3D11VA2_HWACCEL 1
        #define CONFIG_WMV3_D3D11VA2_HWACCEL 1
        #define CONFIG_CLEARVIDEO_DECODER 1
        #define CONFIG_FITS_ENCODER 1
        #define CONFIG_FITS_DECODER 1
        #define CONFIG_FMVC_DECODER 1
        #define CONFIG_GDV_DECODER 1
        #define CONFIG_H263_V4L2M2M_DECODER 0
        #define CONFIG_H264_V4L2M2M_DECODER 0
        #define CONFIG_H264_RKMPP_DECODER 0
        #define CONFIG_HEVC_RKMPP_DECODER 0
        #define CONFIG_HEVC_V4L2M2M_DECODER 0
        #define CONFIG_MPEG4_V4L2M2M_DECODER 0
        #define CONFIG_MPEG1_V4L2M2M_DECODER 0
        #define CONFIG_MPEG2_V4L2M2M_DECODER 0
        #define CONFIG_MPEG2_MEDIACODEC_DECODER 0
        #define CONFIG_MSCC_DECODER 1
        #define CONFIG_PIXLET_DECODER 1
        #define CONFIG_PSD_DECODER 1
        #define CONFIG_SCPR_DECODER 1
        #define CONFIG_SPEEDHQ_DECODER 1
        #define CONFIG_SRGC_DECODER 1
        #define CONFIG_VC1_V4L2M2M_DECODER 0
        #define CONFIG_VP8_RKMPP_DECODER 0
        #define CONFIG_VP8_V4L2M2M_DECODER 0
        #define CONFIG_VP9_RKMPP_DECODER 0
        #define CONFIG_VP9_V4L2M2M_DECODER 0
        #define CONFIG_BITPACKED_DECODER 1
        #define CONFIG_WRAPPED_AVFRAME_DECODER 1
        #define CONFIG_XPM_DECODER 1
        #define CONFIG_ATRAC3AL_DECODER 1
        #define CONFIG_ATRAC3PAL_DECODER 1
        #define CONFIG_DOLBY_E_DECODER 1
        #define CONFIG_OPUS_ENCODER 1
        #define CONFIG_QDMC_DECODER 1
        #define CONFIG_GREMLIN_DPCM_DECODER 1
        #define CONFIG_ADPCM_G726LE_ENCODER 1
        #define CONFIG_LIBRSVG_DECODER 0
        #define CONFIG_H263_V4L2M2M_ENCODER 0
        #define CONFIG_H264_V4L2M2M_ENCODER 0
        #define CONFIG_HEVC_V4L2M2M_ENCODER 0
        #define CONFIG_MPEG2_VAAPI_ENCODER 0
        #define CONFIG_MPEG4_V4L2M2M_ENCODER 0
        #define CONFIG_VP8_QSV_DECODER 0
        #define CONFIG_VP8_V4L2M2M_ENCODER 0
        #define CONFIG_VP8_VAAPI_ENCODER 0
        #define CONFIG_VP9_VAAPI_ENCODER 0
        #define CONFIG_SIPR_PARSER 1
        #define CONFIG_XMA_PARSER 1

        #define CONFIG_DASH_DEMUXER 1
        #define CONFIG_FITS_MUXER 1
        #define CONFIG_FITS_DEMUXER 1
        #define CONFIG_G726_MUXER 1
        #define CONFIG_G726_DEMUXER 1
        #define CONFIG_G726LE_MUXER 1
        #define CONFIG_G726LE_DEMUXER 1
        #define CONFIG_GDV_DEMUXER 1
        #define CONFIG_MJPEG_2000_DEMUXER 1
        #define CONFIG_S337M_DEMUXER 1
        #define CONFIG_SCC_MUXER 1
        #define CONFIG_SCC_DEMUXER 1
        #define CONFIG_SDS_DEMUXER 1
        #define CONFIG_SDX_DEMUXER 1
        #define CONFIG_SUP_MUXER 1
        #define CONFIG_IMAGE_PSD_PIPE_DEMUXER 1
        #define CONFIG_IMAGE_SVG_PIPE_DEMUXER 1
        #define CONFIG_IMAGE_XPM_PIPE_DEMUXER 1
        #define HAVE_LIBC_MSVCRT 1


        #define CONFIG_ACOPY_FILTER 1
        #define CONFIG_AFIR_FILTER 1
        #define CONFIG_CROSSFEED_FILTER 1
        #define CONFIG_HAAS_FILTER 1
        #define CONFIG_HEADPHONE_FILTER 1
        #define CONFIG_SUPEREQUALIZER_FILTER 1
        #define CONFIG_SURROUND_FILTER 1
        #define CONFIG_CONVOLVE_FILTER 1
        #define CONFIG_DEFLICKER_FILTER 1
        #define CONFIG_DEINTERLACE_QSV_FILTER 0
        #define CONFIG_DEINTERLACE_VAAPI_FILTER 0
        #define CONFIG_DESPILL_FILTER 1
        #define CONFIG_DOUBLEWEAVE_FILTER 1
        #define CONFIG_FLOODFILL_FILTER 1
        #define CONFIG_HWMAP_FILTER 1
        #define CONFIG_LIBVMAF_FILTER 0
        #define CONFIG_LIMITER_FILTER 1
        #define CONFIG_LUMAKEY_FILTER 1
        #define CONFIG_MIDEQUALIZER_FILTER 1
        #define CONFIG_OSCILLOSCOPE_FILTER 1
        #define CONFIG_PIXSCOPE_FILTER 1
        #define CONFIG_PREMULTIPLY_FILTER 1
        #define CONFIG_PSEUDOCOLOR_FILTER 1
        #define CONFIG_READEIA608_FILTER 1
        #define CONFIG_ROBERTS_FILTER 1
        #define CONFIG_SCALE_CUDA_FILTER 0
        #define CONFIG_SCALE_QSV_FILTER 0
        #define CONFIG_SIGNATURE_FILTER 1
        #define CONFIG_THRESHOLD_FILTER 1
        #define CONFIG_THUMBNAIL_CUDA_FILTER 0
        #define CONFIG_TLUT2_FILTER 1
        #define CONFIG_TONEMAP_FILTER 1
        #define CONFIG_UNPREMULTIPLY_FILTER 1
        #define CONFIG_VMAFMOTION_FILTER 1
        #define CONFIG_ABITSCOPE_FILTER 1


        #define CONFIG_LIBNDI_NEWTEK_OUTDEV 0
        #define CONFIG_LIBNDI_NEWTEK_INDEV 0
        #define CONFIG_KMSGRAB_INDEV 0
        #define CONFIG_XCBGRAB_INDEV 0




        #define HAVE_FAST_64BIT 1
        #define HAVE_FAST_CLZ 1
        #define HAVE_FAST_CMOV 1

        #define HAVE_EBP_AVAILABLE 0
        #define HAVE_EBX_AVAILABLE 0

        #define HAVE_ARMV5TE_EXTERNAL 0
        #define HAVE_ARMV6_EXTERNAL 0
        #define HAVE_ARMV6T2_EXTERNAL 0
        #define HAVE_ARMV8_EXTERNAL 0
        #define HAVE_NEON_EXTERNAL 0
        #define HAVE_VFP_EXTERNAL 0
        #define HAVE_VFPV3_EXTERNAL 0
        #define HAVE_SETEND_EXTERNAL 0
        #define HAVE_ALTIVEC_EXTERNAL 0
        #define HAVE_DCBZL_EXTERNAL 0
        #define HAVE_LDBRX_EXTERNAL 0
        #define HAVE_POWER8_EXTERNAL 0
        #define HAVE_PPC4XX_EXTERNAL 0
        #define HAVE_VSX_EXTERNAL 0
        #define HAVE_AESNI_EXTERNAL 0
        #define HAVE_AMD3DNOW_EXTERNAL 0
        #define HAVE_AMD3DNOWEXT_EXTERNAL 0
        #define HAVE_AVX_EXTERNAL 0
        #define HAVE_AVX2_EXTERNAL 0
        #define HAVE_FMA3_EXTERNAL 0
        #define HAVE_FMA4_EXTERNAL 0
        #define HAVE_MMX_EXTERNAL 0
        #define HAVE_MMXEXT_EXTERNAL 0
        #define HAVE_SSE_EXTERNAL 0
        #define HAVE_SSE2_EXTERNAL 0
        #define HAVE_SSE3_EXTERNAL 0
        #define HAVE_SSE4_EXTERNAL 0
        #define HAVE_SSE42_EXTERNAL 0
        #define HAVE_SSSE3_EXTERNAL 0
        #define HAVE_XOP_EXTERNAL 0
        #define HAVE_CPUNOP_EXTERNAL 0
        #define HAVE_I686_EXTERNAL 0
        #define HAVE_MIPSFPU_EXTERNAL 0
        #define HAVE_MIPS32R2_EXTERNAL 0
        #define HAVE_MIPS32R5_EXTERNAL 0
        #define HAVE_MIPS64R2_EXTERNAL 0
        #define HAVE_MIPS32R6_EXTERNAL 0
        #define HAVE_MIPS64R6_EXTERNAL 0
        #define HAVE_MIPSDSP_EXTERNAL 0
        #define HAVE_MIPSDSPR2_EXTERNAL 0
        #define HAVE_MSA_EXTERNAL 0
        #define HAVE_LOONGSON2_EXTERNAL 0
        #define HAVE_LOONGSON3_EXTERNAL 0
        #define HAVE_MMI_EXTERNAL 0
        #define HAVE_ARMV5TE_INLINE 0
        #define HAVE_ARMV6_INLINE 0
        #define HAVE_ARMV6T2_INLINE 0
        #define HAVE_ARMV8_INLINE 0
        #define HAVE_NEON_INLINE 0
        #define HAVE_VFP_INLINE 0
        #define HAVE_VFPV3_INLINE 0
        #define HAVE_SETEND_INLINE 0
        #define HAVE_ALTIVEC_INLINE 0
        #define HAVE_DCBZL_INLINE 0
        #define HAVE_LDBRX_INLINE 0
        #define HAVE_POWER8_INLINE 0
        #define HAVE_PPC4XX_INLINE 0
        #define HAVE_VSX_INLINE 0
        #define HAVE_AESNI_INLINE 0
        #define HAVE_AMD3DNOW_INLINE 0
        #define HAVE_AMD3DNOWEXT_INLINE 0
        #define HAVE_AVX_INLINE 0
        #define HAVE_AVX2_INLINE 0
        #define HAVE_FMA3_INLINE 0
        #define HAVE_FMA4_INLINE 0
        #define HAVE_MMX_INLINE 0
        #define HAVE_MMXEXT_INLINE 0
        #define HAVE_SSE_INLINE 0
        #define HAVE_SSE2_INLINE 0
        #define HAVE_SSE3_INLINE 0
        #define HAVE_SSE4_INLINE 0
        #define HAVE_SSE42_INLINE 0
        #define HAVE_SSSE3_INLINE 0
        #define HAVE_XOP_INLINE 0
        #define HAVE_CPUNOP_INLINE 0
        #define HAVE_I686_INLINE 0
        #define HAVE_MIPSFPU_INLINE 0
        #define HAVE_MIPS32R2_INLINE 0
        #define HAVE_MIPS32R5_INLINE 0
        #define HAVE_MIPS64R2_INLINE 0
        #define HAVE_MIPS32R6_INLINE 0
        #define HAVE_MIPS64R6_INLINE 0
        #define HAVE_MIPSDSP_INLINE 0
        #define HAVE_MIPSDSPR2_INLINE 0
        #define HAVE_MSA_INLINE 0
        #define HAVE_LOONGSON2_INLINE 0
        #define HAVE_LOONGSON3_INLINE 0
        #define HAVE_MMI_INLINE 0

        #define CONFIG_BSFS 1

        #define CONFIG_DECODERS 1
        #define CONFIG_ENCODERS 1
        #define CONFIG_HWACCELS 1
        #define CONFIG_PARSERS 1
        #define CONFIG_INDEVS 1
        #define CONFIG_OUTDEVS 1
        #define CONFIG_FILTERS 1
        #define CONFIG_DEMUXERS 1
        #define CONFIG_MUXERS 1
        #define CONFIG_PROTOCOLS 1
        #define CONFIG_DOC 1
        #define CONFIG_HTMLPAGES 1
        #define CONFIG_MANPAGES 1
        #define CONFIG_PODPAGES 1
        #define CONFIG_TXTPAGES 1

        #define CONFIG_AVIO_DIR_CMD_EXAMPLE 1
        #define CONFIG_AVIO_READING_EXAMPLE 1
        #define CONFIG_DECODING_ENCODING_EXAMPLE 1
        #define CONFIG_DEMUXING_DECODING_EXAMPLE 1
        #define CONFIG_EXTRACT_MVS_EXAMPLE 1
        #define CONFIG_FILTER_AUDIO_EXAMPLE 1
        #define CONFIG_FILTERING_AUDIO_EXAMPLE 1
        #define CONFIG_FILTERING_VIDEO_EXAMPLE 1
        #define CONFIG_HTTP_MULTICLIENT_EXAMPLE 1
        #define CONFIG_METADATA_EXAMPLE 1
        #define CONFIG_MUXING_EXAMPLE 1
        #define CONFIG_QSVDEC_EXAMPLE 0
        #define CONFIG_REMUXING_EXAMPLE 1
        #define CONFIG_RESAMPLING_AUDIO_EXAMPLE 1
        #define CONFIG_SCALING_VIDEO_EXAMPLE 1
        #define CONFIG_TRANSCODE_AAC_EXAMPLE 1
        #define CONFIG_TRANSCODING_EXAMPLE 1
        #define CONFIG_AVISYNTH 0
        #define CONFIG_BZLIB 1
        #define CONFIG_CHROMAPRINT 0
        #define CONFIG_CRYSTALHD 0
        #define CONFIG_DECKLINK 0
        #define CONFIG_FREI0R 0
        #define CONFIG_GCRYPT 0
        #define CONFIG_GMP 0
        #define CONFIG_GNUTLS 0
        #define CONFIG_ICONV 1
        #define CONFIG_JNI 0
        #define CONFIG_LADSPA 0
        #define CONFIG_LIBASS 0
        #define CONFIG_LIBBLURAY 0
        #define CONFIG_LIBBS2B 0
        #define CONFIG_LIBCACA 0
        #define CONFIG_LIBCDIO 0
        #define CONFIG_LIBCELT 0
        #define CONFIG_LIBDC1394 0
        #define CONFIG_LIBEBUR128 0
        #define CONFIG_LIBFDK_AAC 0
        #define CONFIG_LIBFLITE 0
        #define CONFIG_LIBFONTCONFIG 0
        #define CONFIG_LIBFREETYPE 0
        #define CONFIG_LIBFRIBIDI 0
        #define CONFIG_LIBGME 0
        #define CONFIG_LIBGSM 0
        #define CONFIG_LIBIEC61883 0
        #define CONFIG_LIBILBC 0
        #define CONFIG_LIBKVAZAAR 0
        #define CONFIG_LIBMODPLUG 0
        #define CONFIG_LIBMP3LAME 0
        #define CONFIG_LIBNUT 0
        #define CONFIG_LIBOPENCORE_AMRNB 0
        #define CONFIG_LIBOPENCORE_AMRWB 0
        #define CONFIG_LIBOPENCV 0
        #define CONFIG_LIBOPENH264 0
        #define CONFIG_LIBOPENJPEG 0
        #define CONFIG_LIBOPENMPT 0
        #define CONFIG_LIBOPUS 0
        #define CONFIG_LIBPULSE 0
        #define CONFIG_LIBRTMP 0
        #define CONFIG_LIBRUBBERBAND 0
        #define CONFIG_LIBSCHROEDINGER 0
        #define CONFIG_LIBSHINE 0
        #define CONFIG_LIBSMBCLIENT 0
        #define CONFIG_LIBSNAPPY 0
        #define CONFIG_LIBSOXR 0
        #define CONFIG_LIBSPEEX 0
        #define CONFIG_LIBSSH 0
        #define CONFIG_LIBTESSERACT 0
        #define CONFIG_LIBTHEORA 0
        #define CONFIG_LIBTWOLAME 0
        #define CONFIG_LIBV4L2 0
        #define CONFIG_LIBVIDSTAB 0
        #define CONFIG_LIBVO_AMRWBENC 0
        #define CONFIG_LIBVORBIS 0
        #define CONFIG_LIBVPX 0
        #define CONFIG_LIBWAVPACK 0
        #define CONFIG_LIBWEBP 0
        #define CONFIG_LIBX264 0
        #define CONFIG_LIBX265 0
        #define CONFIG_LIBXAVS 0
        #define CONFIG_LIBXCB 1
        #define CONFIG_LIBXCB_SHM 1
        #define CONFIG_LIBXCB_SHAPE 0
        #define CONFIG_LIBXCB_XFIXES 0
        #define CONFIG_LIBXVID 0
        #define CONFIG_LIBZIMG 0
        #define CONFIG_LIBZMQ 0
        #define CONFIG_LIBZVBI 0
        #define CONFIG_LZMA 1
        #define CONFIG_MEDIACODEC 0
        #define CONFIG_NETCDF 0
        #define CONFIG_OPENAL 0
        #define CONFIG_OPENCL 0
        #define CONFIG_OPENGL 0
        #define CONFIG_SCHANNEL 1
        #define CONFIG_SDL 0
        #define CONFIG_SDL2 0
        #define CONFIG_SECURETRANSPORT 0
        #define CONFIG_VIDEOTOOLBOX 0
        #define CONFIG_X11GRAB 0
        #define CONFIG_XLIB 1
        #define CONFIG_ZLIB 1
        #define CONFIG_AUDIOTOOLBOX 0
        #define CONFIG_CUDA 0
        #define CONFIG_CUVID 0
        #define CONFIG_D3D11VA 0
        #define CONFIG_DXVA2 1
        #define CONFIG_LIBMFX 0
        #define CONFIG_LIBNPP 0
        #define CONFIG_MMAL 0
        #define CONFIG_NVENC 1
        #define CONFIG_OMX 0
        #define CONFIG_VAAPI 0
        #define CONFIG_VDA 0
        #define CONFIG_VDPAU 0
        #define CONFIG_VIDEOTOOLBOX_HWACCEL 0
        #define CONFIG_XVMC 0
        #define CONFIG_FTRAPV 0
        #define CONFIG_GRAY 0
        #define CONFIG_HARDCODED_TABLES 0
        #define CONFIG_OMX_RPI 0
        #define CONFIG_RUNTIME_CPUDETECT 1
        #define CONFIG_SAFE_BITSTREAM_READER 1
        #define CONFIG_SMALL 0
        #define CONFIG_STATIC 1
        #define CONFIG_SWSCALE_ALPHA 1
        #define CONFIG_GPL 0
        #define CONFIG_NONFREE 0
        #define CONFIG_VERSION3 0
        #define CONFIG_AVCODEC 1
        #define CONFIG_AVDEVICE 1
        #define CONFIG_AVFILTER 1
        #define CONFIG_AVFORMAT 1
        #define CONFIG_AVRESAMPLE 0
        #define CONFIG_AVUTIL 1
        #define CONFIG_POSTPROC 0
        #define CONFIG_SWRESAMPLE 1
        #define CONFIG_SWSCALE 1
        #define CONFIG_FFPLAY 0
        #define CONFIG_FFPROBE 1
        #define CONFIG_FFSERVER 1
        #define CONFIG_FFMPEG 1
        #define CONFIG_DCT 1
        #define CONFIG_DWT 1
        #define CONFIG_ERROR_RESILIENCE 1
        #define CONFIG_FAAN 1
        #define CONFIG_FAST_UNALIGNED 1
        #define CONFIG_FFT 1
        #define CONFIG_LSP 1
        #define CONFIG_LZO 1
        #define CONFIG_MDCT 1
        #define CONFIG_PIXELUTILS 1
        #define CONFIG_NETWORK 1
        #define CONFIG_RDFT 1
        #define CONFIG_FONTCONFIG 0
        #define CONFIG_MEMALIGN_HACK 0
        #define CONFIG_MEMORY_POISONING 0
        #define CONFIG_NEON_CLOBBER_TEST 0
        #define CONFIG_PIC 1
        #define CONFIG_POD2MAN 1
        #define CONFIG_RAISE_MAJOR 0
        #define CONFIG_THUMB 0
        #define CONFIG_VALGRIND_BACKTRACE 0
        #define CONFIG_XMM_CLOBBER_TEST 0
        #define CONFIG_AANDCTTABLES 1
        #define CONFIG_AC3DSP 1
        #define CONFIG_AUDIO_FRAME_QUEUE 1
        #define CONFIG_AUDIODSP 1
        #define CONFIG_BLOCKDSP 1
        #define CONFIG_BSWAPDSP 1
        #define CONFIG_CABAC 1
        #define CONFIG_DIRAC_PARSE 1
        #define CONFIG_DVPROFILE 1
        #define CONFIG_EXIF 1
        #define CONFIG_FAANDCT 1
        #define CONFIG_FAANIDCT 1
        #define CONFIG_FDCTDSP 1
        #define CONFIG_FLACDSP 1

)" <<
R"(

        #define CONFIG_FMTCONVERT 1
        #define CONFIG_FRAME_THREAD_ENCODER 1
        #define CONFIG_G722DSP 1
        #define CONFIG_GOLOMB 1
        #define CONFIG_GPLV3 0
        #define CONFIG_H263DSP 1
        #define CONFIG_H264CHROMA 1
        #define CONFIG_H264DSP 1
        #define CONFIG_H264PRED 1
        #define CONFIG_H264QPEL 1
        #define CONFIG_HPELDSP 1
        #define CONFIG_HUFFMAN 1
        #define CONFIG_HUFFYUVDSP 1
        #define CONFIG_HUFFYUVENCDSP 1
        #define CONFIG_IDCTDSP 1
        #define CONFIG_IIRFILTER 1
        #define CONFIG_IMDCT15 1
        #define CONFIG_INTRAX8 1
        #define CONFIG_ISO_MEDIA 1
        #define CONFIG_IVIDSP 1
        #define CONFIG_JPEGTABLES 1
        #define CONFIG_LGPLV3 0
        #define CONFIG_LIBX262 0
        #define CONFIG_LLAUDDSP 1
        #define CONFIG_LLVIDDSP 1
        #define CONFIG_LPC 1
        #define CONFIG_LZF 1
        #define CONFIG_ME_CMP 1
        #define CONFIG_MPEG_ER 1
        #define CONFIG_MPEGAUDIO 1
        #define CONFIG_MPEGAUDIODSP 1
        #define CONFIG_MPEGVIDEO 1
        #define CONFIG_MPEGVIDEOENC 1
        #define CONFIG_MSS34DSP 1
        #define CONFIG_PIXBLOCKDSP 1
        #define CONFIG_QPELDSP 1
        #define CONFIG_QSV 0
        #define CONFIG_QSVDEC 0
        #define CONFIG_QSVENC 0
        #define CONFIG_RANGECODER 1
        #define CONFIG_RIFFDEC 1
        #define CONFIG_RIFFENC 1
        #define CONFIG_RTPDEC 1
        #define CONFIG_RTPENC_CHAIN 1
        #define CONFIG_RV34DSP 1
        #define CONFIG_SINEWIN 1
        #define CONFIG_SNAPPY 1
        #define CONFIG_SRTP 1
        #define CONFIG_STARTCODE 1
        #define CONFIG_TEXTUREDSP 1
        #define CONFIG_TEXTUREDSPENC 0
        #define CONFIG_TPELDSP 1
        #define CONFIG_VAAPI_ENCODE 0
        #define CONFIG_VC1DSP 1
        #define CONFIG_VIDEODSP 1
        #define CONFIG_VP3DSP 1
        #define CONFIG_VP56DSP 1
        #define CONFIG_VP8DSP 1
        #define CONFIG_VT_BT2020 0
        #define CONFIG_WMA_FREQS 1
        #define CONFIG_WMV2DSP 1
        #define CONFIG_AAC_ADTSTOASC_BSF 1
        #define CONFIG_CHOMP_BSF 1
        #define CONFIG_DUMP_EXTRADATA_BSF 1
        #define CONFIG_DCA_CORE_BSF 1
        #define CONFIG_H264_MP4TOANNEXB_BSF 1
        #define CONFIG_HEVC_MP4TOANNEXB_BSF 1
        #define CONFIG_IMX_DUMP_HEADER_BSF 1
        #define CONFIG_MJPEG2JPEG_BSF 1
        #define CONFIG_MJPEGA_DUMP_HEADER_BSF 1
        #define CONFIG_MP3_HEADER_DECOMPRESS_BSF 1
        #define CONFIG_MPEG4_UNPACK_BFRAMES_BSF 1
        #define CONFIG_MOV2TEXTSUB_BSF 1
        #define CONFIG_NOISE_BSF 1
        #define CONFIG_REMOVE_EXTRADATA_BSF 1
        #define CONFIG_TEXT2MOVSUB_BSF 1
        #define CONFIG_VP9_SUPERFRAME_BSF 1
        #define CONFIG_AASC_DECODER 1
        #define CONFIG_AIC_DECODER 1
        #define CONFIG_ALIAS_PIX_DECODER 1
        #define CONFIG_AMV_DECODER 1
        #define CONFIG_ANM_DECODER 1
        #define CONFIG_ANSI_DECODER 1
        #define CONFIG_APNG_DECODER 1
        #define CONFIG_ASV1_DECODER 1
        #define CONFIG_ASV2_DECODER 1
        #define CONFIG_AURA_DECODER 1
        #define CONFIG_AURA2_DECODER 1
        #define CONFIG_AVRP_DECODER 1
        #define CONFIG_AVRN_DECODER 1
        #define CONFIG_AVS_DECODER 1
        #define CONFIG_AVUI_DECODER 1
        #define CONFIG_AYUV_DECODER 1
        #define CONFIG_BETHSOFTVID_DECODER 1
        #define CONFIG_BFI_DECODER 1
        #define CONFIG_BINK_DECODER 1
        #define CONFIG_BMP_DECODER 1
        #define CONFIG_BMV_VIDEO_DECODER 1
        #define CONFIG_BRENDER_PIX_DECODER 1
        #define CONFIG_C93_DECODER 1
        #define CONFIG_CAVS_DECODER 1
        #define CONFIG_CDGRAPHICS_DECODER 1
        #define CONFIG_CDXL_DECODER 1
        #define CONFIG_CFHD_DECODER 1
        #define CONFIG_CINEPAK_DECODER 1
        #define CONFIG_CLJR_DECODER 1
        #define CONFIG_CLLC_DECODER 1
        #define CONFIG_COMFORTNOISE_DECODER 1
        #define CONFIG_CPIA_DECODER 1
        #define CONFIG_CSCD_DECODER 1
        #define CONFIG_CYUV_DECODER 1
        #define CONFIG_DDS_DECODER 1
        #define CONFIG_DFA_DECODER 1
        #define CONFIG_DIRAC_DECODER 1
        #define CONFIG_DNXHD_DECODER 1
        #define CONFIG_DPX_DECODER 1
        #define CONFIG_DSICINVIDEO_DECODER 1
        #define CONFIG_DVAUDIO_DECODER 1
        #define CONFIG_DVVIDEO_DECODER 1
        #define CONFIG_DXA_DECODER 1
        #define CONFIG_DXTORY_DECODER 1
        #define CONFIG_DXV_DECODER 1
        #define CONFIG_EACMV_DECODER 1
        #define CONFIG_EAMAD_DECODER 1
        #define CONFIG_EATGQ_DECODER 1
        #define CONFIG_EATGV_DECODER 1
        #define CONFIG_EATQI_DECODER 1
        #define CONFIG_EIGHTBPS_DECODER 1
        #define CONFIG_EIGHTSVX_EXP_DECODER 1
        #define CONFIG_EIGHTSVX_FIB_DECODER 1
        #define CONFIG_ESCAPE124_DECODER 1
        #define CONFIG_ESCAPE130_DECODER 1
        #define CONFIG_EXR_DECODER 1
        #define CONFIG_FFV1_DECODER 1
        #define CONFIG_FFVHUFF_DECODER 1
        #define CONFIG_FIC_DECODER 1
        #define CONFIG_FLASHSV_DECODER 1
        #define CONFIG_FLASHSV2_DECODER 1
        #define CONFIG_FLIC_DECODER 1
        #define CONFIG_FLV_DECODER 1
        #define CONFIG_FOURXM_DECODER 1
        #define CONFIG_FRAPS_DECODER 1
        #define CONFIG_FRWU_DECODER 1
        #define CONFIG_G2M_DECODER 1
        #define CONFIG_GIF_DECODER 1
        #define CONFIG_H261_DECODER 1
        #define CONFIG_H263_DECODER 1
        #define CONFIG_H263I_DECODER 1
        #define CONFIG_H263P_DECODER 1
        #define CONFIG_H264_DECODER 1
        #define CONFIG_H264_CRYSTALHD_DECODER 0
        #define CONFIG_H264_MEDIACODEC_DECODER 0
        #define CONFIG_H264_MMAL_DECODER 0
        #define CONFIG_H264_QSV_DECODER 0
        #define CONFIG_H264_VDA_DECODER 0
        #define CONFIG_H264_VDPAU_DECODER 0
        #define CONFIG_HAP_DECODER 1
        #define CONFIG_HEVC_DECODER 1
        #define CONFIG_HEVC_QSV_DECODER 0
        #define CONFIG_HNM4_VIDEO_DECODER 1
        #define CONFIG_HQ_HQA_DECODER 1
        #define CONFIG_HQX_DECODER 1
        #define CONFIG_HUFFYUV_DECODER 1
        #define CONFIG_IDCIN_DECODER 1
        #define CONFIG_IFF_ILBM_DECODER 1
        #define CONFIG_INDEO2_DECODER 1
        #define CONFIG_INDEO3_DECODER 1
        #define CONFIG_INDEO4_DECODER 1
        #define CONFIG_INDEO5_DECODER 1
        #define CONFIG_INTERPLAY_VIDEO_DECODER 1
        #define CONFIG_JPEG2000_DECODER 1
        #define CONFIG_JPEGLS_DECODER 1
        #define CONFIG_JV_DECODER 1
        #define CONFIG_KGV1_DECODER 1
        #define CONFIG_KMVC_DECODER 1
        #define CONFIG_LAGARITH_DECODER 1
        #define CONFIG_LOCO_DECODER 1
        #define CONFIG_M101_DECODER 1
        #define CONFIG_MAGICYUV_DECODER 1
        #define CONFIG_MDEC_DECODER 1
        #define CONFIG_MIMIC_DECODER 1
        #define CONFIG_MJPEG_DECODER 1
        #define CONFIG_MJPEGB_DECODER 1
        #define CONFIG_MMVIDEO_DECODER 1
        #define CONFIG_MOTIONPIXELS_DECODER 1
        #define CONFIG_MPEG_XVMC_DECODER 0
        #define CONFIG_MPEG1VIDEO_DECODER 1
        #define CONFIG_MPEG2VIDEO_DECODER 1
        #define CONFIG_MPEG4_DECODER 1
        #define CONFIG_MPEG4_CRYSTALHD_DECODER 0
        #define CONFIG_MPEG4_MMAL_DECODER 0
        #define CONFIG_MPEG4_VDPAU_DECODER 0
        #define CONFIG_MPEGVIDEO_DECODER 1
        #define CONFIG_MPEG_VDPAU_DECODER 0
        #define CONFIG_MPEG1_VDPAU_DECODER 0
        #define CONFIG_MPEG2_MMAL_DECODER 0
        #define CONFIG_MPEG2_CRYSTALHD_DECODER 0
        #define CONFIG_MPEG2_QSV_DECODER 0
        #define CONFIG_MSA1_DECODER 1
        #define CONFIG_MSMPEG4_CRYSTALHD_DECODER 0
        #define CONFIG_MSMPEG4V1_DECODER 1
        #define CONFIG_MSMPEG4V2_DECODER 1
        #define CONFIG_MSMPEG4V3_DECODER 1
        #define CONFIG_MSRLE_DECODER 1
        #define CONFIG_MSS1_DECODER 1
        #define CONFIG_MSS2_DECODER 1
        #define CONFIG_MSVIDEO1_DECODER 1
        #define CONFIG_MSZH_DECODER 1
        #define CONFIG_MTS2_DECODER 1
        #define CONFIG_MVC1_DECODER 1
        #define CONFIG_MVC2_DECODER 1
        #define CONFIG_MXPEG_DECODER 1
        #define CONFIG_NUV_DECODER 1
        #define CONFIG_PAF_VIDEO_DECODER 1
        #define CONFIG_PAM_DECODER 1
        #define CONFIG_PBM_DECODER 1
        #define CONFIG_PCX_DECODER 1
        #define CONFIG_PGM_DECODER 1
        #define CONFIG_PGMYUV_DECODER 1
        #define CONFIG_PICTOR_DECODER 1
        #define CONFIG_PNG_DECODER 1
        #define CONFIG_PPM_DECODER 1
        #define CONFIG_PRORES_DECODER 1
        #define CONFIG_PRORES_LGPL_DECODER 1
        #define CONFIG_PTX_DECODER 1
        #define CONFIG_QDRAW_DECODER 1
        #define CONFIG_QPEG_DECODER 1
        #define CONFIG_QTRLE_DECODER 1
        #define CONFIG_R10K_DECODER 1
        #define CONFIG_R210_DECODER 1
        #define CONFIG_RAWVIDEO_DECODER 1
        #define CONFIG_RL2_DECODER 1
        #define CONFIG_ROQ_DECODER 1
        #define CONFIG_RPZA_DECODER 1
        #define CONFIG_RSCC_DECODER 1
        #define CONFIG_RV10_DECODER 1
        #define CONFIG_RV20_DECODER 1
        #define CONFIG_RV30_DECODER 1
        #define CONFIG_RV40_DECODER 1
        #define CONFIG_S302M_DECODER 1
        #define CONFIG_SANM_DECODER 1
        #define CONFIG_SCREENPRESSO_DECODER 1
        #define CONFIG_SDX2_DPCM_DECODER 1
        #define CONFIG_SGI_DECODER 1
        #define CONFIG_SGIRLE_DECODER 1
        #define CONFIG_SHEERVIDEO_DECODER 1
        #define CONFIG_SMACKER_DECODER 1
        #define CONFIG_SMC_DECODER 1
        #define CONFIG_SMVJPEG_DECODER 1
        #define CONFIG_SNOW_DECODER 1
        #define CONFIG_SP5X_DECODER 1
        #define CONFIG_SUNRAST_DECODER 1
        #define CONFIG_SVQ1_DECODER 1
        #define CONFIG_SVQ3_DECODER 1
        #define CONFIG_TARGA_DECODER 1
        #define CONFIG_TARGA_Y216_DECODER 1
        #define CONFIG_TDSC_DECODER 1
        #define CONFIG_THEORA_DECODER 1
        #define CONFIG_THP_DECODER 1
        #define CONFIG_TIERTEXSEQVIDEO_DECODER 1
        #define CONFIG_TIFF_DECODER 1
        #define CONFIG_TMV_DECODER 1
        #define CONFIG_TRUEMOTION1_DECODER 1
        #define CONFIG_TRUEMOTION2_DECODER 1
        #define CONFIG_TRUEMOTION2RT_DECODER 1
        #define CONFIG_TSCC_DECODER 1
        #define CONFIG_TSCC2_DECODER 1
        #define CONFIG_TXD_DECODER 1
        #define CONFIG_ULTI_DECODER 1
        #define CONFIG_UTVIDEO_DECODER 1
        #define CONFIG_V210_DECODER 1
        #define CONFIG_V210X_DECODER 1
        #define CONFIG_V308_DECODER 1
        #define CONFIG_V408_DECODER 1
        #define CONFIG_V410_DECODER 1
        #define CONFIG_VB_DECODER 1
        #define CONFIG_VBLE_DECODER 1
        #define CONFIG_VC1_DECODER 1
        #define CONFIG_VC1_CRYSTALHD_DECODER 0
        #define CONFIG_VC1_VDPAU_DECODER 0
        #define CONFIG_VC1IMAGE_DECODER 1
        #define CONFIG_VC1_MMAL_DECODER 0
        #define CONFIG_VC1_QSV_DECODER 0
        #define CONFIG_VCR1_DECODER 1
        #define CONFIG_VMDVIDEO_DECODER 1
        #define CONFIG_VMNC_DECODER 1
        #define CONFIG_VP3_DECODER 1
        #define CONFIG_VP5_DECODER 1
        #define CONFIG_VP6_DECODER 1
        #define CONFIG_VP6A_DECODER 1
        #define CONFIG_VP6F_DECODER 1
        #define CONFIG_VP7_DECODER 1
        #define CONFIG_VP8_DECODER 1
        #define CONFIG_VP9_DECODER 1
        #define CONFIG_VQA_DECODER 1
        #define CONFIG_WEBP_DECODER 1
        #define CONFIG_WMV1_DECODER 1
        #define CONFIG_WMV2_DECODER 1
        #define CONFIG_WMV3_DECODER 1
        #define CONFIG_WMV3_CRYSTALHD_DECODER 0
        #define CONFIG_WMV3_VDPAU_DECODER 0
        #define CONFIG_WMV3IMAGE_DECODER 1
        #define CONFIG_WNV1_DECODER 1
        #define CONFIG_XAN_WC3_DECODER 1
        #define CONFIG_XAN_WC4_DECODER 1
        #define CONFIG_XBM_DECODER 1
        #define CONFIG_XFACE_DECODER 1
        #define CONFIG_XL_DECODER 1
        #define CONFIG_XWD_DECODER 1
        #define CONFIG_Y41P_DECODER 1
        #define CONFIG_YLC_DECODER 1
        #define CONFIG_YOP_DECODER 1
        #define CONFIG_YUV4_DECODER 1
        #define CONFIG_ZERO12V_DECODER 1
        #define CONFIG_ZEROCODEC_DECODER 1
        #define CONFIG_ZLIB_DECODER 1
        #define CONFIG_ZMBV_DECODER 1
        #define CONFIG_AAC_DECODER 1
        #define CONFIG_AAC_FIXED_DECODER 1
        #define CONFIG_AAC_LATM_DECODER 1
        #define CONFIG_AC3_DECODER 1
        #define CONFIG_AC3_FIXED_DECODER 1
        #define CONFIG_ALAC_DECODER 1
        #define CONFIG_ALS_DECODER 1
        #define CONFIG_AMRNB_DECODER 1
        #define CONFIG_AMRWB_DECODER 1
        #define CONFIG_APE_DECODER 1
        #define CONFIG_ATRAC1_DECODER 1
        #define CONFIG_ATRAC3_DECODER 1
        #define CONFIG_ATRAC3P_DECODER 1
        #define CONFIG_BINKAUDIO_DCT_DECODER 1
        #define CONFIG_BINKAUDIO_RDFT_DECODER 1
        #define CONFIG_BMV_AUDIO_DECODER 1
        #define CONFIG_COOK_DECODER 1
        #define CONFIG_DCA_DECODER 1
        #define CONFIG_DSD_LSBF_DECODER 1
        #define CONFIG_DSD_MSBF_DECODER 1
        #define CONFIG_DSD_LSBF_PLANAR_DECODER 1
        #define CONFIG_DSD_MSBF_PLANAR_DECODER 1
        #define CONFIG_DSICINAUDIO_DECODER 1
        #define CONFIG_DSS_SP_DECODER 1
        #define CONFIG_DST_DECODER 1
        #define CONFIG_EAC3_DECODER 1
        #define CONFIG_EVRC_DECODER 1
        #define CONFIG_FFWAVESYNTH_DECODER 1
        #define CONFIG_FLAC_DECODER 1
        #define CONFIG_G723_1_DECODER 1
        #define CONFIG_G729_DECODER 1
        #define CONFIG_GSM_DECODER 1
        #define CONFIG_GSM_MS_DECODER 1
        #define CONFIG_IAC_DECODER 1
        #define CONFIG_IMC_DECODER 1
        #define CONFIG_INTERPLAY_ACM_DECODER 1
        #define CONFIG_MACE3_DECODER 1
        #define CONFIG_MACE6_DECODER 1
        #define CONFIG_METASOUND_DECODER 1
        #define CONFIG_MLP_DECODER 1
        #define CONFIG_MP1_DECODER 1
        #define CONFIG_MP1FLOAT_DECODER 1
        #define CONFIG_MP2_DECODER 1
        #define CONFIG_MP2FLOAT_DECODER 1
        #define CONFIG_MP3_DECODER 1
        #define CONFIG_MP3FLOAT_DECODER 1
        #define CONFIG_MP3ADU_DECODER 1
        #define CONFIG_MP3ADUFLOAT_DECODER 1
        #define CONFIG_MP3ON4_DECODER 1
        #define CONFIG_MP3ON4FLOAT_DECODER 1
        #define CONFIG_MPC7_DECODER 1
        #define CONFIG_MPC8_DECODER 1
        #define CONFIG_NELLYMOSER_DECODER 1
        #define CONFIG_ON2AVC_DECODER 1
        #define CONFIG_OPUS_DECODER 1
        #define CONFIG_PAF_AUDIO_DECODER 1
        #define CONFIG_QCELP_DECODER 1
        #define CONFIG_QDM2_DECODER 1
        #define CONFIG_RA_144_DECODER 1
        #define CONFIG_RA_288_DECODER 1
        #define CONFIG_RALF_DECODER 1
        #define CONFIG_SHORTEN_DECODER 1
        #define CONFIG_SIPR_DECODER 1
        #define CONFIG_SMACKAUD_DECODER 1
        #define CONFIG_SONIC_DECODER 1
        #define CONFIG_TAK_DECODER 1
        #define CONFIG_TRUEHD_DECODER 1
        #define CONFIG_TRUESPEECH_DECODER 1
        #define CONFIG_TTA_DECODER 1
        #define CONFIG_TWINVQ_DECODER 1
        #define CONFIG_VMDAUDIO_DECODER 1
        #define CONFIG_VORBIS_DECODER 1
        #define CONFIG_WAVPACK_DECODER 1
        #define CONFIG_WMALOSSLESS_DECODER 1
        #define CONFIG_WMAPRO_DECODER 1
        #define CONFIG_WMAV1_DECODER 1
        #define CONFIG_WMAV2_DECODER 1
        #define CONFIG_WMAVOICE_DECODER 1
        #define CONFIG_WS_SND1_DECODER 1
        #define CONFIG_XMA1_DECODER 1

)" <<
R"(

        #define CONFIG_XMA2_DECODER 1
        #define CONFIG_PCM_ALAW_DECODER 1
        #define CONFIG_PCM_BLURAY_DECODER 1
        #define CONFIG_PCM_DVD_DECODER 1
        #define CONFIG_PCM_F32BE_DECODER 1
        #define CONFIG_PCM_F32LE_DECODER 1
        #define CONFIG_PCM_F64BE_DECODER 1
        #define CONFIG_PCM_F64LE_DECODER 1
        #define CONFIG_PCM_LXF_DECODER 1
        #define CONFIG_PCM_F16LE_DECODER 1
        #define CONFIG_PCM_F24LE_DECODER 1
        #define CONFIG_PCM_MULAW_DECODER 1
        #define CONFIG_PCM_S8_DECODER 1
        #define CONFIG_PCM_S8_PLANAR_DECODER 1
        #define CONFIG_PCM_S16BE_DECODER 1
        #define CONFIG_PCM_S16BE_PLANAR_DECODER 1
        #define CONFIG_PCM_S16LE_DECODER 1
        #define CONFIG_PCM_S16LE_PLANAR_DECODER 1
        #define CONFIG_PCM_S24BE_DECODER 1
        #define CONFIG_PCM_S24DAUD_DECODER 1
        #define CONFIG_PCM_S24LE_DECODER 1
        #define CONFIG_PCM_S24LE_PLANAR_DECODER 1
        #define CONFIG_PCM_S32BE_DECODER 1
        #define CONFIG_PCM_S32LE_DECODER 1
        #define CONFIG_PCM_S32LE_PLANAR_DECODER 1
        #define CONFIG_PCM_S64BE_DECODER 1
        #define CONFIG_PCM_S64LE_DECODER 1
        #define CONFIG_PCM_U8_DECODER 1
        #define CONFIG_PCM_U16BE_DECODER 1
        #define CONFIG_PCM_U16LE_DECODER 1
        #define CONFIG_PCM_U24BE_DECODER 1
        #define CONFIG_PCM_U24LE_DECODER 1
        #define CONFIG_PCM_U32BE_DECODER 1
        #define CONFIG_PCM_U32LE_DECODER 1
        #define CONFIG_PCM_ZORK_DECODER 1
        #define CONFIG_INTERPLAY_DPCM_DECODER 1
        #define CONFIG_ROQ_DPCM_DECODER 1
        #define CONFIG_SOL_DPCM_DECODER 1
        #define CONFIG_XAN_DPCM_DECODER 1
        #define CONFIG_ADPCM_4XM_DECODER 1
        #define CONFIG_ADPCM_ADX_DECODER 1
        #define CONFIG_ADPCM_AFC_DECODER 1
        #define CONFIG_ADPCM_AICA_DECODER 1
        #define CONFIG_ADPCM_CT_DECODER 1
        #define CONFIG_ADPCM_DTK_DECODER 1
        #define CONFIG_ADPCM_EA_DECODER 1
        #define CONFIG_ADPCM_EA_MAXIS_XA_DECODER 1
        #define CONFIG_ADPCM_EA_R1_DECODER 1
        #define CONFIG_ADPCM_EA_R2_DECODER 1
        #define CONFIG_ADPCM_EA_R3_DECODER 1
        #define CONFIG_ADPCM_EA_XAS_DECODER 1
        #define CONFIG_ADPCM_G722_DECODER 1
        #define CONFIG_ADPCM_G726_DECODER 1
        #define CONFIG_ADPCM_G726LE_DECODER 1
        #define CONFIG_ADPCM_IMA_AMV_DECODER 1
        #define CONFIG_ADPCM_IMA_APC_DECODER 1
        #define CONFIG_ADPCM_IMA_DAT4_DECODER 1
        #define CONFIG_ADPCM_IMA_DK3_DECODER 1
        #define CONFIG_ADPCM_IMA_DK4_DECODER 1
        #define CONFIG_ADPCM_IMA_EA_EACS_DECODER 1
        #define CONFIG_ADPCM_IMA_EA_SEAD_DECODER 1
        #define CONFIG_ADPCM_IMA_ISS_DECODER 1
        #define CONFIG_ADPCM_IMA_OKI_DECODER 1
        #define CONFIG_ADPCM_IMA_QT_DECODER 1
        #define CONFIG_ADPCM_IMA_RAD_DECODER 1
        #define CONFIG_ADPCM_IMA_SMJPEG_DECODER 1
        #define CONFIG_ADPCM_IMA_WAV_DECODER 1
        #define CONFIG_ADPCM_IMA_WS_DECODER 1
        #define CONFIG_ADPCM_MS_DECODER 1
        #define CONFIG_ADPCM_MTAF_DECODER 1
        #define CONFIG_ADPCM_PSX_DECODER 1
        #define CONFIG_ADPCM_SBPRO_2_DECODER 1
        #define CONFIG_ADPCM_SBPRO_3_DECODER 1
        #define CONFIG_ADPCM_SBPRO_4_DECODER 1
        #define CONFIG_ADPCM_SWF_DECODER 1
        #define CONFIG_ADPCM_THP_DECODER 1
        #define CONFIG_ADPCM_THP_LE_DECODER 1
        #define CONFIG_ADPCM_VIMA_DECODER 1
        #define CONFIG_ADPCM_XA_DECODER 1
        #define CONFIG_ADPCM_YAMAHA_DECODER 1
        #define CONFIG_SSA_DECODER 1
        #define CONFIG_ASS_DECODER 1
        #define CONFIG_CCAPTION_DECODER 1
        #define CONFIG_DVBSUB_DECODER 1
        #define CONFIG_DVDSUB_DECODER 1
        #define CONFIG_JACOSUB_DECODER 1
        #define CONFIG_MICRODVD_DECODER 1
        #define CONFIG_MOVTEXT_DECODER 1
        #define CONFIG_MPL2_DECODER 1
        #define CONFIG_PGSSUB_DECODER 1
        #define CONFIG_PJS_DECODER 1
        #define CONFIG_REALTEXT_DECODER 1
        #define CONFIG_SAMI_DECODER 1
        #define CONFIG_SRT_DECODER 1
        #define CONFIG_STL_DECODER 1
        #define CONFIG_SUBRIP_DECODER 1
        #define CONFIG_SUBVIEWER_DECODER 1
        #define CONFIG_SUBVIEWER1_DECODER 1
        #define CONFIG_TEXT_DECODER 1
        #define CONFIG_VPLAYER_DECODER 1
        #define CONFIG_WEBVTT_DECODER 1
        #define CONFIG_XSUB_DECODER 1
        #define CONFIG_AAC_AT_DECODER 0
        #define CONFIG_AC3_AT_DECODER 0
        #define CONFIG_ADPCM_IMA_QT_AT_DECODER 0
        #define CONFIG_ALAC_AT_DECODER 0
        #define CONFIG_AMR_NB_AT_DECODER 0
        #define CONFIG_EAC3_AT_DECODER 0
        #define CONFIG_GSM_MS_AT_DECODER 0
        #define CONFIG_ILBC_AT_DECODER 0
        #define CONFIG_MP1_AT_DECODER 0
        #define CONFIG_MP2_AT_DECODER 0
        #define CONFIG_MP3_AT_DECODER 0
        #define CONFIG_PCM_ALAW_AT_DECODER 0
        #define CONFIG_PCM_MULAW_AT_DECODER 0
        #define CONFIG_QDMC_AT_DECODER 0
        #define CONFIG_QDM2_AT_DECODER 0
        #define CONFIG_LIBCELT_DECODER 0
        #define CONFIG_LIBFDK_AAC_DECODER 0
        #define CONFIG_LIBGSM_DECODER 0
        #define CONFIG_LIBGSM_MS_DECODER 0
        #define CONFIG_LIBILBC_DECODER 0
        #define CONFIG_LIBOPENCORE_AMRNB_DECODER 0
        #define CONFIG_LIBOPENCORE_AMRWB_DECODER 0
        #define CONFIG_LIBOPENJPEG_DECODER 0
        #define CONFIG_LIBOPUS_DECODER 0
        #define CONFIG_LIBSCHROEDINGER_DECODER 0
        #define CONFIG_LIBSPEEX_DECODER 0
        #define CONFIG_LIBVORBIS_DECODER 0
        #define CONFIG_LIBVPX_VP8_DECODER 0
        #define CONFIG_LIBVPX_VP9_DECODER 0
        #define CONFIG_LIBZVBI_TELETEXT_DECODER 0
        #define CONFIG_BINTEXT_DECODER 1
        #define CONFIG_XBIN_DECODER 1
        #define CONFIG_IDF_DECODER 1
        #define CONFIG_LIBOPENH264_DECODER 0
        #define CONFIG_H263_CUVID_DECODER 0
        #define CONFIG_H264_CUVID_DECODER 0
        #define CONFIG_HEVC_CUVID_DECODER 0
        #define CONFIG_HEVC_MEDIACODEC_DECODER 0
        #define CONFIG_MJPEG_CUVID_DECODER 0
        #define CONFIG_MPEG1_CUVID_DECODER 0
        #define CONFIG_MPEG2_CUVID_DECODER 0
        #define CONFIG_MPEG4_CUVID_DECODER 0
        #define CONFIG_MPEG4_MEDIACODEC_DECODER 0
        #define CONFIG_VC1_CUVID_DECODER 0
        #define CONFIG_VP8_CUVID_DECODER 0
        #define CONFIG_VP8_MEDIACODEC_DECODER 0
        #define CONFIG_VP9_CUVID_DECODER 0
        #define CONFIG_VP9_MEDIACODEC_DECODER 0
        #define CONFIG_AA_DEMUXER 1
        #define CONFIG_AAC_DEMUXER 1
        #define CONFIG_AC3_DEMUXER 1
        #define CONFIG_ACM_DEMUXER 1
        #define CONFIG_ACT_DEMUXER 1
        #define CONFIG_ADF_DEMUXER 1
        #define CONFIG_ADP_DEMUXER 1
        #define CONFIG_ADS_DEMUXER 1
        #define CONFIG_ADX_DEMUXER 1
        #define CONFIG_AEA_DEMUXER 1
        #define CONFIG_AFC_DEMUXER 1
        #define CONFIG_AIFF_DEMUXER 1
        #define CONFIG_AIX_DEMUXER 1
        #define CONFIG_AMR_DEMUXER 1
        #define CONFIG_ANM_DEMUXER 1
        #define CONFIG_APC_DEMUXER 1
        #define CONFIG_APE_DEMUXER 1
        #define CONFIG_APNG_DEMUXER 1
        #define CONFIG_AQTITLE_DEMUXER 1
        #define CONFIG_ASF_DEMUXER 1
        #define CONFIG_ASF_O_DEMUXER 1
        #define CONFIG_ASS_DEMUXER 1
        #define CONFIG_AST_DEMUXER 1
        #define CONFIG_AU_DEMUXER 1
        #define CONFIG_AVI_DEMUXER 1
        #define CONFIG_AVISYNTH_DEMUXER 0
        #define CONFIG_AVR_DEMUXER 1
        #define CONFIG_AVS_DEMUXER 1
        #define CONFIG_BETHSOFTVID_DEMUXER 1
        #define CONFIG_BFI_DEMUXER 1
        #define CONFIG_BINTEXT_DEMUXER 1
        #define CONFIG_BINK_DEMUXER 1
        #define CONFIG_BIT_DEMUXER 1
        #define CONFIG_BMV_DEMUXER 1
        #define CONFIG_BFSTM_DEMUXER 1
        #define CONFIG_BRSTM_DEMUXER 1
        #define CONFIG_BOA_DEMUXER 1
        #define CONFIG_C93_DEMUXER 1
        #define CONFIG_CAF_DEMUXER 1
        #define CONFIG_CAVSVIDEO_DEMUXER 1
        #define CONFIG_CDG_DEMUXER 1
        #define CONFIG_CDXL_DEMUXER 1
        #define CONFIG_CINE_DEMUXER 1
        #define CONFIG_CONCAT_DEMUXER 1
        #define CONFIG_DATA_DEMUXER 1
        #define CONFIG_DAUD_DEMUXER 1
        #define CONFIG_DCSTR_DEMUXER 1
        #define CONFIG_DFA_DEMUXER 1
        #define CONFIG_DIRAC_DEMUXER 1
        #define CONFIG_DNXHD_DEMUXER 1
        #define CONFIG_DSF_DEMUXER 1
        #define CONFIG_DSICIN_DEMUXER 1
        #define CONFIG_DSS_DEMUXER 1
        #define CONFIG_DTS_DEMUXER 1
        #define CONFIG_DTSHD_DEMUXER 1
        #define CONFIG_DV_DEMUXER 1
        #define CONFIG_DVBSUB_DEMUXER 1
        #define CONFIG_DVBTXT_DEMUXER 1
        #define CONFIG_DXA_DEMUXER 1
        #define CONFIG_EA_DEMUXER 1
        #define CONFIG_EA_CDATA_DEMUXER 1
        #define CONFIG_EAC3_DEMUXER 1
        #define CONFIG_EPAF_DEMUXER 1
        #define CONFIG_FFM_DEMUXER 1
        #define CONFIG_FFMETADATA_DEMUXER 1
        #define CONFIG_FILMSTRIP_DEMUXER 1
        #define CONFIG_FLAC_DEMUXER 1
        #define CONFIG_FLIC_DEMUXER 1
        #define CONFIG_FLV_DEMUXER 1
        #define CONFIG_LIVE_FLV_DEMUXER 1
        #define CONFIG_FOURXM_DEMUXER 1
        #define CONFIG_FRM_DEMUXER 1
        #define CONFIG_FSB_DEMUXER 1
        #define CONFIG_G722_DEMUXER 1
        #define CONFIG_G723_1_DEMUXER 1
        #define CONFIG_G729_DEMUXER 1
        #define CONFIG_GENH_DEMUXER 1
        #define CONFIG_GIF_DEMUXER 1
        #define CONFIG_GSM_DEMUXER 1
        #define CONFIG_GXF_DEMUXER 1
        #define CONFIG_H261_DEMUXER 1
        #define CONFIG_H263_DEMUXER 1
        #define CONFIG_H264_DEMUXER 1
        #define CONFIG_HEVC_DEMUXER 1
        #define CONFIG_HLS_DEMUXER 1
        #define CONFIG_HNM_DEMUXER 1
        #define CONFIG_ICO_DEMUXER 1
        #define CONFIG_IDCIN_DEMUXER 1
        #define CONFIG_IDF_DEMUXER 1
        #define CONFIG_IFF_DEMUXER 1
        #define CONFIG_ILBC_DEMUXER 1
        #define CONFIG_IMAGE2_DEMUXER 1
        #define CONFIG_IMAGE2PIPE_DEMUXER 1
        #define CONFIG_IMAGE2_ALIAS_PIX_DEMUXER 1
        #define CONFIG_IMAGE2_BRENDER_PIX_DEMUXER 1
        #define CONFIG_INGENIENT_DEMUXER 1
        #define CONFIG_IPMOVIE_DEMUXER 1
        #define CONFIG_IRCAM_DEMUXER 1
        #define CONFIG_ISS_DEMUXER 1
        #define CONFIG_IV8_DEMUXER 1
        #define CONFIG_IVF_DEMUXER 1
        #define CONFIG_IVR_DEMUXER 1
        #define CONFIG_JACOSUB_DEMUXER 1
        #define CONFIG_JV_DEMUXER 1
        #define CONFIG_LMLM4_DEMUXER 1
        #define CONFIG_LOAS_DEMUXER 1
        #define CONFIG_LRC_DEMUXER 1
        #define CONFIG_LVF_DEMUXER 1
        #define CONFIG_LXF_DEMUXER 1
        #define CONFIG_M4V_DEMUXER 1
        #define CONFIG_MATROSKA_DEMUXER 1
        #define CONFIG_MGSTS_DEMUXER 1
        #define CONFIG_MICRODVD_DEMUXER 1
        #define CONFIG_MJPEG_DEMUXER 1
        #define CONFIG_MLP_DEMUXER 1
        #define CONFIG_MLV_DEMUXER 1
        #define CONFIG_MM_DEMUXER 1
        #define CONFIG_MMF_DEMUXER 1
        #define CONFIG_MOV_DEMUXER 1
        #define CONFIG_MP3_DEMUXER 1
        #define CONFIG_MPC_DEMUXER 1
        #define CONFIG_MPC8_DEMUXER 1
        #define CONFIG_MPEGPS_DEMUXER 1
        #define CONFIG_MPEGTS_DEMUXER 1
        #define CONFIG_MPEGTSRAW_DEMUXER 1
        #define CONFIG_MPEGVIDEO_DEMUXER 1
        #define CONFIG_MPJPEG_DEMUXER 1
        #define CONFIG_MPL2_DEMUXER 1
        #define CONFIG_MPSUB_DEMUXER 1
        #define CONFIG_MSF_DEMUXER 1
        #define CONFIG_MSNWC_TCP_DEMUXER 1
        #define CONFIG_MTAF_DEMUXER 1
        #define CONFIG_MTV_DEMUXER 1
        #define CONFIG_MUSX_DEMUXER 1
        #define CONFIG_MV_DEMUXER 1
        #define CONFIG_MVI_DEMUXER 1
        #define CONFIG_MXF_DEMUXER 1
        #define CONFIG_MXG_DEMUXER 1
        #define CONFIG_NC_DEMUXER 1
        #define CONFIG_NISTSPHERE_DEMUXER 1
        #define CONFIG_NSV_DEMUXER 1
        #define CONFIG_NUT_DEMUXER 1
        #define CONFIG_NUV_DEMUXER 1
        #define CONFIG_OGG_DEMUXER 1
        #define CONFIG_OMA_DEMUXER 1
        #define CONFIG_PAF_DEMUXER 1
        #define CONFIG_PCM_ALAW_DEMUXER 1
        #define CONFIG_PCM_MULAW_DEMUXER 1
        #define CONFIG_PCM_F64BE_DEMUXER 1
        #define CONFIG_PCM_F64LE_DEMUXER 1
        #define CONFIG_PCM_F32BE_DEMUXER 1
        #define CONFIG_PCM_F32LE_DEMUXER 1
        #define CONFIG_PCM_S32BE_DEMUXER 1
        #define CONFIG_PCM_S32LE_DEMUXER 1
        #define CONFIG_PCM_S24BE_DEMUXER 1
        #define CONFIG_PCM_S24LE_DEMUXER 1
        #define CONFIG_PCM_S16BE_DEMUXER 1
        #define CONFIG_PCM_S16LE_DEMUXER 1
        #define CONFIG_PCM_S8_DEMUXER 1
        #define CONFIG_PCM_U32BE_DEMUXER 1
        #define CONFIG_PCM_U32LE_DEMUXER 1
        #define CONFIG_PCM_U24BE_DEMUXER 1
        #define CONFIG_PCM_U24LE_DEMUXER 1
        #define CONFIG_PCM_U16BE_DEMUXER 1
        #define CONFIG_PCM_U16LE_DEMUXER 1
        #define CONFIG_PCM_U8_DEMUXER 1
        #define CONFIG_PJS_DEMUXER 1
        #define CONFIG_PMP_DEMUXER 1
        #define CONFIG_PVA_DEMUXER 1
        #define CONFIG_PVF_DEMUXER 1
        #define CONFIG_QCP_DEMUXER 1
        #define CONFIG_R3D_DEMUXER 1
        #define CONFIG_RAWVIDEO_DEMUXER 1
        #define CONFIG_REALTEXT_DEMUXER 1
        #define CONFIG_REDSPARK_DEMUXER 1
        #define CONFIG_RL2_DEMUXER 1
        #define CONFIG_RM_DEMUXER 1
        #define CONFIG_ROQ_DEMUXER 1
        #define CONFIG_RPL_DEMUXER 1
        #define CONFIG_RSD_DEMUXER 1
        #define CONFIG_RSO_DEMUXER 1
        #define CONFIG_RTP_DEMUXER 1
        #define CONFIG_RTSP_DEMUXER 1
        #define CONFIG_SAMI_DEMUXER 1
        #define CONFIG_SAP_DEMUXER 1
        #define CONFIG_SBG_DEMUXER 1
        #define CONFIG_SDP_DEMUXER 1
        #define CONFIG_SDR2_DEMUXER 1
        #define CONFIG_SEGAFILM_DEMUXER 1
        #define CONFIG_SHORTEN_DEMUXER 1
        #define CONFIG_SIFF_DEMUXER 1
        #define CONFIG_SLN_DEMUXER 1
        #define CONFIG_SMACKER_DEMUXER 1
        #define CONFIG_SMJPEG_DEMUXER 1
        #define CONFIG_SMUSH_DEMUXER 1
        #define CONFIG_SOL_DEMUXER 1
        #define CONFIG_SOX_DEMUXER 1
        #define CONFIG_SPDIF_DEMUXER 1
        #define CONFIG_SRT_DEMUXER 1
        #define CONFIG_STR_DEMUXER 1
        #define CONFIG_STL_DEMUXER 1
        #define CONFIG_SUBVIEWER1_DEMUXER 1
        #define CONFIG_SUBVIEWER_DEMUXER 1
        #define CONFIG_SUP_DEMUXER 1
        #define CONFIG_SVAG_DEMUXER 1
        #define CONFIG_SWF_DEMUXER 1
        #define CONFIG_TAK_DEMUXER 1
        #define CONFIG_TEDCAPTIONS_DEMUXER 1
        #define CONFIG_THP_DEMUXER 1
        #define CONFIG_THREEDOSTR_DEMUXER 1
        #define CONFIG_TIERTEXSEQ_DEMUXER 1
        #define CONFIG_TMV_DEMUXER 1
        #define CONFIG_TRUEHD_DEMUXER 1
        #define CONFIG_TTA_DEMUXER 1
        #define CONFIG_TXD_DEMUXER 1
        #define CONFIG_TTY_DEMUXER 1
        #define CONFIG_V210_DEMUXER 1
        #define CONFIG_V210X_DEMUXER 1
        #define CONFIG_VAG_DEMUXER 1
        #define CONFIG_VC1_DEMUXER 1
        #define CONFIG_VC1T_DEMUXER 1
        #define CONFIG_VIVO_DEMUXER 1
        #define CONFIG_VMD_DEMUXER 1
        #define CONFIG_VOBSUB_DEMUXER 1
        #define CONFIG_VOC_DEMUXER 1
        #define CONFIG_VPK_DEMUXER 1
        #define CONFIG_VPLAYER_DEMUXER 1
        #define CONFIG_VQF_DEMUXER 1
        #define CONFIG_W64_DEMUXER 1
        #define CONFIG_WAV_DEMUXER 1
        #define CONFIG_WC3_DEMUXER 1
        #define CONFIG_WEBM_DASH_MANIFEST_DEMUXER 1
        #define CONFIG_WEBVTT_DEMUXER 1
        #define CONFIG_WSAUD_DEMUXER 1
        #define CONFIG_WSD_DEMUXER 1
        #define CONFIG_WSVQA_DEMUXER 1
        #define CONFIG_WTV_DEMUXER 1
        #define CONFIG_WVE_DEMUXER 1
        #define CONFIG_WV_DEMUXER 1
        #define CONFIG_XA_DEMUXER 1
        #define CONFIG_XBIN_DEMUXER 1
        #define CONFIG_XMV_DEMUXER 1
        #define CONFIG_XVAG_DEMUXER 1
        #define CONFIG_XWMA_DEMUXER 1
        #define CONFIG_YOP_DEMUXER 1
        #define CONFIG_YUV4MPEGPIPE_DEMUXER 1

)" <<
R"(

        #define CONFIG_IMAGE_BMP_PIPE_DEMUXER 1
        #define CONFIG_IMAGE_DDS_PIPE_DEMUXER 1
        #define CONFIG_IMAGE_DPX_PIPE_DEMUXER 1
        #define CONFIG_IMAGE_EXR_PIPE_DEMUXER 1
        #define CONFIG_IMAGE_J2K_PIPE_DEMUXER 1
        #define CONFIG_IMAGE_JPEG_PIPE_DEMUXER 1
        #define CONFIG_IMAGE_JPEGLS_PIPE_DEMUXER 1
        #define CONFIG_IMAGE_PAM_PIPE_DEMUXER 1
        #define CONFIG_IMAGE_PBM_PIPE_DEMUXER 1
        #define CONFIG_IMAGE_PCX_PIPE_DEMUXER 1
        #define CONFIG_IMAGE_PGMYUV_PIPE_DEMUXER 1
        #define CONFIG_IMAGE_PGM_PIPE_DEMUXER 1
        #define CONFIG_IMAGE_PICTOR_PIPE_DEMUXER 1
        #define CONFIG_IMAGE_PNG_PIPE_DEMUXER 1
        #define CONFIG_IMAGE_PPM_PIPE_DEMUXER 1
        #define CONFIG_IMAGE_QDRAW_PIPE_DEMUXER 1
        #define CONFIG_IMAGE_SGI_PIPE_DEMUXER 1
        #define CONFIG_IMAGE_SUNRAST_PIPE_DEMUXER 1
        #define CONFIG_IMAGE_TIFF_PIPE_DEMUXER 1
        #define CONFIG_IMAGE_WEBP_PIPE_DEMUXER 1
        #define CONFIG_LIBGME_DEMUXER 0
        #define CONFIG_LIBMODPLUG_DEMUXER 0
        #define CONFIG_LIBNUT_DEMUXER 0
        #define CONFIG_LIBOPENMPT_DEMUXER 0
        #define CONFIG_A64MULTI_ENCODER 1
        #define CONFIG_A64MULTI5_ENCODER 1
        #define CONFIG_ALIAS_PIX_ENCODER 1
        #define CONFIG_AMV_ENCODER 1
        #define CONFIG_APNG_ENCODER 1
        #define CONFIG_ASV1_ENCODER 1
        #define CONFIG_ASV2_ENCODER 1
        #define CONFIG_AVRP_ENCODER 1
        #define CONFIG_AVUI_ENCODER 1
        #define CONFIG_AYUV_ENCODER 1
        #define CONFIG_BMP_ENCODER 1
        #define CONFIG_CINEPAK_ENCODER 1
        #define CONFIG_CLJR_ENCODER 1
        #define CONFIG_COMFORTNOISE_ENCODER 1
        #define CONFIG_DNXHD_ENCODER 1
        #define CONFIG_DPX_ENCODER 1
        #define CONFIG_DVVIDEO_ENCODER 1
        #define CONFIG_FFV1_ENCODER 1
        #define CONFIG_FFVHUFF_ENCODER 1
        #define CONFIG_FLASHSV_ENCODER 1
        #define CONFIG_FLASHSV2_ENCODER 1
        #define CONFIG_FLV_ENCODER 1
        #define CONFIG_GIF_ENCODER 1
        #define CONFIG_H261_ENCODER 1
        #define CONFIG_H263_ENCODER 1
        #define CONFIG_H263P_ENCODER 1
        #define CONFIG_HAP_ENCODER 0
        #define CONFIG_HUFFYUV_ENCODER 1
        #define CONFIG_JPEG2000_ENCODER 1
        #define CONFIG_JPEGLS_ENCODER 1
        #define CONFIG_LJPEG_ENCODER 1
        #define CONFIG_MJPEG_ENCODER 1
        #define CONFIG_MPEG1VIDEO_ENCODER 1
        #define CONFIG_MPEG2VIDEO_ENCODER 1
        #define CONFIG_MPEG4_ENCODER 1
        #define CONFIG_MSMPEG4V2_ENCODER 1
        #define CONFIG_MSMPEG4V3_ENCODER 1
        #define CONFIG_MSVIDEO1_ENCODER 1
        #define CONFIG_PAM_ENCODER 1
        #define CONFIG_PBM_ENCODER 1
        #define CONFIG_PCX_ENCODER 1
        #define CONFIG_PGM_ENCODER 1
        #define CONFIG_PGMYUV_ENCODER 1
        #define CONFIG_PNG_ENCODER 1
        #define CONFIG_PPM_ENCODER 1
        #define CONFIG_PRORES_ENCODER 1
        #define CONFIG_PRORES_AW_ENCODER 1
        #define CONFIG_PRORES_KS_ENCODER 1
        #define CONFIG_QTRLE_ENCODER 1
        #define CONFIG_R10K_ENCODER 1
        #define CONFIG_R210_ENCODER 1
        #define CONFIG_RAWVIDEO_ENCODER 1
        #define CONFIG_ROQ_ENCODER 1
        #define CONFIG_RV10_ENCODER 1
        #define CONFIG_RV20_ENCODER 1
        #define CONFIG_S302M_ENCODER 1
        #define CONFIG_SGI_ENCODER 1
        #define CONFIG_SNOW_ENCODER 1
        #define CONFIG_SUNRAST_ENCODER 1
        #define CONFIG_SVQ1_ENCODER 1
        #define CONFIG_TARGA_ENCODER 1
        #define CONFIG_TIFF_ENCODER 1
        #define CONFIG_UTVIDEO_ENCODER 1
        #define CONFIG_V210_ENCODER 1
        #define CONFIG_V308_ENCODER 1
        #define CONFIG_V408_ENCODER 1
        #define CONFIG_V410_ENCODER 1
        #define CONFIG_VC2_ENCODER 1
        #define CONFIG_WRAPPED_AVFRAME_ENCODER 1
        #define CONFIG_WMV1_ENCODER 1
        #define CONFIG_WMV2_ENCODER 1
        #define CONFIG_XBM_ENCODER 1
        #define CONFIG_XFACE_ENCODER 1
        #define CONFIG_XWD_ENCODER 1
        #define CONFIG_Y41P_ENCODER 1
        #define CONFIG_YUV4_ENCODER 1
        #define CONFIG_ZLIB_ENCODER 1
        #define CONFIG_ZMBV_ENCODER 1
        #define CONFIG_AAC_ENCODER 1
        #define CONFIG_AC3_ENCODER 1
        #define CONFIG_AC3_FIXED_ENCODER 1
        #define CONFIG_ALAC_ENCODER 1
        #define CONFIG_DCA_ENCODER 1
        #define CONFIG_EAC3_ENCODER 1
        #define CONFIG_FLAC_ENCODER 1
        #define CONFIG_G723_1_ENCODER 1
        #define CONFIG_MLP_ENCODER 1
        #define CONFIG_MP2_ENCODER 1
        #define CONFIG_MP2FIXED_ENCODER 1
        #define CONFIG_NELLYMOSER_ENCODER 1
        #define CONFIG_RA_144_ENCODER 1
        #define CONFIG_SONIC_ENCODER 1
        #define CONFIG_SONIC_LS_ENCODER 1
        #define CONFIG_TRUEHD_ENCODER 1
        #define CONFIG_TTA_ENCODER 1
        #define CONFIG_VORBIS_ENCODER 1
        #define CONFIG_WAVPACK_ENCODER 1
        #define CONFIG_WMAV1_ENCODER 1
        #define CONFIG_WMAV2_ENCODER 1
        #define CONFIG_PCM_ALAW_ENCODER 1
        #define CONFIG_PCM_F32BE_ENCODER 1
        #define CONFIG_PCM_F32LE_ENCODER 1
        #define CONFIG_PCM_F64BE_ENCODER 1
        #define CONFIG_PCM_F64LE_ENCODER 1
        #define CONFIG_PCM_MULAW_ENCODER 1
        #define CONFIG_PCM_S8_ENCODER 1
        #define CONFIG_PCM_S8_PLANAR_ENCODER 1
        #define CONFIG_PCM_S16BE_ENCODER 1
        #define CONFIG_PCM_S16BE_PLANAR_ENCODER 1
        #define CONFIG_PCM_S16LE_ENCODER 1
        #define CONFIG_PCM_S16LE_PLANAR_ENCODER 1
        #define CONFIG_PCM_S24BE_ENCODER 1
        #define CONFIG_PCM_S24DAUD_ENCODER 1
        #define CONFIG_PCM_S24LE_ENCODER 1
        #define CONFIG_PCM_S24LE_PLANAR_ENCODER 1
        #define CONFIG_PCM_S32BE_ENCODER 1
        #define CONFIG_PCM_S32LE_ENCODER 1
        #define CONFIG_PCM_S32LE_PLANAR_ENCODER 1
        #define CONFIG_PCM_S64BE_ENCODER 1
        #define CONFIG_PCM_S64LE_ENCODER 1
        #define CONFIG_PCM_U8_ENCODER 1
        #define CONFIG_PCM_U16BE_ENCODER 1
        #define CONFIG_PCM_U16LE_ENCODER 1
        #define CONFIG_PCM_U24BE_ENCODER 1
        #define CONFIG_PCM_U24LE_ENCODER 1
        #define CONFIG_PCM_U32BE_ENCODER 1
        #define CONFIG_PCM_U32LE_ENCODER 1
        #define CONFIG_ROQ_DPCM_ENCODER 1
        #define CONFIG_ADPCM_ADX_ENCODER 1
        #define CONFIG_ADPCM_G722_ENCODER 1
        #define CONFIG_ADPCM_G726_ENCODER 1
        #define CONFIG_ADPCM_IMA_QT_ENCODER 1
        #define CONFIG_ADPCM_IMA_WAV_ENCODER 1
        #define CONFIG_ADPCM_MS_ENCODER 1
        #define CONFIG_ADPCM_SWF_ENCODER 1
        #define CONFIG_ADPCM_YAMAHA_ENCODER 1
        #define CONFIG_SSA_ENCODER 1
        #define CONFIG_ASS_ENCODER 1
        #define CONFIG_DVBSUB_ENCODER 1
        #define CONFIG_DVDSUB_ENCODER 1
        #define CONFIG_MOVTEXT_ENCODER 1
        #define CONFIG_SRT_ENCODER 1
        #define CONFIG_SUBRIP_ENCODER 1
        #define CONFIG_TEXT_ENCODER 1
        #define CONFIG_WEBVTT_ENCODER 1
        #define CONFIG_XSUB_ENCODER 1
        #define CONFIG_AAC_AT_ENCODER 0
        #define CONFIG_ALAC_AT_ENCODER 0
        #define CONFIG_ILBC_AT_ENCODER 0
        #define CONFIG_PCM_ALAW_AT_ENCODER 0
        #define CONFIG_PCM_MULAW_AT_ENCODER 0
        #define CONFIG_LIBFDK_AAC_ENCODER 0
        #define CONFIG_LIBGSM_ENCODER 0
        #define CONFIG_LIBGSM_MS_ENCODER 0
        #define CONFIG_LIBILBC_ENCODER 0
        #define CONFIG_LIBMP3LAME_ENCODER 0
        #define CONFIG_LIBOPENCORE_AMRNB_ENCODER 0
        #define CONFIG_LIBOPENJPEG_ENCODER 0
        #define CONFIG_LIBOPUS_ENCODER 0
        #define CONFIG_LIBSCHROEDINGER_ENCODER 0
        #define CONFIG_LIBSHINE_ENCODER 0
        #define CONFIG_LIBSPEEX_ENCODER 0
        #define CONFIG_LIBTHEORA_ENCODER 0
        #define CONFIG_LIBTWOLAME_ENCODER 0
        #define CONFIG_LIBVO_AMRWBENC_ENCODER 0
        #define CONFIG_LIBVORBIS_ENCODER 0
        #define CONFIG_LIBVPX_VP8_ENCODER 0
        #define CONFIG_LIBVPX_VP9_ENCODER 0
        #define CONFIG_LIBWAVPACK_ENCODER 0
        #define CONFIG_LIBWEBP_ANIM_ENCODER 0
        #define CONFIG_LIBWEBP_ENCODER 0
        #define CONFIG_LIBX262_ENCODER 0
        #define CONFIG_LIBX264_ENCODER 0
        #define CONFIG_LIBX264RGB_ENCODER 0
        #define CONFIG_LIBX265_ENCODER 0
        #define CONFIG_LIBXAVS_ENCODER 0
        #define CONFIG_LIBXVID_ENCODER 0
        #define CONFIG_LIBOPENH264_ENCODER 0
        #define CONFIG_H264_NVENC_ENCODER 1
        #define CONFIG_H264_OMX_ENCODER 0
        #define CONFIG_H264_QSV_ENCODER 0
        #define CONFIG_H264_VAAPI_ENCODER 0
        #define CONFIG_H264_VIDEOTOOLBOX_ENCODER 0
        #define CONFIG_NVENC_ENCODER 1
        #define CONFIG_NVENC_H264_ENCODER 1
        #define CONFIG_NVENC_HEVC_ENCODER 1
        #define CONFIG_HEVC_NVENC_ENCODER 1
        #define CONFIG_HEVC_QSV_ENCODER 0
        #define CONFIG_HEVC_VAAPI_ENCODER 0
        #define CONFIG_LIBKVAZAAR_ENCODER 0
        #define CONFIG_MJPEG_VAAPI_ENCODER 0
        #define CONFIG_MPEG2_QSV_ENCODER 0
        #define CONFIG_ABENCH_FILTER 1
        #define CONFIG_ACOMPRESSOR_FILTER 1
        #define CONFIG_ACROSSFADE_FILTER 1
        #define CONFIG_ACRUSHER_FILTER 1
        #define CONFIG_ADELAY_FILTER 1
        #define CONFIG_AECHO_FILTER 1
        #define CONFIG_AEMPHASIS_FILTER 1
        #define CONFIG_AEVAL_FILTER 1
        #define CONFIG_AFADE_FILTER 1
        #define CONFIG_AFFTFILT_FILTER 1
        #define CONFIG_AFORMAT_FILTER 1
        #define CONFIG_AGATE_FILTER 1
        #define CONFIG_AINTERLEAVE_FILTER 1
        #define CONFIG_ALIMITER_FILTER 1
        #define CONFIG_ALLPASS_FILTER 1
        #define CONFIG_ALOOP_FILTER 1
        #define CONFIG_AMERGE_FILTER 1
        #define CONFIG_AMETADATA_FILTER 1
        #define CONFIG_AMIX_FILTER 1
        #define CONFIG_ANEQUALIZER_FILTER 1
        #define CONFIG_ANULL_FILTER 1
        #define CONFIG_APAD_FILTER 1
        #define CONFIG_APERMS_FILTER 1
        #define CONFIG_APHASER_FILTER 1
        #define CONFIG_APULSATOR_FILTER 1
        #define CONFIG_AREALTIME_FILTER 1
        #define CONFIG_ARESAMPLE_FILTER 1
        #define CONFIG_AREVERSE_FILTER 1
        #define CONFIG_ASELECT_FILTER 1
        #define CONFIG_ASENDCMD_FILTER 1
        #define CONFIG_ASETNSAMPLES_FILTER 1
        #define CONFIG_ASETPTS_FILTER 1
        #define CONFIG_ASETRATE_FILTER 1
        #define CONFIG_ASETTB_FILTER 1
        #define CONFIG_ASHOWINFO_FILTER 1
        #define CONFIG_ASIDEDATA_FILTER 1
        #define CONFIG_ASPLIT_FILTER 1
        #define CONFIG_ASTATS_FILTER 1
        #define CONFIG_ASTREAMSELECT_FILTER 1
        #define CONFIG_ASYNCTS_FILTER 0
        #define CONFIG_ATEMPO_FILTER 1
        #define CONFIG_ATRIM_FILTER 1
        #define CONFIG_AZMQ_FILTER 0
        #define CONFIG_BANDPASS_FILTER 1
        #define CONFIG_BANDREJECT_FILTER 1
        #define CONFIG_BASS_FILTER 1
        #define CONFIG_BIQUAD_FILTER 1
        #define CONFIG_BS2B_FILTER 0
        #define CONFIG_CHANNELMAP_FILTER 1
        #define CONFIG_CHANNELSPLIT_FILTER 1
        #define CONFIG_CHORUS_FILTER 1
        #define CONFIG_COMPAND_FILTER 1
        #define CONFIG_COMPENSATIONDELAY_FILTER 1
        #define CONFIG_CRYSTALIZER_FILTER 1
        #define CONFIG_DCSHIFT_FILTER 1
        #define CONFIG_DYNAUDNORM_FILTER 1
        #define CONFIG_EARWAX_FILTER 1
        #define CONFIG_EBUR128_FILTER 0
        #define CONFIG_EQUALIZER_FILTER 1
        #define CONFIG_EXTRASTEREO_FILTER 1
        #define CONFIG_FIREQUALIZER_FILTER 1
        #define CONFIG_FLANGER_FILTER 1
        #define CONFIG_HDCD_FILTER 1
        #define CONFIG_HIGHPASS_FILTER 1
        #define CONFIG_JOIN_FILTER 1
        #define CONFIG_LADSPA_FILTER 0
        #define CONFIG_LOUDNORM_FILTER 0
        #define CONFIG_LOWPASS_FILTER 1
        #define CONFIG_PAN_FILTER 1
        #define CONFIG_REPLAYGAIN_FILTER 1
        #define CONFIG_RESAMPLE_FILTER 0
        #define CONFIG_RUBBERBAND_FILTER 0
        #define CONFIG_SIDECHAINCOMPRESS_FILTER 1
        #define CONFIG_SIDECHAINGATE_FILTER 1
        #define CONFIG_SILENCEDETECT_FILTER 1
        #define CONFIG_SILENCEREMOVE_FILTER 1
        #define CONFIG_SOFALIZER_FILTER 0
        #define CONFIG_STEREOTOOLS_FILTER 1
        #define CONFIG_STEREOWIDEN_FILTER 1
        #define CONFIG_TREBLE_FILTER 1
        #define CONFIG_TREMOLO_FILTER 1
        #define CONFIG_VIBRATO_FILTER 1
        #define CONFIG_VOLUME_FILTER 1
        #define CONFIG_VOLUMEDETECT_FILTER 1
        #define CONFIG_AEVALSRC_FILTER 1
        #define CONFIG_ANOISESRC_FILTER 1
        #define CONFIG_ANULLSRC_FILTER 1
        #define CONFIG_FLITE_FILTER 0
        #define CONFIG_SINE_FILTER 1
        #define CONFIG_ANULLSINK_FILTER 1
        #define CONFIG_ALPHAEXTRACT_FILTER 1
        #define CONFIG_ALPHAMERGE_FILTER 1
        #define CONFIG_ASS_FILTER 0
        #define CONFIG_ATADENOISE_FILTER 1
        #define CONFIG_AVGBLUR_FILTER 1
        #define CONFIG_BBOX_FILTER 1
        #define CONFIG_BENCH_FILTER 1
        #define CONFIG_BITPLANENOISE_FILTER 1
        #define CONFIG_BLACKDETECT_FILTER 1
        #define CONFIG_BLACKFRAME_FILTER 0
        #define CONFIG_BLEND_FILTER 1
        #define CONFIG_BOXBLUR_FILTER 0
        #define CONFIG_BWDIF_FILTER 1
        #define CONFIG_CHROMAKEY_FILTER 1
        #define CONFIG_CIESCOPE_FILTER 1
        #define CONFIG_CODECVIEW_FILTER 1
        #define CONFIG_COLORBALANCE_FILTER 1
        #define CONFIG_COLORCHANNELMIXER_FILTER 1
        #define CONFIG_COLORKEY_FILTER 1
        #define CONFIG_COLORLEVELS_FILTER 1
        #define CONFIG_COLORMATRIX_FILTER 0
        #define CONFIG_COLORSPACE_FILTER 1
        #define CONFIG_CONVOLUTION_FILTER 1
        #define CONFIG_COPY_FILTER 1
        #define CONFIG_COREIMAGE_FILTER 0
        #define CONFIG_COVER_RECT_FILTER 0
        #define CONFIG_CROP_FILTER 1
        #define CONFIG_CROPDETECT_FILTER 0
        #define CONFIG_CURVES_FILTER 1
        #define CONFIG_DATASCOPE_FILTER 1
        #define CONFIG_DCTDNOIZ_FILTER 1
        #define CONFIG_DEBAND_FILTER 1
        #define CONFIG_DECIMATE_FILTER 1
        #define CONFIG_DEFLATE_FILTER 1
        #define CONFIG_DEJUDDER_FILTER 1
        #define CONFIG_DELOGO_FILTER 0
        #define CONFIG_DESHAKE_FILTER 1
        #define CONFIG_DETELECINE_FILTER 1
        #define CONFIG_DILATION_FILTER 1
        #define CONFIG_DISPLACE_FILTER 1
        #define CONFIG_DRAWBOX_FILTER 1
        #define CONFIG_DRAWGRAPH_FILTER 1
        #define CONFIG_DRAWGRID_FILTER 1
        #define CONFIG_DRAWTEXT_FILTER 0
        #define CONFIG_EDGEDETECT_FILTER 1
        #define CONFIG_ELBG_FILTER 1
        #define CONFIG_EQ_FILTER 0
        #define CONFIG_EROSION_FILTER 1
        #define CONFIG_EXTRACTPLANES_FILTER 1
        #define CONFIG_FADE_FILTER 1
        #define CONFIG_FFTFILT_FILTER 1
        #define CONFIG_FIELD_FILTER 1
        #define CONFIG_FIELDHINT_FILTER 1
        #define CONFIG_FIELDMATCH_FILTER 1
        #define CONFIG_FIELDORDER_FILTER 1
        #define CONFIG_FIND_RECT_FILTER 0
        #define CONFIG_FORMAT_FILTER 1
        #define CONFIG_FPS_FILTER 1
        #define CONFIG_FRAMEPACK_FILTER 1
        #define CONFIG_FRAMERATE_FILTER 1
        #define CONFIG_FRAMESTEP_FILTER 1
        #define CONFIG_FREI0R_FILTER 0
        #define CONFIG_FSPP_FILTER 0
        #define CONFIG_GBLUR_FILTER 1
        #define CONFIG_GEQ_FILTER 0
        #define CONFIG_GRADFUN_FILTER 1
        #define CONFIG_HALDCLUT_FILTER 1
        #define CONFIG_HFLIP_FILTER 1
        #define CONFIG_HISTEQ_FILTER 0
        #define CONFIG_HISTOGRAM_FILTER 1
        #define CONFIG_HQDN3D_FILTER 0
        #define CONFIG_HQX_FILTER 1
        #define CONFIG_HSTACK_FILTER 1
        #define CONFIG_HUE_FILTER 1
        #define CONFIG_HWDOWNLOAD_FILTER 1
        #define CONFIG_HWUPLOAD_FILTER 1
        #define CONFIG_HWUPLOAD_CUDA_FILTER 0
        #define CONFIG_HYSTERESIS_FILTER 1
        #define CONFIG_IDET_FILTER 1
        #define CONFIG_IL_FILTER 1
        #define CONFIG_INFLATE_FILTER 1
        #define CONFIG_INTERLACE_FILTER 0
        #define CONFIG_INTERLEAVE_FILTER 1
        #define CONFIG_KERNDEINT_FILTER 0
        #define CONFIG_LENSCORRECTION_FILTER 1
        #define CONFIG_LOOP_FILTER 1
        #define CONFIG_LUT_FILTER 1
        #define CONFIG_LUT2_FILTER 1
        #define CONFIG_LUT3D_FILTER 1

)" <<
R"(

        #define CONFIG_LUTRGB_FILTER 1
        #define CONFIG_LUTYUV_FILTER 1
        #define CONFIG_MASKEDCLAMP_FILTER 1
        #define CONFIG_MASKEDMERGE_FILTER 1
        #define CONFIG_MCDEINT_FILTER 0
        #define CONFIG_MERGEPLANES_FILTER 1
        #define CONFIG_MESTIMATE_FILTER 1
        #define CONFIG_METADATA_FILTER 1
        #define CONFIG_MINTERPOLATE_FILTER 1
        #define CONFIG_MPDECIMATE_FILTER 0
        #define CONFIG_NEGATE_FILTER 1
        #define CONFIG_NLMEANS_FILTER 1
        #define CONFIG_NNEDI_FILTER 0
        #define CONFIG_NOFORMAT_FILTER 1
        #define CONFIG_NOISE_FILTER 1
        #define CONFIG_NULL_FILTER 1
        #define CONFIG_OCR_FILTER 0
        #define CONFIG_OCV_FILTER 0
        #define CONFIG_OVERLAY_FILTER 1
        #define CONFIG_OWDENOISE_FILTER 0
        #define CONFIG_PAD_FILTER 1
        #define CONFIG_PALETTEGEN_FILTER 1
        #define CONFIG_PALETTEUSE_FILTER 1
        #define CONFIG_PERMS_FILTER 1
        #define CONFIG_PERSPECTIVE_FILTER 0
        #define CONFIG_PHASE_FILTER 0
        #define CONFIG_PIXDESCTEST_FILTER 1
        #define CONFIG_PP_FILTER 0
        #define CONFIG_PP7_FILTER 0
        #define CONFIG_PREWITT_FILTER 1
        #define CONFIG_PSNR_FILTER 1
        #define CONFIG_PULLUP_FILTER 0
        #define CONFIG_QP_FILTER 1
        #define CONFIG_RANDOM_FILTER 1
        #define CONFIG_READVITC_FILTER 1
        #define CONFIG_REALTIME_FILTER 1
        #define CONFIG_REMAP_FILTER 1
        #define CONFIG_REMOVEGRAIN_FILTER 1
        #define CONFIG_REMOVELOGO_FILTER 1
        #define CONFIG_REPEATFIELDS_FILTER 0
        #define CONFIG_REVERSE_FILTER 1
        #define CONFIG_ROTATE_FILTER 1
        #define CONFIG_SAB_FILTER 0
        #define CONFIG_SCALE_FILTER 1
        #define CONFIG_SCALE_NPP_FILTER 0
        #define CONFIG_SCALE_VAAPI_FILTER 0
        #define CONFIG_SCALE2REF_FILTER 1
        #define CONFIG_SELECT_FILTER 1
        #define CONFIG_SELECTIVECOLOR_FILTER 1
        #define CONFIG_SENDCMD_FILTER 1
        #define CONFIG_SEPARATEFIELDS_FILTER 1
        #define CONFIG_SETDAR_FILTER 1
        #define CONFIG_SETFIELD_FILTER 1
        #define CONFIG_SETPTS_FILTER 1
        #define CONFIG_SETSAR_FILTER 1
        #define CONFIG_SETTB_FILTER 1
        #define CONFIG_SHOWINFO_FILTER 1
        #define CONFIG_SHOWPALETTE_FILTER 1
        #define CONFIG_SHUFFLEFRAMES_FILTER 1
        #define CONFIG_SHUFFLEPLANES_FILTER 1
        #define CONFIG_SIDEDATA_FILTER 1
        #define CONFIG_SIGNALSTATS_FILTER 1
        #define CONFIG_SMARTBLUR_FILTER 0
        #define CONFIG_SOBEL_FILTER 1
        #define CONFIG_SPLIT_FILTER 1
        #define CONFIG_SPP_FILTER 0
        #define CONFIG_SSIM_FILTER 1
        #define CONFIG_STEREO3D_FILTER 0
        #define CONFIG_STREAMSELECT_FILTER 1
        #define CONFIG_SUBTITLES_FILTER 0
        #define CONFIG_SUPER2XSAI_FILTER 0
        #define CONFIG_SWAPRECT_FILTER 1
        #define CONFIG_SWAPUV_FILTER 1
        #define CONFIG_TBLEND_FILTER 1
        #define CONFIG_TELECINE_FILTER 1
        #define CONFIG_THUMBNAIL_FILTER 1
        #define CONFIG_TILE_FILTER 1
        #define CONFIG_TINTERLACE_FILTER 0
        #define CONFIG_TRANSPOSE_FILTER 1
        #define CONFIG_TRIM_FILTER 1
        #define CONFIG_UNSHARP_FILTER 1
        #define CONFIG_USPP_FILTER 0
        #define CONFIG_VAGUEDENOISER_FILTER 0
        #define CONFIG_VECTORSCOPE_FILTER 1
        #define CONFIG_VFLIP_FILTER 1
        #define CONFIG_VIDSTABDETECT_FILTER 0
        #define CONFIG_VIDSTABTRANSFORM_FILTER 0
        #define CONFIG_VIGNETTE_FILTER 1
        #define CONFIG_VSTACK_FILTER 1
        #define CONFIG_W3FDIF_FILTER 1
        #define CONFIG_WAVEFORM_FILTER 1
        #define CONFIG_WEAVE_FILTER 1
        #define CONFIG_XBR_FILTER 1
        #define CONFIG_YADIF_FILTER 1
        #define CONFIG_ZMQ_FILTER 0
        #define CONFIG_ZOOMPAN_FILTER 1
        #define CONFIG_ZSCALE_FILTER 0
        #define CONFIG_ALLRGB_FILTER 1
        #define CONFIG_ALLYUV_FILTER 1
        #define CONFIG_CELLAUTO_FILTER 1
        #define CONFIG_COLOR_FILTER 1
        #define CONFIG_COREIMAGESRC_FILTER 0
        #define CONFIG_FREI0R_SRC_FILTER 0
        #define CONFIG_HALDCLUTSRC_FILTER 1
        #define CONFIG_LIFE_FILTER 1
        #define CONFIG_MANDELBROT_FILTER 1
        #define CONFIG_MPTESTSRC_FILTER 0
        #define CONFIG_NULLSRC_FILTER 1
        #define CONFIG_RGBTESTSRC_FILTER 1
        #define CONFIG_SMPTEBARS_FILTER 1
        #define CONFIG_SMPTEHDBARS_FILTER 1
        #define CONFIG_TESTSRC_FILTER 1
        #define CONFIG_TESTSRC2_FILTER 1
        #define CONFIG_YUVTESTSRC_FILTER 1
        #define CONFIG_NULLSINK_FILTER 1
        #define CONFIG_ADRAWGRAPH_FILTER 1
        #define CONFIG_AHISTOGRAM_FILTER 1
        #define CONFIG_APHASEMETER_FILTER 1
        #define CONFIG_AVECTORSCOPE_FILTER 1
        #define CONFIG_CONCAT_FILTER 1
        #define CONFIG_SHOWCQT_FILTER 1
        #define CONFIG_SHOWFREQS_FILTER 1
        #define CONFIG_SHOWSPECTRUM_FILTER 1
        #define CONFIG_SHOWSPECTRUMPIC_FILTER 1
        #define CONFIG_SHOWVOLUME_FILTER 1
        #define CONFIG_SHOWWAVES_FILTER 1
        #define CONFIG_SHOWWAVESPIC_FILTER 1
        #define CONFIG_SPECTRUMSYNTH_FILTER 1
        #define CONFIG_AMOVIE_FILTER 1
        #define CONFIG_MOVIE_FILTER 1
        #define CONFIG_H263_CUVID_HWACCEL 0
        #define CONFIG_H263_VAAPI_HWACCEL 0
        #define CONFIG_H263_VIDEOTOOLBOX_HWACCEL 0
        #define CONFIG_H264_CUVID_HWACCEL 0
        #define CONFIG_H264_D3D11VA_HWACCEL 0
        #define CONFIG_H264_DXVA2_HWACCEL 1
        #define CONFIG_H264_MEDIACODEC_HWACCEL 0
        #define CONFIG_H264_MMAL_HWACCEL 0
        #define CONFIG_H264_QSV_HWACCEL 0
        #define CONFIG_H264_VAAPI_HWACCEL 0
        #define CONFIG_H264_VDA_HWACCEL 0
        #define CONFIG_H264_VDA_OLD_HWACCEL 0
        #define CONFIG_H264_VDPAU_HWACCEL 0
        #define CONFIG_H264_VIDEOTOOLBOX_HWACCEL 0
        #define CONFIG_HEVC_CUVID_HWACCEL 0
        #define CONFIG_HEVC_D3D11VA_HWACCEL 0
        #define CONFIG_HEVC_DXVA2_HWACCEL 0
        #define CONFIG_HEVC_MEDIACODEC_HWACCEL 0
        #define CONFIG_HEVC_QSV_HWACCEL 0
        #define CONFIG_HEVC_VAAPI_HWACCEL 0
        #define CONFIG_HEVC_VDPAU_HWACCEL 0
        #define CONFIG_MJPEG_CUVID_HWACCEL 0
        #define CONFIG_MPEG1_CUVID_HWACCEL 0
        #define CONFIG_MPEG1_XVMC_HWACCEL 0
        #define CONFIG_MPEG1_VDPAU_HWACCEL 0
        #define CONFIG_MPEG1_VIDEOTOOLBOX_HWACCEL 0
        #define CONFIG_MPEG2_CUVID_HWACCEL 0
        #define CONFIG_MPEG2_XVMC_HWACCEL 0
        #define CONFIG_MPEG2_D3D11VA_HWACCEL 0
        #define CONFIG_MPEG2_DXVA2_HWACCEL 1
        #define CONFIG_MPEG2_MMAL_HWACCEL 0
        #define CONFIG_MPEG2_QSV_HWACCEL 0
        #define CONFIG_MPEG2_VAAPI_HWACCEL 0
        #define CONFIG_MPEG2_VDPAU_HWACCEL 0
        #define CONFIG_MPEG2_VIDEOTOOLBOX_HWACCEL 0
        #define CONFIG_MPEG4_CUVID_HWACCEL 0
        #define CONFIG_MPEG4_MEDIACODEC_HWACCEL 0
        #define CONFIG_MPEG4_MMAL_HWACCEL 0
        #define CONFIG_MPEG4_VAAPI_HWACCEL 0
        #define CONFIG_MPEG4_VDPAU_HWACCEL 0
        #define CONFIG_MPEG4_VIDEOTOOLBOX_HWACCEL 0
        #define CONFIG_VC1_CUVID_HWACCEL 0
        #define CONFIG_VC1_D3D11VA_HWACCEL 0
        #define CONFIG_VC1_DXVA2_HWACCEL 1
        #define CONFIG_VC1_VAAPI_HWACCEL 0
        #define CONFIG_VC1_VDPAU_HWACCEL 0
        #define CONFIG_VC1_MMAL_HWACCEL 0
        #define CONFIG_VC1_QSV_HWACCEL 0
        #define CONFIG_VP8_CUVID_HWACCEL 0
        #define CONFIG_VP8_MEDIACODEC_HWACCEL 0
        #define CONFIG_VP9_CUVID_HWACCEL 0
        #define CONFIG_VP9_D3D11VA_HWACCEL 0
        #define CONFIG_VP9_DXVA2_HWACCEL 0
        #define CONFIG_VP9_MEDIACODEC_HWACCEL 0
        #define CONFIG_VP9_VAAPI_HWACCEL 0
        #define CONFIG_WMV3_D3D11VA_HWACCEL 0
        #define CONFIG_WMV3_DXVA2_HWACCEL 1
        #define CONFIG_WMV3_VAAPI_HWACCEL 0
        #define CONFIG_WMV3_VDPAU_HWACCEL 0
        #define CONFIG_ALSA_INDEV 0
        #define CONFIG_AVFOUNDATION_INDEV 0
        #define CONFIG_BKTR_INDEV 0
        #define CONFIG_DECKLINK_INDEV 0
        #define CONFIG_DSHOW_INDEV 0
        #define CONFIG_DV1394_INDEV 0
        #define CONFIG_FBDEV_INDEV 0
        #define CONFIG_GDIGRAB_INDEV 1
        #define CONFIG_IEC61883_INDEV 0
        #define CONFIG_JACK_INDEV 0
        #define CONFIG_LAVFI_INDEV 1
        #define CONFIG_OPENAL_INDEV 0
        #define CONFIG_OSS_INDEV 0
        #define CONFIG_PULSE_INDEV 0
        #define CONFIG_QTKIT_INDEV 0
        #define CONFIG_SNDIO_INDEV 0
        #define CONFIG_V4L2_INDEV 0
        #define CONFIG_VFWCAP_INDEV 1
        #define CONFIG_X11GRAB_INDEV 0
        #define CONFIG_X11GRAB_XCB_INDEV 0
        #define CONFIG_LIBCDIO_INDEV 0
        #define CONFIG_LIBDC1394_INDEV 0
        #define CONFIG_A64_MUXER 1
        #define CONFIG_AC3_MUXER 1
        #define CONFIG_ADTS_MUXER 1
        #define CONFIG_ADX_MUXER 1
        #define CONFIG_AIFF_MUXER 1
        #define CONFIG_AMR_MUXER 1
        #define CONFIG_APNG_MUXER 1
        #define CONFIG_ASF_MUXER 1
        #define CONFIG_ASS_MUXER 1
        #define CONFIG_AST_MUXER 1
        #define CONFIG_ASF_STREAM_MUXER 1
        #define CONFIG_AU_MUXER 1
        #define CONFIG_AVI_MUXER 1
        #define CONFIG_AVM2_MUXER 1
        #define CONFIG_BIT_MUXER 1
        #define CONFIG_CAF_MUXER 1
        #define CONFIG_CAVSVIDEO_MUXER 1
        #define CONFIG_CRC_MUXER 1
        #define CONFIG_DASH_MUXER 1
        #define CONFIG_DATA_MUXER 1
        #define CONFIG_DAUD_MUXER 1
        #define CONFIG_DIRAC_MUXER 1
        #define CONFIG_DNXHD_MUXER 1
        #define CONFIG_DTS_MUXER 1
        #define CONFIG_DV_MUXER 1
        #define CONFIG_EAC3_MUXER 1
        #define CONFIG_F4V_MUXER 1
        #define CONFIG_FFM_MUXER 1
        #define CONFIG_FFMETADATA_MUXER 1
        #define CONFIG_FIFO_MUXER 1
        #define CONFIG_FILMSTRIP_MUXER 1
        #define CONFIG_FLAC_MUXER 1
        #define CONFIG_FLV_MUXER 1
        #define CONFIG_FRAMECRC_MUXER 1
        #define CONFIG_FRAMEHASH_MUXER 1
        #define CONFIG_FRAMEMD5_MUXER 1
        #define CONFIG_G722_MUXER 1
        #define CONFIG_G723_1_MUXER 1
        #define CONFIG_GIF_MUXER 1
        #define CONFIG_GSM_MUXER 1
        #define CONFIG_GXF_MUXER 1
        #define CONFIG_H261_MUXER 1
        #define CONFIG_H263_MUXER 1
        #define CONFIG_H264_MUXER 1
        #define CONFIG_HASH_MUXER 1
        #define CONFIG_HDS_MUXER 1
        #define CONFIG_HEVC_MUXER 1
        #define CONFIG_HLS_MUXER 1
        #define CONFIG_ICO_MUXER 1
        #define CONFIG_ILBC_MUXER 1
        #define CONFIG_IMAGE2_MUXER 1
        #define CONFIG_IMAGE2PIPE_MUXER 1
        #define CONFIG_IPOD_MUXER 1
        #define CONFIG_IRCAM_MUXER 1
        #define CONFIG_ISMV_MUXER 1
        #define CONFIG_IVF_MUXER 1
        #define CONFIG_JACOSUB_MUXER 1
        #define CONFIG_LATM_MUXER 1
        #define CONFIG_LRC_MUXER 1
        #define CONFIG_M4V_MUXER 1
        #define CONFIG_MD5_MUXER 1
        #define CONFIG_MATROSKA_MUXER 1
        #define CONFIG_MATROSKA_AUDIO_MUXER 1
        #define CONFIG_MICRODVD_MUXER 1
        #define CONFIG_MJPEG_MUXER 1
        #define CONFIG_MLP_MUXER 1
        #define CONFIG_MMF_MUXER 1
        #define CONFIG_MOV_MUXER 1
        #define CONFIG_MP2_MUXER 1
        #define CONFIG_MP3_MUXER 1
        #define CONFIG_MP4_MUXER 1
        #define CONFIG_MPEG1SYSTEM_MUXER 1
        #define CONFIG_MPEG1VCD_MUXER 1
        #define CONFIG_MPEG1VIDEO_MUXER 1
        #define CONFIG_MPEG2DVD_MUXER 1
        #define CONFIG_MPEG2SVCD_MUXER 1
        #define CONFIG_MPEG2VIDEO_MUXER 1
        #define CONFIG_MPEG2VOB_MUXER 1
        #define CONFIG_MPEGTS_MUXER 1
        #define CONFIG_MPJPEG_MUXER 1
        #define CONFIG_MXF_MUXER 1
        #define CONFIG_MXF_D10_MUXER 1
        #define CONFIG_MXF_OPATOM_MUXER 1
        #define CONFIG_NULL_MUXER 1
        #define CONFIG_NUT_MUXER 1
        #define CONFIG_OGA_MUXER 1
        #define CONFIG_OGG_MUXER 1
        #define CONFIG_OGV_MUXER 1
        #define CONFIG_OMA_MUXER 1
        #define CONFIG_OPUS_MUXER 1
        #define CONFIG_PCM_ALAW_MUXER 1
        #define CONFIG_PCM_MULAW_MUXER 1
        #define CONFIG_PCM_F64BE_MUXER 1
        #define CONFIG_PCM_F64LE_MUXER 1
        #define CONFIG_PCM_F32BE_MUXER 1
        #define CONFIG_PCM_F32LE_MUXER 1
        #define CONFIG_PCM_S32BE_MUXER 1
        #define CONFIG_PCM_S32LE_MUXER 1
        #define CONFIG_PCM_S24BE_MUXER 1
        #define CONFIG_PCM_S24LE_MUXER 1
        #define CONFIG_PCM_S16BE_MUXER 1
        #define CONFIG_PCM_S16LE_MUXER 1
        #define CONFIG_PCM_S8_MUXER 1
        #define CONFIG_PCM_U32BE_MUXER 1
        #define CONFIG_PCM_U32LE_MUXER 1
        #define CONFIG_PCM_U24BE_MUXER 1
        #define CONFIG_PCM_U24LE_MUXER 1
        #define CONFIG_PCM_U16BE_MUXER 1
        #define CONFIG_PCM_U16LE_MUXER 1
        #define CONFIG_PCM_U8_MUXER 1
        #define CONFIG_PSP_MUXER 1
        #define CONFIG_RAWVIDEO_MUXER 1
        #define CONFIG_RM_MUXER 1
        #define CONFIG_ROQ_MUXER 1
        #define CONFIG_RSO_MUXER 1
        #define CONFIG_RTP_MUXER 1
        #define CONFIG_RTP_MPEGTS_MUXER 1
        #define CONFIG_RTSP_MUXER 1
        #define CONFIG_SAP_MUXER 1
        #define CONFIG_SEGMENT_MUXER 1
        #define CONFIG_STREAM_SEGMENT_MUXER 1
        #define CONFIG_SINGLEJPEG_MUXER 1
        #define CONFIG_SMJPEG_MUXER 1
        #define CONFIG_SMOOTHSTREAMING_MUXER 1
        #define CONFIG_SOX_MUXER 1
        #define CONFIG_SPX_MUXER 1
        #define CONFIG_SPDIF_MUXER 1
        #define CONFIG_SRT_MUXER 1
        #define CONFIG_SWF_MUXER 1
        #define CONFIG_TEE_MUXER 1
        #define CONFIG_TG2_MUXER 1
        #define CONFIG_TGP_MUXER 1
        #define CONFIG_MKVTIMESTAMP_V2_MUXER 1
        #define CONFIG_TRUEHD_MUXER 1
        #define CONFIG_TTA_MUXER 1
        #define CONFIG_UNCODEDFRAMECRC_MUXER 1
        #define CONFIG_VC1_MUXER 1
        #define CONFIG_VC1T_MUXER 1
        #define CONFIG_VOC_MUXER 1
        #define CONFIG_W64_MUXER 1
        #define CONFIG_WAV_MUXER 1
        #define CONFIG_WEBM_MUXER 1
        #define CONFIG_WEBM_DASH_MANIFEST_MUXER 1
        #define CONFIG_WEBM_CHUNK_MUXER 1
        #define CONFIG_WEBP_MUXER 1
        #define CONFIG_WEBVTT_MUXER 1
        #define CONFIG_WTV_MUXER 1
        #define CONFIG_WV_MUXER 1
        #define CONFIG_YUV4MPEGPIPE_MUXER 1
        #define CONFIG_CHROMAPRINT_MUXER 0
        #define CONFIG_LIBNUT_MUXER 0
        #define CONFIG_ALSA_OUTDEV 0
        #define CONFIG_CACA_OUTDEV 0
        #define CONFIG_DECKLINK_OUTDEV 0
        #define CONFIG_FBDEV_OUTDEV 0
        #define CONFIG_OPENGL_OUTDEV 0
        #define CONFIG_OSS_OUTDEV 0
        #define CONFIG_PULSE_OUTDEV 0
        #define CONFIG_SDL2_OUTDEV 0
        #define CONFIG_SNDIO_OUTDEV 0
        #define CONFIG_V4L2_OUTDEV 0
        #define CONFIG_XV_OUTDEV 0
        #define CONFIG_AAC_PARSER 1
        #define CONFIG_AAC_LATM_PARSER 1
        #define CONFIG_AC3_PARSER 1
        #define CONFIG_ADX_PARSER 1
        #define CONFIG_BMP_PARSER 1
        #define CONFIG_CAVSVIDEO_PARSER 1
        #define CONFIG_COOK_PARSER 1
        #define CONFIG_DCA_PARSER 1
        #define CONFIG_DIRAC_PARSER 1
        #define CONFIG_DNXHD_PARSER 1
        #define CONFIG_DPX_PARSER 1
        #define CONFIG_DVAUDIO_PARSER 1
        #define CONFIG_DVBSUB_PARSER 1
        #define CONFIG_DVDSUB_PARSER 1
        #define CONFIG_DVD_NAV_PARSER 1
        #define CONFIG_FLAC_PARSER 1
        #define CONFIG_G729_PARSER 1
        #define CONFIG_GSM_PARSER 1
        #define CONFIG_H261_PARSER 1
        #define CONFIG_H263_PARSER 1
        #define CONFIG_H264_PARSER 1
        #define CONFIG_HEVC_PARSER 1
        #define CONFIG_MJPEG_PARSER 1
        #define CONFIG_MLP_PARSER 1

)" <<
R"(

        #define CONFIG_MPEG4VIDEO_PARSER 1
        #define CONFIG_MPEGAUDIO_PARSER 1
        #define CONFIG_MPEGVIDEO_PARSER 1
        #define CONFIG_OPUS_PARSER 1
        #define CONFIG_PNG_PARSER 1
        #define CONFIG_PNM_PARSER 1
        #define CONFIG_RV30_PARSER 1
        #define CONFIG_RV40_PARSER 1
        #define CONFIG_TAK_PARSER 1
        #define CONFIG_VC1_PARSER 1
        #define CONFIG_VORBIS_PARSER 1
        #define CONFIG_VP3_PARSER 1
        #define CONFIG_VP8_PARSER 1
        #define CONFIG_VP9_PARSER 1
        #define CONFIG_ASYNC_PROTOCOL 1
        #define CONFIG_BLURAY_PROTOCOL 0
        #define CONFIG_CACHE_PROTOCOL 1
        #define CONFIG_CONCAT_PROTOCOL 1
        #define CONFIG_CRYPTO_PROTOCOL 1
        #define CONFIG_DATA_PROTOCOL 1
        #define CONFIG_FFRTMPCRYPT_PROTOCOL 0
        #define CONFIG_FFRTMPHTTP_PROTOCOL 1
        #define CONFIG_FILE_PROTOCOL 1
        #define CONFIG_FTP_PROTOCOL 1
        #define CONFIG_GOPHER_PROTOCOL 1
        #define CONFIG_HLS_PROTOCOL 1
        #define CONFIG_HTTP_PROTOCOL 1
        #define CONFIG_HTTPPROXY_PROTOCOL 1
        #define CONFIG_HTTPS_PROTOCOL 1
        #define CONFIG_ICECAST_PROTOCOL 1
        #define CONFIG_MMSH_PROTOCOL 1
        #define CONFIG_MMST_PROTOCOL 1
        #define CONFIG_MD5_PROTOCOL 1
        #define CONFIG_PIPE_PROTOCOL 1
        #define CONFIG_RTMP_PROTOCOL 1
        #define CONFIG_RTMPE_PROTOCOL 0
        #define CONFIG_RTMPS_PROTOCOL 1
        #define CONFIG_RTMPT_PROTOCOL 1
        #define CONFIG_RTMPTE_PROTOCOL 0
        #define CONFIG_RTMPTS_PROTOCOL 1
        #define CONFIG_RTP_PROTOCOL 1
        #define CONFIG_SCTP_PROTOCOL 0
        #define CONFIG_SRTP_PROTOCOL 1
        #define CONFIG_SUBFILE_PROTOCOL 1
        #define CONFIG_TEE_PROTOCOL 1
        #define CONFIG_TCP_PROTOCOL 1
        #define CONFIG_TLS_GNUTLS_PROTOCOL 0
        #define CONFIG_TLS_SCHANNEL_PROTOCOL 1
        #define CONFIG_TLS_SECURETRANSPORT_PROTOCOL 0
        #define CONFIG_TLS_OPENSSL_PROTOCOL 1
        #define CONFIG_UDP_PROTOCOL 1
        #define CONFIG_UDPLITE_PROTOCOL 1
        #define CONFIG_UNIX_PROTOCOL 1
        #define CONFIG_LIBRTMP_PROTOCOL 0
        #define CONFIG_LIBRTMPE_PROTOCOL 0
        #define CONFIG_LIBRTMPS_PROTOCOL 0
        #define CONFIG_LIBRTMPT_PROTOCOL 0
        #define CONFIG_LIBRTMPTE_PROTOCOL 0
        #define CONFIG_LIBSSH_PROTOCOL 0
        #define CONFIG_LIBSMBCLIENT_PROTOCOL 0
)";
        }

        avutil.writeFileOnce(avutil.BinaryPrivateDir / "config.h", oss.str());
        avutil.Protected += IncludeDirectory(avutil.BinaryPrivateDir);

        if (avutil.getBuildSettings().TargetOS.Arch == ArchType::x86)
        {
            avutil.Public += "ARCH_X86_32=1"_def;
            avutil.Public += "ARCH_X86_64=0"_def;
        }
        else
        {
            avutil.Public += "ARCH_X86_32=0"_def;
            avutil.Public += "ARCH_X86_64=1"_def;
        }

        avutil.patch("libavutil/xga_font_data.h",
            "extern a",
            "extern SW_IMPORT a"
        );

        (avutil + avcodec)->IncludeDirectoriesOnly = true;
    }

    auto &swscale = ffmpeg.addTarget<LibraryTarget>("swscale");
    {
        swscale.setExtensionProgram(".asm", "org.sw.demo.yasm"_dep);

        swscale.ExportAllSymbols = true;
        swscale +=
            "libswscale/.*\\.asm"_rr,
            "libswscale/.*\\.c"_rr,
            "libswscale/.*\\.h"_rr;
        setup(swscale, "libswscale");
        swscale.Public += "SWS_MAX_FILTER_SIZE=256"_d;
        swscale.Public += avutil;

        setup_asm(swscale);
    }

    auto &swresample = ffmpeg.addTarget<LibraryTarget>("swresample");
    {
        swresample.ExportAllSymbols = true;
        swresample +=
            "libswresample/.*\\.c"_rr,
            "libswresample/.*\\.h"_rr;
        swresample -=
            "libswresample/soxr_resample.c",
            "libswresample/noise_shaping_data.c"
            ;
        setup(swresample, "libswresample");
        swresample.Public += avutil;
    }

    auto &postproc = ffmpeg.addTarget<LibraryTarget>("postproc");
    {
        postproc +=
            "libpostproc/.*\\.c"_rr,
            "libpostproc/.*\\.h"_rr;
        setup(postproc, "libpostproc");
        postproc.Public += avutil;
    }

    // avcodec
    {
        avcodec.setExtensionProgram(".asm", "org.sw.demo.yasm"_dep);

        avcodec +=
            "libavcodec/.*\\.asm"_rr,
            "libavcodec/.*\\.c"_rr,
            "libavcodec/.*\\.h"_rr;
        setup(avcodec, "libavcodec");

        avcodec -=
            "libavcodec/audiotoolboxenc.c",
            "libavcodec/audiotoolboxdec.c",
            "libavcodec/aacpsdata.c",
            //"libavcodec/crystalhd.c",
            "libavcodec/ffjni.c",
            "libavcodec/hapenc.c",

            "libavcodec/rkmppdec.c",
            "libavcodec/librsvgdec.c",

            "libavcodec/pcm.c",
            "libavcodec/libaribb24.c",
            //"libavcodec/mjpegdec.c",

            "libavcodec/libcelt_dec.c",
            "libavcodec/libdavs2.c",
            "libavcodec/libfdk-aacdec.c",
            "libavcodec/libfdk-aacenc.c",
            "libavcodec/libgsmdec.c",
            "libavcodec/libgsmenc.c",
            "libavcodec/libmp3lame.c",
            "libavcodec/libopenh264dec.c",
            "libavcodec/libopenh264.c",
            "libavcodec/libilbc.c",
            "libavcodec/libopenh264enc.c",
            "libavcodec/libkvazaar.c",
            "libavcodec/libopus.c",
            "libavcodec/libopusdec.c",
            "libavcodec/libopusenc.c",
            "libavcodec/libshine.c",
            "libavcodec/libspeexenc.c",
            "libavcodec/libspeexdec.c",
            "libavcodec/libvo-amrwbenc.c",
            //"libavcodec/libvpx.c",
            "libavcodec/libtwolame.c",
            "libavcodec/libvpxdec.c",
            "libavcodec/libvpxenc.c",
            "libavcodec/libtheoraenc.c",
            "libavcodec/libx265.c",
            "libavcodec/libxvid.c",
            "libavcodec/libxavs.c",
            "libavcodec/libxavs2.c",
            "libavcodec/libx264.c",
            "libavcodec/libzvbi-teletextdec.c",

            "libavcodec/cuvid.*"_rr,
            "libavcodec/vp9.*"_rr,
            "libavcodec/libaom.*"_rr,
            "libavcodec/codec2.*"_rr,
            "libavcodec/libcodec2.*"_rr,
            "libavcodec/amf.*"_rr,
            "libavcodec/nvdec.*"_rr,
            "libavcodec/nvenc.*"_rr,
            "libavcodec/mediacodec.*"_rr,
            "libavcodec/mmaldec.c",
            "libavcodec/omx.c",
            "libavcodec/videotoolboxenc.c",
            "libavcodec/videotoolbox.c",
            "libavcodec/aacps.c",
            "libavcodec/.*tablegen.*"_rr,
            "libavcodec/qsv.*"_rr,
            "libavcodec/va.*"_rr,
            "libavcodec/vdpau.*"_rr,

            "libavcodec/adpcm.c",
            "libavcodec/adpcmenc.c",
            //"libavcodec/allcodecs.c",
            "libavcodec/av1dec.c",
            //"libavcodec/h264dec.c",
            "libavcodec/libdav1d.c",
            "libavcodec/libjxldec.c",
            "libavcodec/libjxlenc.c",
            "libavcodec/librav1e.c",
            "libavcodec/libsvtav1.c",
            "libavcodec/libuavs3d.c",
            //"libavcodec/mpegvideo.c",
            //"libavcodec/mpegvideo_enc.c",
            "libavcodec/proresdec2.c",
            "libavcodec/vp3.c"
            //"libavcodec/x86/hevcdsp_init.c"
            ;

        // data vars are not exported properly on win
        //avcodec += "libavutil/xga_font_data.c";

        setup_asm(avcodec);
        avcodec.writeFileOnce("config.asm");

        avcodec += "CONFIG_MSMPEG4DEC=1"_def;
        avcodec += "CONFIG_MSMPEG4ENC=1"_def;
        avcodec += "CONFIG_INFLATE_WRAPPER"_def;
        avcodec += "CONFIG_DEFLATE_WRAPPER"_def;
        avcodec += "CONFIG_SPEEDHQ_ENCODER"_def;
        avcodec += "CONFIG_MPEGVIDEODEC"_def;
        avcodec += "HAVE_AVX512ICL_EXTERNAL=0"_def;

        avcodec.Public += swresample;
        //avcodec.Public += avutil;
        (avcodec + avutil)->IncludeDirectoriesOnly = true;
        (avcodec + avfilter)->IncludeDirectoriesOnly = true;
        //(avcodec + avformat)->IncludeDirectoriesOnly = true;

        avcodec += "Ole32.lib"_slib;
        avcodec += "uuid.lib"_slib;
        avcodec += "mfuuid.lib"_slib;
        avcodec += "gdi32.lib"_slib;
        avcodec += "strmiids.lib"_slib;

        avcodec.Public += "org.sw.demo.madler.zlib"_dep;
        avcodec.Public += "org.sw.demo.uclouvain.openjpeg.openjp2"_dep;
        avcodec.Public += "org.sw.demo.gnu.iconv.libiconv"_dep;
        avcodec.Public += "org.sw.demo.bzip2"_dep;
        avcodec.Public += "org.sw.demo.webmproject.webp"_dep;
        avcodec.Public += "org.sw.demo.xz_utils.lzma"_dep;
        avcodec.Public += "org.sw.demo.xiph.vorbis.libvorbis"_dep;
        avcodec.Public += "org.sw.demo.videolan.dav1d"_dep;
        avcodec.Public += "org.sw.demo.jxl"_dep;
        avcodec.Public += "org.sw.demo.ArtifexSoftware.thirdparty.lcms2"_dep;

        avcodec.replaceInFileOnce("libavcodec/ac3dec.h", "static int ff_eac3_parse_header", "int ff_eac3_parse_header");
        avcodec.replaceInFileOnce("libavcodec/ac3dec.h", "static void ff_eac3_decode_transform_coeffs_aht_ch", "void ff_eac3_decode_transform_coeffs_aht_ch");
        avcodec.replaceInFileOnce("libavcodec/ac3dec.h", "static void ff_eac3_apply_spectral_extension", "void ff_eac3_apply_spectral_extension");

        avcodec.writeFileOnce("libavcodec/bsf_list.c", R"(
            static const AVBitStreamFilter *bitstream_filters[] = {
                &ff_aac_adtstoasc_bsf,
                &ff_chomp_bsf,
                &ff_dump_extradata_bsf,
                &ff_dca_core_bsf,
                &ff_h264_mp4toannexb_bsf,
                &ff_hevc_mp4toannexb_bsf,
                &ff_imx_dump_header_bsf,
                &ff_mjpeg2jpeg_bsf,
                &ff_mjpega_dump_header_bsf,
                &ff_mp3_header_decompress_bsf,
                &ff_mpeg4_unpack_bframes_bsf,
                &ff_mov2textsub_bsf,
                &ff_noise_bsf,
                &ff_remove_extradata_bsf,
                &ff_text2movsub_bsf,
                //&ff_vp9_superframe_bsf,
                NULL };
            )");
        avcodec.writeFileOnce("libavcodec/parser_list.c", R"(
            static const AVCodecParser *parser_list[] = {
                &ff_h264_parser,
                &ff_gif_parser,
                NULL };
)");
        avcodec.writeFileOnce("libavcodec/codec_list.c", R"(
            static const FFCodec *codec_list[] = {
                &ff_h264_decoder,
                &ff_gif_encoder,
                &ff_gif_decoder,
                NULL };
)");
    }

    auto &avformat = ffmpeg.addTarget<LibraryTarget>("avformat");
    {
        avformat.setChecks("avutil");
        avformat +=
            "libavformat/.*\\.c"_rr,
            "libavformat/.*\\.h"_rr;
        setup(avformat, "libavformat");

        if (avformat.getCompilerType() == CompilerType::MSVC)
        {
            avformat.Public += "CONFIG_NETWORK"_d;
            avformat.Public += "HAVE_STRUCT_SOCKADDR_STORAGE"_d;
        }
        if (avformat.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            avformat.Public += "Secur32.lib"_slib;
        }

        avformat.Public += avcodec;
        avformat.Public += "org.sw.demo.xmlsoft.libxml2"_dep;
        avformat.Public += "org.sw.demo.openssl.ssl"_dep;

        avformat.writeFileOnce("libavformat/protocol_list.c", R"(
            static const URLProtocol *url_protocols[] = {
                &ff_async_protocol,
                &ff_cache_protocol,
                &ff_concat_protocol,
                &ff_crypto_protocol,
                &ff_data_protocol,
                &ff_ffrtmphttp_protocol,
                &ff_file_protocol,
                &ff_ftp_protocol,
                &ff_gopher_protocol,
                &ff_hls_protocol,
                &ff_http_protocol,
                &ff_httpproxy_protocol,
                &ff_https_protocol,
                &ff_icecast_protocol,
                &ff_mmsh_protocol,
                &ff_mmst_protocol,
                &ff_md5_protocol,
                &ff_pipe_protocol,
                &ff_rtmp_protocol,
                &ff_rtmps_protocol,
                &ff_rtmpt_protocol,
                &ff_rtmpts_protocol,
                &ff_rtp_protocol,
                &ff_srtp_protocol,
                &ff_subfile_protocol,
                &ff_tee_protocol,
                &ff_tcp_protocol,
                //&ff_tls_schannel_protocol,
                &ff_udp_protocol,
                &ff_udplite_protocol,
                //&ff_unix_protocol,
                NULL };
)");
        avformat.writeFileOnce("libavformat/muxer_list.c", R"(
            static const AVOutputFormat *muxer_list[] = {
                &ff_gif_muxer,
                NULL };
)");
        avformat.writeFileOnce("libavformat/demuxer_list.c", R"(
            static const AVInputFormat *demuxer_list[] = {
                &ff_h264_demuxer,
                &ff_mpegps_demuxer,
                &ff_mov_demuxer,
                NULL };
)");

        avformat -=
            "libavformat/bluray.c",
            "libavformat/libgme.c",
            "libavformat/libmodplug.c",
            "libavformat/libssh.c",
            "libavformat/libsmbclient.c",
            "libavformat/libopenmpt.c",
            "libavformat/librtmp.c",
            "libavformat/libsrt.c",
            //"libavformat/rtmpcrypt.c",
            //"libavformat/rtmpdh.c",
            "libavformat/chromaprint.c",
            "libavformat/tls_gnutls.c",
            "libavformat/tls_libtls.c",
            "libavformat/tls_mbedtls.c",
            "libavformat/tls_schannel.c",
            "libavformat/tls_securetransport.c",
            "libavformat/vapoursynth.c",
            "libavformat/sctp.c",

            "libavformat/avisynth.c",
            "libavformat/img2dec.c",
            "libavformat/img2_.*"_rr,
            "libavformat/libamqp.c",
            "libavformat/librist.c",
            "libavformat/libzmq.c",
            "libavformat/movenc.c",
            "libavformat/movenchint.c",
            "libavformat/pcmdec.c",
            "libavformat/pcmenc.c"
            //"libavformat/url.c"
            ;
        if (avformat.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            avformat -=
                "libavformat/unix.c";
            avformat.Public += "HAVE_WINSOCK2_H"_def;
            avformat.Public += "HAVE_STRUCT_POLLFD"_def;
            avformat.Public += "HAVE_PTHREAD_CANCEL=0"_def;
            avformat.Public += "HAVE_DOS_PATHS=1"_def;
        } else {
            avformat.Public += "HAVE_DOS_PATHS=0"_def;
        }

        avformat += "CONFIG_IMAGE2_DEMUXER"_def;
        avformat += "IMAGEAUTO_DEMUXER_CONFIG_IMAGE_GIF_PIPE_DEMUXER=0"_def;
        avformat += "IMAGEAUTO_DEMUXER_CONFIG_IMAGE_PHM_PIPE_DEMUXER=0"_def;
        avformat += "IMAGEAUTO_DEMUXER_CONFIG_IMAGE_PHOTOCD_PIPE_DEMUXER=0"_def;
    }

    // avfilter
    {
        avfilter +=
            "libavfilter/.*\\.c"_rr,
            "libavfilter/.*\\.h"_rr,
            "libavfilter/.*\\.m"_rr;
        setup(avfilter, "libavfilter");

        if (avfilter.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            avfilter -=
                "libavfilter/.*opencl.*.c"_rr,
                "libavfilter/.*vaapi.*.c"_rr,
                "libavfilter/.*qsv.*.c"_rr,
                "libavfilter/.*cuda.*.c"_rr,

                "libavfilter/af_asr.c",
                "libavfilter/vf_transpose_npp.c",
                "libavfilter/af_afftdn.c",
                "libavfilter/af_surround.c",
                "libavfilter/vf_lensfun.c",

                "libavfilter/af_lv2.c",
                "libavfilter/af_rubberband.c",
                "libavfilter/asrc_flite.c",
                "libavfilter/af_ladspa.c",
                "libavfilter/af_sofalizer.c",
                "libavfilter/af_bs2b.c",
                "libavfilter/f_zmq.c",
                "libavfilter/vf_ocr.c",
                "libavfilter/vf_libopencv.c",
                "libavfilter/vidstabutils.c",
                "libavfilter/vf_frei0r.c",
                "libavfilter/vf_vidstabdetect.c",
                "libavfilter/vf_subtitles.c",
                "libavfilter/af_loudnorm.c",
                "libavfilter/vf_vidstabtransform.c",
                "libavfilter/vf_scale_npp.c",
                "libavfilter/vf_zscale.c",
                "libavfilter/vf_libvmaf.c",
                "libavfilter/dnn/dnn_backend_tf.c",
                "libavfilter/blend_modes.c",

                "libavfilter/af_biquads.c",
                "libavfilter/blend_modes.c",
                "libavfilter/dnn/dnn_backend_openvino.c",
                "libavfilter/dnn/dnn_backend_tf.c",
                "libavfilter/fflcms2.c",
                "libavfilter/vf_iccdetect.c",
                "libavfilter/vf_iccgen.c",
                "libavfilter/vf_libplacebo.c",
                "libavfilter/vf_mcdeint.c",
                "libavfilter/vf_sharpen_npp.c",
                "libavfilter/vf_uspp.c",
                "libavfilter/vf_v360.c"
                //"libavfilter/x86/vf_gblur_init.c"
                ;
        }

        /*if (NOT APPLE)
        remove_src(libavfilter/vf_coreimage.m)
        endif()*/

        avfilter += "HAVE_AVX512_EXTERNAL=0"_def;

        avfilter.Public += avformat;
        avfilter.Public += swscale;
        avfilter.Public += postproc;
        avfilter.Public += "org.sw.demo.freetype-2"_dep;

        avfilter.writeFileOnce("libavfilter/filter_list.c", R"(
            static const AVFilter *filter_list[] = {
                &ff_vf_null,

                &ff_vf_format,
                &ff_vf_scale,

                &ff_vsrc_buffer,
                &ff_vsink_buffer,
                NULL };
)");
    }

    auto &avdevice = ffmpeg.addTarget<LibraryTarget>("avdevice");
    {
        avdevice +=
            "libavdevice/.*\\.c.*"_rr,
            "libavdevice/.*\\.h"_rr;
        setup(avdevice, "libavdevice");

        if (avdevice.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            avdevice.Public += "Shlwapi.lib"_slib;
            avdevice.Public += "Strmiids.lib"_slib;
            avdevice.Public += "Vfw32.lib"_slib;
        }

        avdevice.Public += avfilter;
        avdevice.Public += "org.sw.demo.valve.sdl.sdl-2"_dep;
        avdevice.Public += "org.sw.demo.kcat.openal-1"_dep;

        if (avdevice.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            avdevice -=
                "libavdevice/alsa.c",
                "libavdevice/alsa_dec.c",
                "libavdevice/alsa_enc.c",
                "libavdevice/iec61883.c",
                "libavdevice/caca.c",
                "libavdevice/fbdev_enc.c",
                "libavdevice/fbdev_common.c",
                "libavdevice/jack.c",
                "libavdevice/fbdev_dec.c",
                "libavdevice/libcdio.c",
                "libavdevice/libdc1394.c",
                "libavdevice/oss.c",
                "libavdevice/oss_dec.c",
                "libavdevice/oss_enc.c",
                "libavdevice/xcbgrab.c",
                "libavdevice/bktr.c",
                "libavdevice/xv.c",
                "libavdevice/opengl_enc.c",
                "libavdevice/kmsgrab.c"
                ;
        }

        avdevice -=
            "libavdevice/pulse.*"_rr,
            "libavdevice/sndio.*"_rr,
            "libavdevice/v4l.*"_rr,
            "libavdevice/decklink.*"_rr
            ;

        avdevice.writeFileOnce("libavdevice/outdev_list.c", R"(
            static const AVOutputFormat *outdev_list[] = {
                NULL };
)");

        avdevice.writeFileOnce("libavdevice/indev_list.c", R"(
            static const AVInputFormat *indev_list[] = {
                NULL };
)");
    }

    auto &ffmpeg2 = ffmpeg.addTarget<ExecutableTarget>("ffmpeg");
    {
        auto &ffmpeg = ffmpeg2;

        ffmpeg +=
            "fftools/.*\\.h"_rr,
            //"fftools/objpool.*"_rr,
            //"fftools/sync_queue.*"_rr,
            //"fftools/thread_queue.*"_rr,
            "fftools/opt_common.*"_rr,
            "fftools/cmdutils.*"_rr,
            "fftools/ffmpeg.*"_rr;
        setup(ffmpeg, "fftools");

        //ffmpeg += "fftools"_idir;

        ffmpeg.Public += "CONFIG_THIS_YEAR=2018"_d;
        if (ffmpeg.getBuildSettings().TargetOS.Type != OSType::Windows)
        {
            ffmpeg.Public += "AVCONV_DATADIR=\"/usr/local/share/ffmpeg\""_d;
            ffmpeg.Public += "CC_IDENT=\"Software Network\""_d;
            ffmpeg.Public += "FFMPEG_DATADIR=\"/usr/local/share/ffmpeg\""_d;
        }
        if (ffmpeg.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            ffmpeg.Public += "AVCONV_DATADIR=\"ffmpeg\""_d;
            ffmpeg.Public += "CC_IDENT=\"Software Network\""_d;
            ffmpeg.Public += "FFMPEG_DATADIR=\"ffmpeg\""_d;
        }

        ffmpeg.Public += avdevice;
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("avutil");
    s.checkFunctionExists("access");
    s.checkFunctionExists("aligned_malloc");
    s.checkFunctionExists("arc4random");
    s.checkFunctionExists("atan2f");
    s.checkFunctionExists("atanf");
    s.checkFunctionExists("cbrt");
    s.checkFunctionExists("cbrtf");
    s.checkFunctionExists("clock_gettime");
    s.checkFunctionExists("CommandLineToArgvW");
    s.checkFunctionExists("copysign");
    s.checkFunctionExists("cosf");
    s.checkFunctionExists("CoTaskMemFree");
    s.checkFunctionExists("CryptGenRandom");
    s.checkFunctionExists("dlopen");
    s.checkFunctionExists("dlsym");
    s.checkFunctionExists("erf");
    s.checkFunctionExists("exp2");
    s.checkFunctionExists("exp2f");
    s.checkFunctionExists("expf");
    s.checkFunctionExists("fcntl");
    s.checkFunctionExists("flt_lim");
    s.checkFunctionExists("fork");
    s.checkFunctionExists("getaddrinfo");
    s.checkFunctionExists("gethrtime");
    s.checkFunctionExists("getopt");
    s.checkFunctionExists("GetProcessAffinityMask");
    s.checkFunctionExists("GetProcessMemoryInfo");
    s.checkFunctionExists("GetProcessTimes");
    s.checkFunctionExists("getrusage");
    s.checkFunctionExists("GetSystemTimeAsFileTime");
    s.checkFunctionExists("gettimeofday");
    s.checkFunctionExists("glob");
    s.checkFunctionExists("glXGetProcAddress");
    s.checkFunctionExists("gmtime_r");
    s.checkFunctionExists("hypot");
    s.checkFunctionExists("inet_aton");
    s.checkFunctionExists("isatty");
    s.checkFunctionExists("isfinite");
    s.checkFunctionExists("isinf");
    s.checkFunctionExists("isnan");
    s.checkFunctionExists("jack_port_get_latency_range");
    s.checkFunctionExists("kbhit");
    s.checkFunctionExists("ldexpf");
    s.checkFunctionExists("lrint");
    s.checkFunctionExists("lrintf");
    s.checkFunctionExists("llrint");
    s.checkFunctionExists("llrintf");
    s.checkFunctionExists("LoadLibrary");
    s.checkFunctionExists("localtime_r");
    s.checkFunctionExists("log10f");
    s.checkFunctionExists("log2");
    s.checkFunctionExists("log2f");
    s.checkFunctionExists("lrint");
    s.checkFunctionExists("lrintf");
    s.checkFunctionExists("lstat");
    s.checkFunctionExists("lzo1x_999_compress");
    s.checkFunctionExists("mach_absolute_time");
    s.checkFunctionExists("MapViewOfFile");
    s.checkFunctionExists("memalign");
    s.checkFunctionExists("mkstemp");
    s.checkFunctionExists("mmap");
    s.checkFunctionExists("mprotect");
    s.checkFunctionExists("nanosleep");
    s.checkFunctionExists("PeekNamedPipe");
    s.checkFunctionExists("posix_memalign");
    s.checkFunctionExists("powf");
    s.checkFunctionExists("pthread_cancel");
    s.checkFunctionExists("pthread_create");
    s.checkFunctionExists("pthread_join");
    s.checkFunctionExists("rint");
    s.checkFunctionExists("round");
    s.checkFunctionExists("roundf");
    s.checkFunctionExists("sched_getaffinity");
    s.checkFunctionExists("SecIdentityCreate");
    s.checkFunctionExists("SetConsoleCtrlHandler");
    s.checkFunctionExists("SetConsoleTextAttribute");
    s.checkFunctionExists("setmode");
    s.checkFunctionExists("setrlimit");
    s.checkFunctionExists("sinf");
    s.checkFunctionExists("Sleep");
    s.checkFunctionExists("strerror_r");
    s.checkFunctionExists("strtoll");
    s.checkFunctionExists("sysconf");
    s.checkFunctionExists("sysctl");
    s.checkFunctionExists("usleep");
    s.checkFunctionExists("UTGetOSTypeFromString");
    s.checkFunctionExists("VirtualAlloc");
    s.checkFunctionExists("wglGetProcAddress");
    s.checkIncludeExists("alsa/asoundlib.h");
    s.checkIncludeExists("altivec.h");
    s.checkIncludeExists("arpa/inet.h");
    s.checkIncludeExists("asm/types.h");
    s.checkIncludeExists("AudioToolbox/AudioToolbox.h");
    s.checkIncludeExists("cdio/paranoia.h");
    s.checkIncludeExists("cdio/paranoia/paranoia.h");
    s.checkIncludeExists("d3d11.h");
    s.checkIncludeExists("DeckLinkAPI.h");
    s.checkIncludeExists("dev/bktr/ioctl_bt848.h");
    s.checkIncludeExists("dev/bktr/ioctl_meteor.h");
    s.checkIncludeExists("dev/ic/bt8xx.h");
    s.checkIncludeExists("dev/video/bktr/ioctl_bt848.h");
    s.checkIncludeExists("dev/video/meteor/ioctl_meteor.h");
    s.checkIncludeExists("direct.h");
    s.checkIncludeExists("dirent.h");
    s.checkIncludeExists("dispatch/dispatch.h");
    s.checkIncludeExists("dlfcn.h");
    s.checkIncludeExists("dxva2api.h");
    s.checkIncludeExists("dxva.h");
    s.checkIncludeExists("ES2/gl.h");
    s.checkIncludeExists("frei0r.h");
    s.checkIncludeExists("gsm.h");
    s.checkIncludeExists("io.h");
    s.checkIncludeExists("jni.h");
    s.checkIncludeExists("ladspa.h");
    s.checkIncludeExists("libcrystalhd/libcrystalhd_if.h");
    s.checkIncludeExists("linux/fb.h");
    s.checkIncludeExists("linux/videodev2.h");
    s.checkIncludeExists("linux/videodev.h");
    s.checkIncludeExists("machine/ioctl_bt848.h");
    s.checkIncludeExists("machine/ioctl_meteor.h");
    s.checkIncludeExists("mach/mach_time.h");
    s.checkIncludeExists("malloc.h");
    s.checkIncludeExists("msa.h");
    s.checkIncludeExists("net/udplite.h");
    s.checkIncludeExists("OMX_Core.h");
    s.checkIncludeExists("opencv2/core/core.c.h");
    s.checkIncludeExists("OpenGL/gl3.h");
    s.checkIncludeExists("openjpeg/1.5.openjpeg.h");
    s.checkIncludeExists("openjpeg/2.0/openjpeg.h");
    s.checkIncludeExists("openjpeg/2.1/openjpeg.h");
    s.checkIncludeExists("poll.h");
    s.checkIncludeExists("sndio.h");
    s.checkIncludeExists("soundcard.h");
    s.checkIncludeExists("sys/mman.h");
    s.checkIncludeExists("sys/param.h");
    s.checkIncludeExists("sys/resource.h");
    s.checkIncludeExists("sys/select.h");
    s.checkIncludeExists("sys/shm.h");
    s.checkIncludeExists("sys/soundcard.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/un.h");
    s.checkIncludeExists("sys/videoio.h");
    s.checkIncludeExists("termios.h");
    s.checkIncludeExists("udplite.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("valgrind/valgrind.h");
    s.checkIncludeExists("vdpau/vdpau.h");
    s.checkIncludeExists("vdpau/vdpau_x11.h");
    s.checkIncludeExists("VideoDecodeAcceleration/VDADecoder.h");
    s.checkIncludeExists("VideoToolbox/VideoToolbox.h");
    s.checkIncludeExists("windows.h");
    s.checkIncludeExists("winsock2.h");
    s.checkIncludeExists("X11/extensions/XvMClib.h");
    s.checkTypeSize("DXVA2_ConfigPictureDecode");
    s.checkTypeSize("DXVA_PicParams_HEVC");
    s.checkTypeSize("DXVA_PicParams_VP9");
    s.checkTypeSize("IBaseFilter");
    s.checkTypeSize("ID3D11VideoContext");
    s.checkTypeSize("ID3D11VideoDecoder");
    s.checkTypeSize("size_t");
    s.checkTypeSize("socklen_t");
    s.checkTypeSize("socklen_t");
    s.checkTypeSize("struct addrinfo");
    s.checkTypeSize("struct addrinfo");
    s.checkTypeSize("struct group_source_req");
    s.checkTypeSize("struct group_source_req");
    s.checkTypeSize("struct ipv6_mreq");
    s.checkTypeSize("struct ipv6_mreq");
    s.checkTypeSize("struct ip_mreq_source");
    s.checkTypeSize("struct ip_mreq_source");
    s.checkTypeSize("struct pollfd");
    s.checkTypeSize("struct sctp_event_subscribe");
    s.checkTypeSize("struct sockaddr_in6");
    s.checkTypeSize("struct sockaddr_in6");
    s.checkTypeSize("struct sockaddr_storage");
    s.checkTypeSize("struct sockaddr_storage");
    s.checkTypeSize("VADecPictureParameterBufferVP9");
    s.checkTypeSize("VAEncPictureParameterBufferH264");
    s.checkTypeSize("VAEncPictureParameterBufferHEVC");
    s.checkTypeSize("VAEncPictureParameterBufferJPEG");
    s.checkTypeSize("VAPictureParameterBufferHEVC");
    s.checkTypeSize("VAProcPipelineParameterBuffer");
    s.checkTypeSize("VdpPictureInfoHEVC");
    s.checkTypeSize("void *");
    {
        auto &c = s.checkSymbolExists("arc4random");
        c.Parameters.Includes.push_back("stdlib.h");
    }
    {
        auto &c = s.checkSymbolExists("cabs");
        c.Parameters.Includes.push_back("complex.h");
        c.Parameters.Includes.push_back("math.h");
    }
    {
        auto &c = s.checkSymbolExists("capCreateCaptureWindow");
        c.Parameters.Includes.push_back("windows.h");
        c.Parameters.Includes.push_back("vfw.h");
    }
    {
        auto &c = s.checkSymbolExists("cexp");
        c.Parameters.Includes.push_back("complex.h");
        c.Parameters.Includes.push_back("math.h");
    }
    {
        auto &c = s.checkSymbolExists("clock_gettime");
        c.Parameters.Includes.push_back("time.h");
    }
    {
        auto &c = s.checkSymbolExists("closesocket");
        c.Parameters.Includes.push_back("winsock2.h");
    }
    {
        auto &c = s.checkSymbolExists("CoTaskMemFree");
        c.Parameters.Includes.push_back("windows.h");
    }
    {
        auto &c = s.checkSymbolExists("CreateDIBSection");
        c.Parameters.Includes.push_back("windows.h");
    }
    {
        auto &c = s.checkSymbolExists("CreateThread");
        c.Parameters.Includes.push_back("windows.h");
    }
    {
        auto &c = s.checkSymbolExists("gcry_mpi_new");
        c.Parameters.Includes.push_back("gcrypt.h");
    }
    {
        auto &c = s.checkSymbolExists("getaddrinfo");
        c.Parameters.Includes.push_back("ws2tcpip.h");
    }
    {
        auto &c = s.checkSymbolExists("getenv");
        c.Parameters.Includes.push_back("stdlib.h");
    }
    {
        auto &c = s.checkSymbolExists("GetProcessAffinityMask");
        c.Parameters.Includes.push_back("windows.h");
    }
    {
        auto &c = s.checkSymbolExists("GetProcessTimes");
        c.Parameters.Includes.push_back("windows.h");
    }
    {
        auto &c = s.checkSymbolExists("GetSystemTimeAsFileTime");
        c.Parameters.Includes.push_back("windows.h");
    }
    {
        auto &c = s.checkSymbolExists("glob");
        c.Parameters.Includes.push_back("glob.h");
    }
    {
        auto &c = s.checkSymbolExists("InitializeSecurityContext");
        c.Parameters.Includes.push_back("windows.h");
        c.Parameters.Includes.push_back("security.h");
    }
    {
        auto &c = s.checkSymbolExists("kbhit");
        c.Parameters.Includes.push_back("conio.h");
    }
    {
        auto &c = s.checkSymbolExists("kCVImageBufferColorPrimaries_ITU_R_2020");
        c.Parameters.Includes.push_back("CoreVideo/CVImageBuffer.h");
    }
    {
        auto &c = s.checkSymbolExists("LoadLibrary");
        c.Parameters.Includes.push_back("windows.h");
    }
    {
        auto &c = s.checkSymbolExists("lstat");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkSymbolExists("MapViewOfFile");
        c.Parameters.Includes.push_back("windows.h");
    }
    {
        auto &c = s.checkSymbolExists("MMAL_PARAMETER_VIDEO_MAX_NUM_CALLBACKS");
        c.Parameters.Includes.push_back("interface/mmal/mmal.h");
    }
    {
        auto &c = s.checkSymbolExists("nanosleep");
        c.Parameters.Includes.push_back("time.h");
    }
    {
        auto &c = s.checkSymbolExists("PeekNamedPipe");
        c.Parameters.Includes.push_back("windows.h");
    }
    {
        auto &c = s.checkSymbolExists("SetConsoleCtrlHandler");
        c.Parameters.Includes.push_back("windows.h");
    }
    {
        auto &c = s.checkSymbolExists("SetConsoleTextAttribute");
        c.Parameters.Includes.push_back("windows.h");
    }
    {
        auto &c = s.checkSymbolExists("setmode");
        c.Parameters.Includes.push_back("io.h");
    }
    {
        auto &c = s.checkSymbolExists("Sleep");
        c.Parameters.Includes.push_back("windows.h");
    }
    {
        auto &c = s.checkSymbolExists("VirtualAlloc");
        c.Parameters.Includes.push_back("windows.h");
    }
    {
        auto &c = s.checkSymbolExists("VTCompressionSessionPrepareToEncodeFrames");
        c.Parameters.Includes.push_back("VideoToolbox/VTCompressionSession.h");
    }
    {
        auto &c = s.checkSymbolExists("XvGetPortAttribute");
        c.Parameters.Includes.push_back("X11/Xlib.h");
        c.Parameters.Includes.push_back("X11/extensions/Xvlib.h");
    }
    {
        auto &c = s.checkSymbolExists("_aligned_malloc");
        c.Parameters.Includes.push_back("malloc.h");
    }
    {
        auto &c = s.checkSymbolExists("_beginthreadex");
        c.Parameters.Includes.push_back("windows.h");
        c.Parameters.Includes.push_back("process.h");
    }
    {
        auto &c = s.checkStructMemberExists("audio_buf_info", "bytes");
        c.Parameters.Includes.push_back("sys/soundcard.h");
    }
    {
        auto &c = s.checkStructMemberExists("CONDITION_VARIABLE", "Ptr");
        c.Parameters.Includes.push_back("windows.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct msghdr", "msg_flags");
        c.Parameters.Includes.push_back("sys/socket.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct rusage", "ru_maxrss");
        c.Parameters.Includes.push_back("sys/time.h");
        c.Parameters.Includes.push_back("sys/resource.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct sockaddr", "sa_len");
        c.Parameters.Includes.push_back("sys/types.h");
        c.Parameters.Includes.push_back("sys/socket.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct sockaddr", "sa_len");
        c.Parameters.Includes.push_back("winsock2.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct stat", "st_mtim.tv_nsec");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
}
