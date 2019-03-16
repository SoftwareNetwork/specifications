void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.gsl_lite", "master");
    t += Git("https://github.com/martinmoene/gsl-lite");
}
