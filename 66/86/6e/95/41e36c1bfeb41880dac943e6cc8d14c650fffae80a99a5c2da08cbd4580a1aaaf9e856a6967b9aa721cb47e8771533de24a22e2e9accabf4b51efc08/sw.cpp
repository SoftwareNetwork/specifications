void build(Solution &s)
{
    auto &eigen = s.addTarget<LibraryTarget>("eigen", "3.3.9");
    eigen += Git("https://gitlab.com/libeigen/eigen");

    eigen +=
        "Eigen/.*"_rr,
        "unsupported/Eigen/.*"_rr;
}
