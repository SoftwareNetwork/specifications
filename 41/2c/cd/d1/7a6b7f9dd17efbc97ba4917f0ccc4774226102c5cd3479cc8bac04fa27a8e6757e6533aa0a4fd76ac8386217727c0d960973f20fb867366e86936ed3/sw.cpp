void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("mavam.libbf", "master");
    t += Git("https://github.com/mavam/libbf");

    t += "bf/.*"_rr;
    t.Public += "."_id;
    t += "src/.*"_rr;
    t.pushFrontToFileOnce("src/bitvector.cpp", "#include <algorithm>");
}
