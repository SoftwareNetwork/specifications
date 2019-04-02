void build(Solution &s)
{
    auto &p = s.addTarget<Library>("shibatch.sleef", "3.3.1");
    p += Git("https://github.com/shibatch/sleef");

    p += "include/.*"_rr;
    p += "src/arch/.*"_rr;
    p += "src/common/.*"_rr;
    p += "src/dft/.*"_rr;
}
