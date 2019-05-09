#pragma sw header on

static void gen_ragel(const DependencyPtr &ragel, NativeExecutedTarget &t, const path &in)
{
    auto c = t.addCommand();
    c << cmd::prog(ragel)
        << cmd::in(in)
        << cmd::out(in.filename() += ".cpp", cmd::Prefix{ "-o" });
}

#pragma sw header off

void build(Solution &s)
{
    auto &ragel = s.addTarget<ExecutableTarget>("ragel", "6.10.0");
    ragel += RemoteFile("https://www.colm.net/files/ragel/ragel-{M}.{m}.tar.gz");

    ragel += "aapl/.*"_rr;
    ragel += "ragel/.*\\.cpp"_rr;
    ragel += "ragel/.*\\.h"_rr;
    ragel += "aapl"_idir;

    ragel.writeFileOnce(ragel.BinaryPrivateDir / "config.h");
    if (ragel.getSettings().TargetOS.Type == OSType::Windows)
        ragel.writeFileOnce(ragel.BinaryPrivateDir / "unistd.h");
}
