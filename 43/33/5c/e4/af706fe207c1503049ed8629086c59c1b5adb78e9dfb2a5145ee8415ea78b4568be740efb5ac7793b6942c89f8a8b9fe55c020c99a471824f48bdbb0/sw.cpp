void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("LIONant.properties", "master");
    t += Git("https://gitlab.com/LIONant/properties");
}
