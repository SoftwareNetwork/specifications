void build(Solution &s)
{
    auto &eigen = s.addTarget<LibraryTarget>("eigen", "5.0.1");
    eigen += Git("https://gitlab.com/libeigen/eigen");

    eigen +=
        "Eigen/.*"_rr,
        "unsupported/Eigen/.*"_rr;
}
