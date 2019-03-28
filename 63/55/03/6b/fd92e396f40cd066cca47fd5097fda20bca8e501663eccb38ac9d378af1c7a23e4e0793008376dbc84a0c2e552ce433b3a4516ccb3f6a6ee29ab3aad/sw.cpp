void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("joboccara.NamedType", "master");
    t += Git("https://github.com/joboccara/NamedType");
    t += ".*\\.hpp"_rr;
    t ^= "catch.hpp";
}
