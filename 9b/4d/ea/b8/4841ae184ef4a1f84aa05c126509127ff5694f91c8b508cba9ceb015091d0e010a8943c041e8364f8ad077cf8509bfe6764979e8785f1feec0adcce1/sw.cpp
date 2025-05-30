#pragma sw require header org.sw.demo.lexxmark.winflexbison.bison

void build(Solution &s)
{
    auto &thrift = s.addTarget<LibraryTarget>("apache.thrift", "0.22.0");
    thrift += Git("https://github.com/apache/thrift", "v{v}");

    auto &c = thrift.addTarget<ExecutableTarget>("compiler");
    {
        c.setRootDirectory("compiler/cpp");

        c += "src/.*"_rr;
        c -= "src/thrift/logging.cc";
        //c.configureFile("src/thrift/version.h.in", "thrift/version.h");

        gen_bison("org.sw.demo.lexxmark.winflexbison"_dep, c, "src/thrift/thrifty.yy", "thrift/thrifty.cc");
        gen_flex("org.sw.demo.lexxmark.winflexbison"_dep, c, "src/thrift/thriftl.ll");

        if (c.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            c += "Ole32.lib"_slib;
        }
    }

    // thrift
    {
        thrift.PackageDefinitions = true;
        thrift.setRootDirectory("lib/cpp");
        thrift.setChecks("thrift");

        thrift +=
            "src/thrift/.*\\.cpp"_rr,
            "src/thrift/.*\\.h"_rr,
            "src/thrift/.*\\.tcc"_rr;

        thrift += sw::Shared, "thrift_EXPORTS"_def;
        thrift.Public += sw::Static, "THRIFT_STATIC_DEFINE"_def;

        thrift.Public += "org.sw.demo.boost.locale"_dep;
        thrift.Public += "org.sw.demo.boost.scope_exit"_dep;
        thrift.Public += "org.sw.demo.boost.system"_dep;
        thrift.Public += "org.sw.demo.boost.thread"_dep;
        //thrift.Public += "org.sw.demo.libressl.ssl"_dep;
        thrift.Public += "org.sw.demo.openssl.ssl"_dep;
        thrift.Public += "org.sw.demo.madler.zlib"_dep;
        thrift.Public += "org.sw.demo.libevent"_dep;

        if (thrift.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            thrift.configureFile("src/thrift/windows/config.h", "thrift/config.h");
            thrift -= "src/thrift/VirtualProfiling.cpp";
            thrift += "Shlwapi.lib"_slib;
            thrift += "Ole32.lib"_slib;
        }
        else
        {
            thrift -= "src/thrift/windows/.*"_rr;
        }

        // remove non blocking (needs unix?)
        thrift -= "src/thrift/server/TNonblockingServer.cpp";
        thrift -= "src/thrift/qt/.*"_rr;
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("thrift");
    s.checkIncludeExists("stdlib.h");
    s.checkTypeSize("size_t");
    s.checkTypeSize("void *");
}
