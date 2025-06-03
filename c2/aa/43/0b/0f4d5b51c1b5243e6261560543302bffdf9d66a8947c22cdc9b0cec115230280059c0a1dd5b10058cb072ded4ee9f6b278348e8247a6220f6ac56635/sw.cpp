void build(Solution &s)
{
    auto &t = s.addTarget<Library>("ArashPartow.exprtk", "master");
    t += Git("https://github.com/ArashPartow/exprtk");
    t += "exprtk.hpp";
}
