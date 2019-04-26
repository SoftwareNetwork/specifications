void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("RaftLib.RaftLib", "master");
    t += Git("https://github.com/RaftLib/RaftLib");

    t += "raftinc/.*"_rr;
    t += "src/.*"_rr;

    t.Public += "raftinc"_id;
}
