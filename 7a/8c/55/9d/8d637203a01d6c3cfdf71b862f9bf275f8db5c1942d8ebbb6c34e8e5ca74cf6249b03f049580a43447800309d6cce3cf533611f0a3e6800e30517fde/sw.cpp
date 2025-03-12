void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("strukturag.libheif", "1.19.7");
    t += Git("https://github.com/strukturag/libheif", "v{v}");

    t += "libheif/.*"_rr;

    t += "libheif"_idir;
    t.Public += "libheif/api"_idir;

    t.Public += sw::Static, "LIBHEIF_STATIC_BUILD"_def;
    t += sw::Shared, "LIBHEIF_EXPORTS"_def;
    t += "HAVE_VISIBILITY"_def;

    t.Variables["PROJECT_VERSION_MAJOR"] = t.Variables["PACKAGE_VERSION_MAJOR"];
    t.Variables["PROJECT_VERSION_MINOR"] = t.Variables["PACKAGE_VERSION_MINOR"];
    t.Variables["PROJECT_VERSION_PATCH"] = t.Variables["PACKAGE_VERSION_PATCH"];
    t.Variables["PROJECT_VERSION_TWEAK"] = t.Variables["PACKAGE_VERSION_TWEAK"];

    t.configureFile("libheif/api/libheif/heif_version.h.in", "libheif/heif_version.h");
}

void check(Checker &c)
{
    auto &s = c.addSet("jas");
    s.checkIncludeExists("io.h");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("sys/time.h");
    s.checkTypeSize("int");
    s.checkTypeSize("long");
    s.checkTypeSize("long long");
    s.checkTypeSize("size_t");
    s.checkTypeSize("ssize_t");

    for (auto &check : s.all)
        check->Prefixes.insert("JAS_");
}
