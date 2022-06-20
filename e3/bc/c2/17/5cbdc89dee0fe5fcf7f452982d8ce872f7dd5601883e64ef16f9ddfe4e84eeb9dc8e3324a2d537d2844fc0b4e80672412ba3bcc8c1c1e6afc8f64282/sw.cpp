void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("facebook.folly", "2022.6.20.0");
    t += Git("https://github.com/facebook/folly", "v{M}.{m:02}.{p:02}.{t:02}");

    t += cpp17;

    t += "folly/.*\\.cpp"_rr;
    t += "folly/.*\\.h"_rr;

    t -= ".*tools.*"_rr;
    t -= ".*experimental.*"_rr;
    t -= ".*[tT]est.*"_rr;

    if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        t -= "folly/Benchmark.cpp";
        t -= "folly/Demangle.cpp";
        t -= "folly/Subprocess.cpp";
        t -= "folly/python/.*"_rr;
    }

    t.Public += "_ENABLE_EXTENDED_ALIGNED_STORAGE=1"_def;
    t.Public += "FOLLY_HAVE_LIBGFLAGS=1"_def;

    t.Public += "org.sw.demo.google.glog"_dep;
    t.Public += "org.sw.demo.google.double_conversion"_dep;
    t.Public += "org.sw.demo.boost.crc"_dep;
    t.Public += "org.sw.demo.boost.intrusive"_dep;
    t.Public += "org.sw.demo.boost.thread"_dep;
    t.Public += "org.sw.demo.boost.multi_index"_dep;
    t.Public += "org.sw.demo.boost.context"_dep;
    t.Public += "org.sw.demo.openssl.ssl"_dep;
    t.Public += "org.sw.demo.libevent"_dep;
    t.Public += "org.sw.demo.fmt"_dep;
    //t.Public += "org.sw.demo.python.lib"_dep;

    if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
        t += "NOMINMAX"_d;

    t.writeFileOnce("folly/folly-config.h");
}
