void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &p = s.addProject("mgk25.jbig", "2.1.0");
    p.Source = RemoteFile("https://www.cl.cam.ac.uk/~mgk25/jbigkit/download/jbigkit-{M}.{m}.tar.gz");

    auto &ar = p.addTarget<StaticLibraryTarget>("ar");
    ar +=
        "libjbig/jbig_ar.c",
        "libjbig/jbig_ar.h";
    ar.Public +=
        "libjbig"_id;

    auto &jbig = p.addTarget<StaticLibraryTarget>("jbig");
    jbig +=
        "libjbig/jbig.c",
        "libjbig/jbig.h";
    jbig.Public +=
        "libjbig"_id;
    jbig.Public += ar;

    auto &jbig85 = p.addTarget<StaticLibraryTarget>("jbig85");
    jbig85 +=
        "libjbig/jbig85.c",
        "libjbig/jbig85.h";
    jbig85.Public +=
        "libjbig"_id;
    jbig85.Public += ar;
}
