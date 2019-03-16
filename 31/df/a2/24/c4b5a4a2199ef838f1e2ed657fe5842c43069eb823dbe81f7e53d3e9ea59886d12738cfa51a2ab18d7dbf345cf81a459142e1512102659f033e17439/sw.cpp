void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.gsl_lite", "0.33.0");
    t += Git("https://github.com/martinmoene/gsl-lite", "v{v}");
}
