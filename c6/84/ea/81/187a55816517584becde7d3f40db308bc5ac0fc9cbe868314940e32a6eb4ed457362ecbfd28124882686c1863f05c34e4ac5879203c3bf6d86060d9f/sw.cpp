void build(Solution &s)
{
    auto &turf = s.addTarget<StaticLibraryTarget>("preshing.turf", "master");
    turf += Git("https://github.com/preshing/turf", "", "{b}");

    turf +=
        "cmake/turf_config.h.in",
        "turf/.*"_rr;

    turf.Public +=
        "."_id;

    turf.Variables["TURF_USERCONFIG"];
    turf.Variables["TURF_ENABLE_CPP11"] = "1";
    turf.Variables["TURF_WITH_BOOST"] = "FALSE";
    turf.Variables["TURF_WITH_EXCEPTIONS"] = "FALSE";
    if (turf.getCompilerType() == CompilerType::MSVC)
        turf.Variables["TURF_WITH_SECURE_COMPILER"] = "FALSE";
    //turf.Variables["TURF_REPLACE_OPERATOR_NEW"] = 0;

    turf.Variables["TURF_HAS_LONG_LONG"] = "1";
    turf.Variables["TURF_HAS_STDINT"] = "1";
    turf.Variables["TURF_HAS_NOEXCEPT"] = "1";
    turf.Variables["TURF_HAS_CONSTEXPR"] = "1";
    turf.Variables["TURF_HAS_OVERRIDE"] = "1";
    turf.Variables["TURF_HAS_STATIC_ASSERT"] = "1";
    turf.Variables["TURF_HAS_MOVE"] = "1";

    turf.configureFile("cmake/turf_config.h.in", "turf_config.h");
    turf.writeFileOnce("turf_userconfig.h");

    turf.patch("turf/c/platform_detect.h",
        "defined(__arm64__)",
        "defined(__arm64__ ) || defined(__aarch64__)");
    turf.patch("turf/c/platform_detect.h",
        "defined(__ARM64_ARCH_8__)",
        "defined(__ARM64_ARCH_8__ ) || defined(__aarch64__)");
    turf.patch("turf/c/platform_detect.h",
        "#define TURF_TARGET_MINGW 1\n        #define TURF_TARGET_POSIX 1",
        "#define TURF_TARGET_MINGW 1\n        #define TURF_TARGET_WIN32 1");

    for (auto &&from : {
        "Relaxed"s,
        "Consume"s,
        "Acquire"s,
        "Release"s,
        "ConsumeRelease"s,
        "AcquireRelease"s,
    }) {
        turf.patch("turf/impl/Atomic_CPP11.h", from + " = ", from + "  = (int)");
    }
    turf.patch("turf/c/platform_detect.h",
        "    #else\n        #error \"Unrecognized platform!\"",
        R"(
    #elif defined(_M_ARM64) || defined(_M_ARM64EC)
        // ARM64
        #define TURF_CPU_ARM64 1
        #define TURF_PTR_SIZE 8
    #else
       #error "Unrecognized platform!"
)");
    turf.patch("turf/impl/TID_Win32.h",
        "GetCurrentThreadID",
        "GetCurrentThreadId"
    );
    turf.patch("turf/impl/TID_Win32.h",
        "GetCurrentProcessID",
        "GetCurrentProcessId"
    );
}
