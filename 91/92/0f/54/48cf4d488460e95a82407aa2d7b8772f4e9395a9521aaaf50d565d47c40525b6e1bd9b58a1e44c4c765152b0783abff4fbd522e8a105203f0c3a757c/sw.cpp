void build(Solution &s)
{
    auto &t = s.addStaticLibrary("Arryboom.fmemopen_windows", "1.1");
    t += Git("https://github.com/Arryboom/fmemopen_windows", "{M}.{m}");
    t += "libfmemopen.[hc]"_r;
    //t.pushFrontToFileOnce("libfmemopen.c", "#include \"libfmemopen.h\"");
    //t.pushFrontToFileOnce("libfmemopen.h", "#include <stdio.h>");
}
