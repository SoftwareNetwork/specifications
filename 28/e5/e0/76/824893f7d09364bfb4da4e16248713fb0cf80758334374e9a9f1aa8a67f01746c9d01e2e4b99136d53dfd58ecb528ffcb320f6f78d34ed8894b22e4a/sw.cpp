void build(Solution &s)
{
    auto &gsl = s.addTarget<LibraryTarget>("microsoft.gsl", "4.1.0");
    gsl += Git("https://github.com/Microsoft/GSL", "v{v}");
    gsl += "include/.*"_rr;
}
