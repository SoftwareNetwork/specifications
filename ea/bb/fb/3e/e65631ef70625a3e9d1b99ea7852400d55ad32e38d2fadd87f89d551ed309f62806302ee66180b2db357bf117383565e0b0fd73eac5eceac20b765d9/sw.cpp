void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("bloomen.transwarp", "master");
    t += Git("https://github.com/bloomen/transwarp");
}
