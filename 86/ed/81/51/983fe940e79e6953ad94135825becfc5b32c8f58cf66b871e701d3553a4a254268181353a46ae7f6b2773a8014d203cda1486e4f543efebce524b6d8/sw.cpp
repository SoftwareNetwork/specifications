void build(Solution &s)
{
    auto &eigen = s.addTarget<LibraryTarget>("eigen", "3.3.7");
    eigen += RemoteFile("https://bitbucket.org/eigen/eigen/get/{v}.tar.gz");

    eigen +=
        "Eigen/.*"_rr,
        "unsupported/Eigen/.*"_rr;
}
