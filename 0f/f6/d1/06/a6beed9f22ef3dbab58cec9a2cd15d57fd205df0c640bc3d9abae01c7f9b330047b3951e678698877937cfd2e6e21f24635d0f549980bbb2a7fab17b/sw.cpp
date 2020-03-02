// not working yet

/*static auto gen_flex(const DependencyPtr &flex, const DependencyPtr &m4, NativeExecutedTarget &t, FlexBisonData d)
{
    ...
}*/

void build(Solution &s)
{
    auto &t = s.addTarget<Executable>("westes.flex", "2.6.4");
    t += RemoteFile("https://github.com/westes/flex/releases/download/v{v}/flex-{v}.tar.gz");

    t += "src/.*"_rr;
    t -= "src/libmain.c";
    t -= "src/libyywrap.c";
    t -= "org.sw.demo.gnu.grep.gnulib"_dep;
    t -= "org.sw.demo.lexxmark.winflexbison.common"_dep;

    if (t.getBuildSettings().TargetOS.is(OSType::Windows))
    {
        t += "S_IXUSR=0000100"_def;
        t.writeFileOnce("unistd.h");
        t.writeFileOnce("sys/wait.h");
        t += "org.sw.demo.gnu.grep.gnulib"_dep;
        t += "org.sw.demo.lexxmark.winflexbison.common"_dep;
        t += "ws2_32.lib"_slib;
    }

    t.replaceInFileOnce("src/flexdef.h", "#include <libgen.h>", "//#include  <libgen.h>");
    t.replaceInFileOnce("src/flexdef.h", "#include <strings.h>", "//#include  <strings.h>");

    t += Definition("VERSION=\"" + t.getPackage().getVersion().toString() + "\"");
    t += "M4=\"\""_def;
}
