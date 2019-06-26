void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("stxxl.stxxl", "master");
    t += Git("https://github.com/stxxl/stxxl");
}
