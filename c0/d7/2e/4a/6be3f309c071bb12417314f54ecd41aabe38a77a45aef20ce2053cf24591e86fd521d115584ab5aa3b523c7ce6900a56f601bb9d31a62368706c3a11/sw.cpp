void build(Solution &s)
{
    auto &t = s.addStaticLibrary("google.highway", "0.15.0");
    t += Git("https://github.com/google/highway");

    t -= "hwy/.*"_rr;
    t += "hwy/aligned_allocator.cc";
    t += "hwy/nanobenchmark.cc";
    t += "hwy/targets.cc";
}
