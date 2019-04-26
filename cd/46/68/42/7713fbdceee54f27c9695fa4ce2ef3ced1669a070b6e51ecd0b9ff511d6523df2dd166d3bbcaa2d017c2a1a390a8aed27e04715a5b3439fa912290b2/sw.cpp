void build(Solution &s)
{
    auto &t = s.addTarget<Library>("LPeter1997.CppCmb", "master");
    t += Git("https://github.com/LPeter1997/CppCmb");
    t += "cppcmb.hpp";
}
