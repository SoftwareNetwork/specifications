void build(Solution &s)
{
    auto &t = s.addStaticLibrary("chromium.third_party.zlib.google.compression_utils_portable", "master");
    t += RemoteFile("https://chromium.googlesource.com/chromium/src/+archive/master/third_party/zlib/google.tar.gz");

    t += "compression_utils_portable.cc";
    t += "compression_utils_portable.h";

    //t.Public += "USE_SYSTEM_ZLIB"_def;
    t.Public += "org.sw.demo.madler.zlib"_dep;
}
