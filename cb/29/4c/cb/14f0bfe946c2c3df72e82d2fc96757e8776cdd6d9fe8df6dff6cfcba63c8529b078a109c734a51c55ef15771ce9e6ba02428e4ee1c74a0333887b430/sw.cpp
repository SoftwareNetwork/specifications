void build(Solution &s)
{
    auto &p = s.addStaticLibrary("CGAL.cgal", "6.0.0");
    p += Git("https://github.com/CGAL/cgal", "v{M}.{m}{po}");

    p.Public += sw::Shared, "CGAL_BUILD_SHARED_LIBS"_def;
    p += sw::Shared, "CGAL_Core_EXPORTS"_def;
    p += "CGAL_HEADER_ONLY"_def;

    auto add_dir = [&p](const String &name) -> decltype(auto)
    {
        //auto &t = p.addLibrary(name);
        //t.setRootDirectory(name);
        p.AllowEmptyRegexes = true;
        p += FileRegex(path(name) / "include", ".*", true);
        p += FileRegex(path(name) / "src", ".*", true);
        p.AllowEmptyRegexes = false;
        p.Public += IncludeDirectory(name + "/include");
        return p;
    };

    auto &i = add_dir("Installation");
    {
        i.Public += "org.sw.demo.boost.config"_dep;
        i.Public += "org.sw.demo.boost.predef"_dep;
        i.Public += "org.sw.demo.boost.lexical_cast"_dep;
        i.Public += "org.sw.demo.gnu.gmp.lib"_dep;
        i.Public += "org.sw.demo.gnu.mpfr"_dep;

        // remove Installation for separate libs
        i.patch("Installation/include/CGAL/Installation/internal/enable_third_party_libraries.h",
            "! __has_include(<gmp.h>)",
            "0//! __has_include(<gmp.h>)"
        );
        i.patch("Installation/include/CGAL/Installation/internal/enable_third_party_libraries.h",
            "! __has_include(<mpfr.h>)",
            "0//! __has_include(<mpfr.h>)"
        );
    }

    //auto &stl = add_dir("STL_Extension");
    //auto &k = add_dir("Kernel_23");
    //auto &c = add_dir("CGAL_Core");
    //(stl + i)->IncludeDirectoriesOnly = true;
    //(stl + c)->IncludeDirectoriesOnly = true;
    //c += i, stl, k;

    StringSet dirs;
    for (auto &d : fs::directory_iterator(p.SourceDir))
    {
        if (!d.is_directory())
            continue;
        auto s = d.path().filename().string();
        if (s.empty() || s[0] == '.')
            continue;
        if (!fs::exists(d.path() / "include"))
            continue;
        dirs.insert(s);
    }
    for (auto &d : dirs)
        add_dir(d);
}
