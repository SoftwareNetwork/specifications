void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("gnzlbg.static_vector", "master");
    t += Git("https://github.com/gnzlbg/static_vector");
}
