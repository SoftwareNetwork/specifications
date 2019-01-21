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
    if (s.Settings.Native.CompilerType == CompilerType::MSVC)
        turf.Variables["TURF_WITH_SECURE_COMPILER"] = "FALSE";
    turf.Variables["TURF_REPLACE_OPERATOR_NEW"] = "FALSE";

    turf.Variables["TURF_HAS_LONG_LONG"] = "1";
    turf.Variables["TURF_HAS_STDINT"] = "1";
    turf.Variables["TURF_HAS_NOEXCEPT"] = "1";
    turf.Variables["TURF_HAS_CONSTEXPR"] = "1";
    turf.Variables["TURF_HAS_OVERRIDE"] = "1";
    turf.Variables["TURF_HAS_STATIC_ASSERT"] = "1";
    turf.Variables["TURF_HAS_MOVE"] = "1";

    turf.configureFile("cmake/turf_config.h.in", "turf_config.h");
    turf.writeFileOnce("turf_userconfig.h");
}
