void build(Solution &s)
{
    auto &p = s.addTarget<LibraryTarget>("gnzlbg.static_vector", "master");
    p += Git("https://github.com/gnzlbg/static_vector");
}
