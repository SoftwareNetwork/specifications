void build(Solution &s)
{
    auto &p = s.addStaticLibrary("cisco.openh264", "2.4.1");
    p += Git("https://github.com/cisco/openh264", "v{v}");

    auto &api = p.addLibrary("api");
    {
        auto &t = api;
        t.setRootDirectory("codec/api/wels");
    }
    auto &common = p.addStaticLibrary("common");
    {
        auto &t = common;
        t.setRootDirectory("codec/common");
        t.Public += api;
    }
    auto &processing = p.addLibrary("processing");
    {
        auto &t = processing;
        t.setRootDirectory("codec/processing");
        t += "interface/.*"_rr;
        t +=
          "src/adaptivequantization/.*"_rr,
          "src/backgrounddetection/.*"_rr,
          "src/common/.*"_rr,
          "src/complexityanalysis/.*"_rr,
          "src/denoise/.*"_rr,
          "src/downsample/.*"_rr,
          "src/imagerotate/.*"_rr,
          "src/scenechangedetection/.*"_rr,
          "src/scrolldetection/.*"_rr,
          "src/vaacalc/.*"_rr
          ;
        t += "src/common"_idir;
        t.Public += "interface"_idir;
        t.Public += common;
    }
    auto encdec = [&](auto &&name)
    {
        auto &core = p.addStaticLibrary(name + ".core");
        {
            auto &t = core;
            t.setRootDirectory("codec/" + name + "/core");
            t.Public += common;
        }
        auto &plus = p.addStaticLibrary(name + ".plus");
        {
            auto &t = plus;
            t.setRootDirectory("codec/" + name + "/plus");
            t.Public += core;
        }
        return std::tie(core,plus);
    };
    auto &&[dc,dp] = encdec("decoder"s);
    auto &&[ec,ep] = encdec("encoder"s);
    ec.Public += processing;
    ep.Public += processing;

    auto &console_common = p.addStaticLibrary("console.common");
    {
        auto &t = console_common;
        t.setRootDirectory("codec/console/common");
        t.Public += common;
    }
    auto &console_dec = p.addStaticLibrary("console.h264decoder");
    {
        auto &t = console_dec;
        t.setRootDirectory("codec/console/dec");
        t.Public += console_common, dp;
    }
    auto &console_enc = p.addStaticLibrary("console.h264encoder");
    {
        auto &t = console_enc;
        t.setRootDirectory("codec/console/enc");
        t.Public += console_common, ep;
    }

    // h264 main
    {
        auto &t = p;
        t += "openh264.def";
        if (!t.DryRun && t.getBuildSettings().TargetOS.is(OSType::Windows))
        {
            t.writeFileOnce(t.BinaryPrivateDir / "dummy.cpp");
            t += t.BinaryPrivateDir / "dummy.cpp";
        }
        t.Public += console_dec, console_enc;
    }

    /*auto &test = p.addExecutable("test.decoder");
    {
        auto &t = test;
        t.setRootDirectory("test/decoder");
        t += console_dec, "org.sw.demo.google.googletest.gtest"_dep;
        t += "User32.lib"_slib;
    }*/
}
