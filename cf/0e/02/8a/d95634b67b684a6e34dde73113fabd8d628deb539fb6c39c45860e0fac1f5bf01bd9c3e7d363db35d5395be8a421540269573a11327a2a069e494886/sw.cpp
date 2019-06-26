void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("eliaskosunen.scnlib", "master");
    t += Git("https://github.com/eliaskosunen/scnlib");
}
