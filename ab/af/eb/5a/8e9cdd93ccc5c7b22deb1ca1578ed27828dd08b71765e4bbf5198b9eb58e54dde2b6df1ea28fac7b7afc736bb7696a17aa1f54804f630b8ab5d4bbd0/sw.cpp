void build(Solution &s)
{
    auto &p = s.addTarget<StaticLibrary>("facebook.folly", "2019.3.18.0");
    p += Git("https://github.com/facebook/folly", "v{M}.{m:02}.{p:02}.{t:02}");

    auto& t = p;
    //t.CPPVersion = CPPLanguageStandard::CPP14;
    t.CPPVersion = CPPLanguageStandard::CPP17;

    t += "folly/.*\\.cpp"_rr;
    t += "folly/.*\\.h"_rr;

    t -= ".*tools.*"_rr;
    t -= ".*experimental.*"_rr;
    t -= ".*exercises.*"_rr;
    t -= ".*[tT]est.*"_rr;

    t.Public += "_ENABLE_EXTENDED_ALIGNED_STORAGE=1"_def;
    t.Public += "FOLLY_HAVE_LIBGFLAGS=1"_def;

    t.Public += "org.sw.demo.google.glog-master"_dep;
    t.Public += "org.sw.demo.google.double_conversion"_dep;
    t.Public += "org.sw.demo.boost.crc"_dep;
    t.Public += "org.sw.demo.boost.intrusive"_dep;
    t.Public += "org.sw.demo.boost.thread"_dep;
    t.Public += "org.sw.demo.boost.multi_index"_dep;
    t.Public += "org.sw.demo.boost.context"_dep;
    t.Public += "org.sw.demo.openssl.ssl-*.*.*.*"_dep;
    t.Public += "org.sw.demo.libevent-master"_dep;
    t.Public += "org.sw.demo.python.lib"_dep;

    if (s.Settings.TargetOS.Type == OSType::Windows)
        t += "NOMINMAX"_d;

    t.writeFileOnce("folly/folly-config.h");
}
