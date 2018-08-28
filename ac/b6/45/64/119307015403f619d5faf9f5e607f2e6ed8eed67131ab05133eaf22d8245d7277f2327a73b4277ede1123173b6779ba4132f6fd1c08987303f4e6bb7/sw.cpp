#ifdef SW_PRAGMA_HEADER
#pragma sw header on

void gen_ragel(NativeExecutedTarget &t, const path &in)
{
    auto ragel = THIS_PACKAGE_DEPENDENCY;
    auto d = t + ragel;
    d->Dummy = true;

    auto o = t.BinaryDir / (in.filename().u8string() + ".cpp");

    auto c = std::make_shared<Command>();
    c->fs = t.getSolution()->fs;
    c->setProgram(ragel);
    c->args.push_back((t.SourceDir / in).u8string());
    c->args.push_back("-o");
    c->args.push_back(o.u8string());
    c->addInput(t.SourceDir / in);
    c->addOutput(o);
    t += o;
    t.Storage.push_back(c);
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
