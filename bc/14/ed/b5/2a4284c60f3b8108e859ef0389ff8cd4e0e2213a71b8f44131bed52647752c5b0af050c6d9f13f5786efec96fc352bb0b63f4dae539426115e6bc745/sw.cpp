void build(Solution &s)
{
    auto &flac = s.addProject("xiph.flac", "1.5.0");
    flac += Git("https://github.com/xiph/flac");

    auto &libflac = flac.addTarget<LibraryTarget>("libflac");
    libflac.PackageDefinitions = true;

    libflac +=
        "include/FLAC/.*\\.h"_rr,
        "include/share/.*\\.h"_rr,
        "src/libFLAC/.*\\.c"_r,
        "src/libFLAC/.*\\.h"_rr,
        "src/share/win_utf8_io/win_utf8_io.c";

    libflac.Private +=
        "src/libFLAC/include"_id;

    libflac.Public +=
        "include"_id;

    libflac.Private += "FLAC__HAS_OGG=1"_d;
    libflac.Private += "VERSION=PACKAGE_VERSION"_d;
    libflac.Private += sw::Shared, "FLAC_API_EXPORTS"_d;
    if (libflac.getBuildSettings().TargetOS.Type != OSType::Windows)
    {
        libflac.Public += "FLAC__USE_VISIBILITY_ATTR"_d;
        libflac -= "src/share/win_utf8_io/win_utf8_io.c";
    }
    libflac.Public += sw::Static, "FLAC__NO_DLL"_d;
    libflac.Public += "org.sw.demo.xiph.libogg"_dep;

    auto &libflacpp = flac.addTarget<LibraryTarget>("libflacpp");
    libflacpp +=
        "include/FLAC\\+\\+/.*\\.h"_rr,
        "src/libFLAC++/metadata.cpp",
        "src/libFLAC++/stream_decoder.cpp",
        "src/libFLAC++/stream_encoder.cpp";

    libflacpp.Private +=
        "src"_id;

    libflacpp.Public +=
        "include"_id;

    libflacpp.Private += "VERSION=PACKAGE_VERSION"_d;
    libflacpp.Private += sw::Shared, "FLACPP_API_EXPORTS"_d;
    if (libflacpp.getBuildSettings().TargetOS.Type != OSType::Windows)
    {
        libflacpp.Public += "FLAC__USE_VISIBILITY_ATTR"_d;
    }
    libflacpp.Public += sw::Static, "FLAC__NO_DLL"_d;

    libflacpp.Public += libflac;
}
