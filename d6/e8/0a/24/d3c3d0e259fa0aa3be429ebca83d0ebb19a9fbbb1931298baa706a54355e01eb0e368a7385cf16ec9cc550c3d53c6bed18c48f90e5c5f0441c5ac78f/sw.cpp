void build(Solution &s)
{
    auto &junction = s.addTarget<StaticLibraryTarget>("preshing.junction", "master");
    junction += Git("https://github.com/preshing/junction", "", "{b}");

    junction +=
        "cmake/junction_config.h.in",
        "junction/.*"_rr;

    junction.Public +=
        "."_id;

    junction.Public += "org.sw.demo.preshing.turf-master"_dep;

    junction.Variables["JUNCTION_TRACK_GRAMPA_STATS"] = "FALSE";
    junction.Variables["JUNCTION_USE_STRIPING"] = "TRUE";

    junction.configureFile("cmake/junction_config.h.in", "junction_config.h");
    junction.writeFileOnce("junction_userconfig.h");
}
