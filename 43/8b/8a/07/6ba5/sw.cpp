void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &p = sln.addProject("preshing", "master");
    auto &turf = p.addTarget<StaticLibraryTarget>("turf");
    turf.Source = Git("https://github.com/preshing/turf", "", "{b}");

    turf +=
        "cmake/turf_config.h.in",
        "turf/.*"_rr;

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
    turf.fileWriteOnce("turf_userconfig.h", "", true);
        
        
    auto &junction = p.addTarget<StaticLibraryTarget>("junction");
    junction.Source = Git("https://github.com/preshing/junction", "", "{b}");

    junction +=
        "cmake/junction_config.h.in",
        "junction/.*"_rr;

    junction.Public += turf;

    junction.Variables["JUNCTION_TRACK_GRAMPA_STATS"] = "FALSE";
    junction.Variables["JUNCTION_USE_STRIPING"] = "TRUE";

    junction.configureFile("cmake/junction_config.h.in", "junction_config.h");
    junction.fileWriteOnce("junction_userconfig.h", "", true);
}
