void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &ragel = s.addTarget<ExecutableTarget>("ragel", "6.10.0");
    ragel.Source = RemoteFile("https://www.colm.net/files/ragel/ragel-6.10.tar.gz");

    ragel +=
        "aapl/.*\\.h"_rr,
        "ragel/.*\\.cpp"_rr,
        "ragel/.*\\.h"_rr;

    ragel.Private +=
        "aapl"_id;

    ragel.writeFileOnce("config.h", "", true);
    if (ragel.Settings.TargetOS.Type == OSType::Windows)
        ragel.writeFileOnce("unistd.h", "", true);
    ragel.replaceInFileOnce("ragel/rbxgoto.cpp", "rbxGoto\\(ret, \"_out\"\\)", "rbxGoto(ret, \"_out\").rdbuf()");
}
