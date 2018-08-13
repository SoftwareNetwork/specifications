void build(Solution &s)
{
    auto &libharu = s.addTarget<LibraryTarget>("libharu", "2.3.0");
    libharu += Git("https://github.com/libharu/libharu", "RELEASE_2_3_0");

    libharu.setChecks("libharu");

    libharu +=
        "include/.*"_rr,
        "src/.*"_rr;

    libharu.Private +=
        "src"_id;

    libharu.Public +=
        "include"_id;

    libharu.Interface += sw::Shared, "HPDF_DLL_CDECL"_d;
    libharu.Private += sw::Shared, "HPDF_DLL_MAKE_CDECL"_d;

    libharu.Public += "org.sw.demo.glennrp.png-1"_dep;
    libharu.configureFile("include/hpdf_config.h.cmake", "hpdf_config.h");
}

void check(Checker &c)
{
    auto &s = c.addSet("libharu");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("unistd.h");
    s.checkTypeSize("size_t");
    s.checkTypeSize("void *");
    s.checkLibraryFunctionExists("m", "floor");
}
