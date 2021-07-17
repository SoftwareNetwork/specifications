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

    for (auto &&from : {
        "Relaxed"s,
        "Consume"s,
        "Acquire"s,
        "Release"s,
        "ConsumeRelease"s,
        "AcquireRelease"s,
    })
        turf.patch("turf/impl/Atomic_CPP11.h", from + " = ", from + "  = (int)");
}
