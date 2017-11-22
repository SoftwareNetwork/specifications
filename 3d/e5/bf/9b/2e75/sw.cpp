void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &libharu = s.addTarget<LibraryTarget>("libharu", "2.3.0");
    libharu.Source = Git("https://github.com/libharu/libharu", "RELEASE_{M}_{m}_{p}");

    libharu.setChecks("libharu");

    libharu +=
        "include/.*"_rr,
        "src/.*"_rr;

    libharu.Interface += sw::Shared, "HPDF_DLL_CDECL"_d;
    libharu.Private += sw::Shared, "HPDF_DLL_MAKE_CDECL"_d;

    libharu.Public += "pub.cppan2.demo.png-1"_dep;

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
    s.checkLibraryFunctionExists("z", "deflate");
    s.checkLibraryFunctionExists("m", "floor");
    s.checkLibraryFunctionExists("png", "png_init_io");
}
