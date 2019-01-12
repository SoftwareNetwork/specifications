void build(Solution &s)
{
    auto &dtl = s.addTarget<LibraryTarget>("cubicdaiya.dtl", "master");
    dtl += Git("https://github.com/cubicdaiya/dtl", "", "{v}");
    dtl += "dtl/.*"_rr;
}
