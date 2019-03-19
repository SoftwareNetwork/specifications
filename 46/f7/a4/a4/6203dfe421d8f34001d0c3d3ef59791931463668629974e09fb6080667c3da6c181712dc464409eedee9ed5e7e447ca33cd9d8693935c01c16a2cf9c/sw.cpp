void build(Solution &s)
{
    auto &t = s.addTarget<Library>("leo_yuriev.t1ha", "2.1.1");
    t += Git("https://github.com/leo-yuriev/t1ha", "v{v}");

    t.ApiName = "T1HA_API";

    t += "src/.*"_rr;
    t += "t1ha.h";
}
