#ifdef SW_PRAGMA_HEADER
#pragma sw header on

void gen_ragel(NativeExecutedTarget &t, const path &in)
{
    auto ragel = THIS_PACKAGE_DEPENDENCY;

    auto c = t.addCommand();
    c << cmd::prog(ragel)
        << cmd::in(in)
        << cmd::out(in.filename() += ".cpp", cmd::Prefix{ "-o" });
}

#pragma sw header off
#endif

void build(Solution &s)
{
    auto &ragel = s.addTarget<ExecutableTarget>("ragel", "6.10.0");
    ragel += RemoteFile("https://www.colm.net/files/ragel/ragel-{M}.{m}.tar.gz");

    ragel += "aapl/.*"_rr;
    ragel += "ragel/.*\\.cpp"_rr;
    ragel += "ragel/.*\\.h"_rr;
    ragel += "aapl"_idir;

    ragel.writeFileOnce(ragel.BinaryPrivateDir / "config.h");
    if (s.Settings.TargetOS.Type == OSType::Windows)
        ragel.writeFileOnce(ragel.BinaryPrivateDir / "unistd.h");
}
