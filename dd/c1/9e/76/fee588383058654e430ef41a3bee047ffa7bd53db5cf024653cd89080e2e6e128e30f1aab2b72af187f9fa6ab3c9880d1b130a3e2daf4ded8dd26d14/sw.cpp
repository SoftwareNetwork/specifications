void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("sgorsten.linalg", "master");
    t += Git("https://github.com/sgorsten/linalg");
}
