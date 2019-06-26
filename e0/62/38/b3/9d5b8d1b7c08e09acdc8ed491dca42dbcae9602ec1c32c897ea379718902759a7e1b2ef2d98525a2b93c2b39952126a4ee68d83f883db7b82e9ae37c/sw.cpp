void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("tcbrindle.numeris_romanis", "master");
    t += Git("https://github.com/tcbrindle/numeris_romanis");
    t += "numeris_romanis.hpp";
}
