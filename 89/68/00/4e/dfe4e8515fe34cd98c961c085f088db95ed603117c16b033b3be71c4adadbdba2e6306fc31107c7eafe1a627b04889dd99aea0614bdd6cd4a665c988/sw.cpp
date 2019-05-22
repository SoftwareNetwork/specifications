void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("dmitigr.common", "master");
    t += Git("https://github.com/dmitigr/common");
    t += cpp17;

    t += "DMITIGR_COMMON_DLL_BUILDING"_def;
    t.Public += sw::Shared, "DMITIGR_COMMON_DLL"_def;

    t -= "lib/dmitigr/.*"_rr;
    t += "lib/dmitigr/common.cpp"_rr;

    //t -= "lib/dmitigr/common/graphicsmagick.cpp";
    //t.patch("lib/dmitigr/common/graphicsmagick.cpp", "GraphicsMagick/Magick++.h", "Magick++.h");
    //t += "org.sw.demo.graphicsmagick.magickpp"_dep;

    if (t.getSettings().TargetOS.is(OSType::Windows))
    {
        t += "ws2_32.lib"_slib;
        t += "advapi32.lib"_slib;
    }
}
