void build(Solution &s)
{
    auto &t = s.addTarget<Library>("ampl.mp", "3.1.0");
    t += Git("https://github.com/ampl/mp");
}
