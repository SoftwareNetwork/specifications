void build(Solution &s)
{
    auto &hwloc = s.addTarget<LibraryTarget>("open_mpi.hwloc", "2.0.2");
    hwloc += Git("https://github.com/open-mpi/hwloc", "hwloc-{v}");

    hwloc +=
        "contrib/windows/.*\\.h"_rr,
        "hwloc/.*\\.c"_rr,
        "include/.*\\.h"_rr,
        "include/.*\\.h.in"_rr;

    hwloc -= "hwloc/topology-.*"_rr;
    hwloc += "hwloc/topology-noos.c";
    hwloc += "hwloc/topology-synthetic.c";
    hwloc += "hwloc/topology-x86.c";
    hwloc += "hwloc/topology-xml.*"_rr;

    hwloc.Public +=
        "include"_id;

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        hwloc += "hwloc/topology-windows.c";
        hwloc.Private += "DECLSPEC_EXPORTS"_d;
        hwloc.Public += "_USRDLL=_WINDLL"_d;
        hwloc.configureFile("contrib/windows/hwloc_config.h", "hwloc/autogen/config.h");
        hwloc.configureFile("contrib/windows/private_config.h", "private/autogen/config.h");
        hwloc.configureFile("contrib/windows/static-components.h", "static-components.h");
    }

    hwloc.Public += "org.sw.demo.libxml2-2"_dep;
}
