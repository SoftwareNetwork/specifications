void build(Solution &s)
{
    auto &t = s.addLibrary("google.highway", "1.0.7");
    t += Git("https://github.com/google/highway");

    t += cpp17;
    t -= "hwy/.*"_rr;
    t += "hwy/aligned_allocator.cc";
    //t += "hwy/nanobenchmark.cc";
    t += "hwy/targets.cc";
    t += "hwy/per_target.cc";

    t += "HWY_DLLEXPORT"_api;

    t.Public += sw::Shared, "HWY_SHARED_DEFINE"_def;
    /*t += sw::Shared, "hwy_EXPORTS"_def;
    t += sw::Shared, "hwy_contrib_EXPORTS"_def;*/
}
