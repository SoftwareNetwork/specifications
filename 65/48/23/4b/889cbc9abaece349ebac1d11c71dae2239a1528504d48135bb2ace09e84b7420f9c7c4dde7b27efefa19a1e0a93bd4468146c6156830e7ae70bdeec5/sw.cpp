void build(Solution &s)
{
    auto &t = s.addLibrary("Snaipe.fmem", "master");
    t += Git("https://github.com/Snaipe/fmem");

    t.ApiName = "FMEM_API";

    t += "include/.*"_rr;
    t -= "src/.*"_rr;
    t += "src/alloc.c";
    if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
        t += "src/fmem-winapi-tmpfile.c";
    else
        t += "src/fmem-tmpfile.c";

    t.configureFile("include/fmem.h.in", "fmem.h");
}
