void build(Solution &s)
{
    auto &p = s.addLibrary("martinmoene.gsl_lite", "0.33.0");
    p += Git("https://github.com/martinmoene/gsl-lite", "v{v}");
}
