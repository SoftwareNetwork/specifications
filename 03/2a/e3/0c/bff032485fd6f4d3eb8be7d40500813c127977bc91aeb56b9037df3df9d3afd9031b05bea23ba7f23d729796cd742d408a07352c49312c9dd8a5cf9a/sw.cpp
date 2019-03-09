void build(Solution &s)
{
    auto &cds = s.addTarget<LibraryTarget>("khizmax.cds", "2.3.3");
    cds += Git("https://github.com/khizmax/libcds", "v{v}");

    cds +=
        "cds/.*"_rr,
        "src/.*"_rr;

    cds += "CDS_BUILD_LIB"_d;
}
