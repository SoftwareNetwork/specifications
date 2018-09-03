void build(Solution &s)
{
    auto &double_conversion = s.addTarget<StaticLibraryTarget>("google.double_conversion", "2.0.1");
    double_conversion += Git("https://github.com/google/double-conversion", "v{v}");
    double_conversion += "src/.*"_rr;
}
