void build(Solution &s)
{
    auto &gsl = s.addTarget<LibraryTarget>("microsoft.gsl", "5.0.1");
    gsl += Git("https://github.com/Microsoft/GSL", "v{v}");
    gsl += "include/.*"_rr;
}
