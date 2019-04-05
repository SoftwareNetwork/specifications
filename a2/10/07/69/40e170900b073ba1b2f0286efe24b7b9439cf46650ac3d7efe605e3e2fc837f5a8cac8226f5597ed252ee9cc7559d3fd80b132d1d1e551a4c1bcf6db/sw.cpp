void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("vincentlaucsb.csv_parser", "1.1.3");
    t += Git("https://github.com/vincentlaucsb/csv-parser");

    t += "include/.*"_rr;
}
