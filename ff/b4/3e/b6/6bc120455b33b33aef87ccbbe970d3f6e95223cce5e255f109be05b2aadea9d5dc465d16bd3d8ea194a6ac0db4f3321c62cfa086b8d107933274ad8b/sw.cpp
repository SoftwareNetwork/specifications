void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("gurki.vivid", "master");
    t += Git("https://github.com/gurki/vivid");
}
