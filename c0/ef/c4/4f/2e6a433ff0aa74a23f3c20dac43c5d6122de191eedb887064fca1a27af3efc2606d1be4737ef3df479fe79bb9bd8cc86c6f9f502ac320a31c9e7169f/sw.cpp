void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("google.tcmalloc", "master");
    t += Git("https://github.com/google/tcmalloc");

    t +=
        "tcmalloc/.*"_r,
        "tcmalloc/internal.*"_r;
    t -= ".*_test.cc"_rr;
}
