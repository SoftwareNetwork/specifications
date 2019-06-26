void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("madplotlib.madplotlib", "master");
    t += Git("https://github.com/madplotlib/madplotlib");
    t += "Madplotlib.h";
}
