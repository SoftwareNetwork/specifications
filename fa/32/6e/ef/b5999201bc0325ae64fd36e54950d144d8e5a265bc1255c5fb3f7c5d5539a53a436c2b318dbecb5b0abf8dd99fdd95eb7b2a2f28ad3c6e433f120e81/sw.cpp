void build(Solution &s)
{
    auto &hwloc = s.addTarget<LibraryTarget>("open_mpi.hwloc", "2.11.0");
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

    if (hwloc.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        hwloc += "hwloc/topology-windows.c";
        hwloc.Private += "DECLSPEC_EXPORTS"_d;
        hwloc.Public += "_USRDLL=_WINDLL"_d;
        hwloc.configureFile("contrib/windows/hwloc_config.h", "hwloc/autogen/config.h");
        hwloc.configureFile("contrib/windows/private_config.h", "private/autogen/config.h");
    }
    else
    {
        hwloc.ApiName = "HWLOC_DECLSPEC";

        /*hwloc.writeFileOnce("private/autogen/config.h", R"(
#pragma once

#include <stdint.h>
)");*/

        hwloc.writeFileOnce("hwloc/autogen/config.h", R"(
#pragma once

#define HWLOC_VERSION "2.3.0"
#define HWLOC_VERSION_MAJOR 2
#define HWLOC_VERSION_MINOR 3
#define HWLOC_VERSION_RELEASE 0
#define HWLOC_VERSION_GREEK ""

#define __hwloc_restrict
#define __hwloc_inline __inline

#define __hwloc_attribute_unused
#define __hwloc_attribute_malloc
#define __hwloc_attribute_const
#define __hwloc_attribute_pure
#define __hwloc_attribute_deprecated
#define __hwloc_attribute_may_alias
#define __hwloc_attribute_warn_unused_result

#define hwloc_pid_t pid_t
#define hwloc_thread_t pthread_t

#include <stdint.h>
typedef uint64_t hwloc_uint64_t;

/* Whether we need to re-define all the hwloc public symbols or not */
#define HWLOC_SYM_TRANSFORM 0

/* The hwloc symbol prefix */
#define HWLOC_SYM_PREFIX hwloc_

/* The hwloc symbol prefix in all caps */
#define HWLOC_SYM_PREFIX_CAPS HWLOC_
)");
    }

    hwloc.Public += "org.sw.demo.xmlsoft.libxml2"_dep;

    // common
    hwloc.configureFile("contrib/windows/static-components.h", "static-components.h");
}
