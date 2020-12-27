void build(Solution &s)
{
    auto &t = s.addStaticLibrary("Arryboom.fmemopen_windows", "master");
    t += Git("https://github.com/Arryboom/fmemopen_windows");
    t += "libfmemopen.[hc]"_r;
    t.pushFrontToFileOnce("libfmemopen.c", "#include \"libfmemopen.h\"");
    t.pushFrontToFileOnce("libfmemopen.h", "#include <stdio.h>");
}
