void build(Solution &s)
{
    auto &wdk = s.addProject("microsoft.windows.driver_frameworks", "10.0.22621.1");
    wdk += Git("https://github.com/microsoft/Windows-Driver-Frameworks", "win-{v}");

    auto &shared = wdk.addStaticLibrary("shared");
    {
        auto &t = shared;
        t += "src/framework/shared/.*"_rr;
        //t -= "src/framework/shared/.*/km/.*"_rr;
        //t -= "src/framework/shared/.*/um/.*"_rr;
        t.Public += "_AMD64_"_def;
        t +=
            "src/framework/shared/core"_idir,
            "src/framework/shared/core/km"_idir,
            "src/framework/shared/core/um"_idir,
            "src/framework/shared/enhancedverif"_idir,
            "src/framework/shared/enhancedverif/km"_idir,
            "src/framework/shared/enhancedverif/um"_idir,
            "src/framework/shared/inc"_idir,
            "src/framework/shared/inc/primitives"_idir,
            "src/framework/shared/inc/primitives/common"_idir,
            "src/framework/shared/inc/primitives/km"_idir,
            "src/framework/shared/inc/primitives/um"_idir,
            "src/framework/shared/inc/private"_idir,
            "src/framework/shared/inc/private/common"_idir,
            "src/framework/shared/inc/private/km"_idir,
            "src/framework/shared/inc/private/um"_idir,
            "src/framework/shared/irphandlers"_idir,
            "src/framework/shared/irphandlers/default"_idir,
            "src/framework/shared/irphandlers/general"_idir,
            "src/framework/shared/irphandlers/io"_idir,
            "src/framework/shared/irphandlers/io/km"_idir,
            "src/framework/shared/irphandlers/package"_idir,
            "src/framework/shared/irphandlers/pnp"_idir,
            "src/framework/shared/irphandlers/pnp/km"_idir,
            "src/framework/shared/irphandlers/pnp/um"_idir,
            "src/framework/shared/object"_idir,
            "src/framework/shared/object/km"_idir,
            "src/framework/shared/object/um"_idir,
            "src/framework/shared/primitives"_idir,
            "src/framework/shared/primitives/km"_idir,
            "src/framework/shared/primitives/um"_idir,
            "src/framework/shared/support"_idir,
            "src/framework/shared/support/km"_idir,
            "src/framework/shared/support/um"_idir,
            "src/framework/shared/targets"_idir,
            "src/framework/shared/targets/general"_idir,
            "src/framework/shared/targets/general/km"_idir,
            "src/framework/shared/targets/general/um"_idir,
            "src/framework/shared/targets/usb"_idir,
            "src/framework/shared/targets/usb/km"_idir,
            "src/framework/shared/targets/usb/um"_idir
            ;
        //t += "com.Microsoft.Windows.SDK.km"_dep;
        t.writeFileOnce("wdmdefs.h");
    }


/*#if (FX_CORE_MODE == FX_CORE_USER_MODE)
#include "MxUm.h"
#elif (FX_CORE_MODE == FX_CORE_KERNEL_MODE)
#include "MxKm.h"*/


    /*auto &kmdf = wdk.addStaticLibrary("kmdf");
    {
        auto &t = kmdf;
        t += "src/framework/kmdf/.*"_rr;
        t += "_AMD64_"_def;
        t.Public += "src/framework/kmdf/inc/private"_idir;
        t += "com.Microsoft.Windows.SDK.km"_dep;
    }*/
}
