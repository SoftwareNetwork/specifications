// not working yet

#pragma sw require header org.sw.demo.lexxmark.winflexbison.bison

void build(Solution &s)
{
    auto &t = s.addTarget<Executable>("westes.flex", "2.6.4");
    t += Git("https://github.com/westes/flex", "v{v}");

    t += "src/.*"_rr;
    if (t.getBuildSettings().TargetOS.is(OSType::Windows))
    {
        t.writeFileOnce("unistd.h");
        t.writeFileOnce("sys/wait.h");
    }
    t += "org.sw.demo.gnu.grep.gnulib"_dep;
    gen_bison("org.sw.demo.lexxmark.winflexbison"_dep, t, "src/parse.y", t.BinaryDir / "parse.c", t.BinaryDir / "parse.h");
}
