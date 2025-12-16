void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("dpilger26.NumCpp", "2.16.0");
    t += Git("https://github.com/dpilger26/NumCpp", "Version_{v}");
}
