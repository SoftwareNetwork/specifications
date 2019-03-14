void build(Solution &s)
{
    auto &p = s.addLibrary("fastflow.fastflow", "2.2.0");
    p += Git("https://github.com/fastflow/fastflow", "{v}");
    p += "ff/.*"_rr;
}
