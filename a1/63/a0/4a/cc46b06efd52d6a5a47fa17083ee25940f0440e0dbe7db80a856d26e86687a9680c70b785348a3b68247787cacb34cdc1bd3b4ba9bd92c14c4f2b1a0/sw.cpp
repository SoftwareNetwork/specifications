void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("onqtam.doctest", "2.3.0");
    t += Git("https://github.com/onqtam/doctest");
    t += "doctest/.*"_rr;
}
