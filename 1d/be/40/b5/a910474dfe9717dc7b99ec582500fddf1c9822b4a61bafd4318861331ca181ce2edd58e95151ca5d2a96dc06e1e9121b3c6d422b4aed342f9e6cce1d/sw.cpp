void build(Solution &s)
{
    auto &t = s.addLibrary("xorg.pciaccess", "0.16.0");
    t += Git("https://gitlab.freedesktop.org/xorg/lib/libpciaccess", "libpciaccess-0.16");

    t.setChecks("x", true);

    t.ExportAllSymbols = true;

    t += "include/.*"_rr;
    t += "src/.*"_rr;

    t -= "src/freebsd_pci.c";
    t -= "src/netbsd_pci.c";
    t -= "src/hurd_pci.c";
    t -= "src/openbsd_pci.c";
    t -= "src/solx_devfs.c";
    t -= "src/x86_pci.c"; // cygwin

    t -= "src/common_vgaarb_stub.c";

    t += "PCIIDS_PATH=\".\""_def;

    t += "HAVE_CONFIG_H"_def;
    t.writeFileOnce(t.BinaryPrivateDir / "config.h", R"(
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
    )");

    t.LinkOptions.push_back("--no-undefined");
}

void check(Checker &c)
{
    auto &s = c.addSet("x");
    s.checkIncludeExists("err.h");
    s.checkIncludeExists("machine/sysarch.h");
    s.checkIncludeExists("machine/mtrr.h");
    s.checkIncludeExists("asm/mtrr.h");

    s.checkIncludeExists("string.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("stddef.h");
}
