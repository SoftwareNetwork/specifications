void build(Solution &s)
{
    auto &t = s.addLibrary("optimad.bitpit", "master");
    t += Git("https://github.com/optimad/bitpit");
}
