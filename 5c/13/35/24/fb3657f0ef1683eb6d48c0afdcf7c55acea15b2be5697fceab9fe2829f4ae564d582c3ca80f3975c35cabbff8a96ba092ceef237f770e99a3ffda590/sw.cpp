void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("ianf1.rngpp", "master");
    t += Git("https://gitlab.com/ianf1/rngpp");
}
