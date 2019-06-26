void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("p_ranav.pprint", "master");
    t += Git("https://github.com/p-ranav/pprint");
}
