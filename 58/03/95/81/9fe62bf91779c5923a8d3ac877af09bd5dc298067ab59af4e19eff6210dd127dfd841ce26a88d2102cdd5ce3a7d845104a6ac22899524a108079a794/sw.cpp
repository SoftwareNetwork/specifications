void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("dpilger26.NumCpp", "2.14.2");
    t += Git("https://github.com/dpilger26/NumCpp", "Version_{v}");
}
