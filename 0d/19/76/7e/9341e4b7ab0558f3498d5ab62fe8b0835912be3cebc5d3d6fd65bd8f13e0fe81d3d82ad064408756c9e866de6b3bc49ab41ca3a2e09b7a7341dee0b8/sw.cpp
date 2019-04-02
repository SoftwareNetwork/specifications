void build(Solution &s)
{
    auto &p = s.addTarget<Library>("nuspell", "2.2.0");
    p += Git("https://github.com/nuspell/nuspell", "v{v}");

    p += "src/parsers/.*"_rr;
    p += "src/hunspell/.*"_rr;
    p += "src/nuspell/.*"_rr;
    p -= "src/nuspell/main.cxx";

    p += "BUILDING_LIBHUNSPELL"_def;
    p += sw::Static, "HUNSPELL_STATIC"_def;

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        p += "NOMINMAX"_def;
        p += "WIN32_LEAN_AND_MEAN"_def;
    }

    p += "org.sw.demo.boost.algorithm"_dep;
    p += "org.sw.demo.boost.container"_dep;
    p += "org.sw.demo.boost.multi_index"_dep;
    p += "org.sw.demo.boost.utility"_dep;
    p += "org.sw.demo.unicode.icu.i18n"_dep;
}
