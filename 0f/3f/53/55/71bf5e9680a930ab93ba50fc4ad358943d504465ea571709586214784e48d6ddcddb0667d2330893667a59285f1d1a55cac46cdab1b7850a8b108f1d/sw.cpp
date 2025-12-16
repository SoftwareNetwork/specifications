void build(Solution &s)
{
    auto &double_conversion = s.addTarget<StaticLibraryTarget>("google.double_conversion", "3.4.0");
    double_conversion += Git("https://github.com/google/double-conversion", "v{v}");
    double_conversion += "double-conversion/.*"_rr;
}
