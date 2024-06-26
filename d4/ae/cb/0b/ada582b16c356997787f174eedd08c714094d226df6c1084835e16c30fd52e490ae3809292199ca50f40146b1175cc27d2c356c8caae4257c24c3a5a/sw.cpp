void build(Solution &s)
{
    auto &jpeg = s.addTarget<LibraryTarget>("jpeg", "9.6.0");
    auto url = "https://www.ijg.org/files/jpegsr9"s;
    url += 'a' + jpeg.getPackage().getVersion().getMinor() - 1;
    url += ".zip";
    jpeg += RemoteFile(url);

    jpeg.ApiName = "JPEG_API";

    jpeg +=
        ".*\\.h"_rr,
        "jaricom.c",
        "jcapimin.c",
        "jcapistd.c",
        "jcarith.c",
        "jccoefct.c",
        "jccolor.c",
        "jcdctmgr.c",
        "jchuff.c",
        "jcinit.c",
        "jcmainct.c",
        "jcmarker.c",
        "jcmaster.c",
        "jcomapi.c",
        "jconfig.cfg",
        "jconfig.vc",
        "jcparam.c",
        "jcprepct.c",
        "jcsample.c",
        "jctrans.c",
        "jdapimin.c",
        "jdapistd.c",
        "jdarith.c",
        "jdatadst.c",
        "jdatasrc.c",
        "jdcoefct.c",
        "jdcolor.c",
        "jddctmgr.c",
        "jdhuff.c",
        "jdinput.c",
        "jdmainct.c",
        "jdmarker.c",
        "jdmaster.c",
        "jdmerge.c",
        "jdpostct.c",
        "jdsample.c",
        "jdtrans.c",
        "jerror.c",
        "jfdctflt.c",
        "jfdctfst.c",
        "jfdctint.c",
        "jidctflt.c",
        "jidctfst.c",
        "jidctint.c",
        "jmemmgr.c",
        "jmemnobs.c",
        "jquant1.c",
        "jquant2.c",
        "jutils.c",
        "transupp.c";

    jpeg.setChecks("jpeg");
    jpeg.Variables["HAVE_PROTOTYPES"] = 1;
    jpeg.configureFile("jconfig.cfg", "jconfig.h", ConfigureFlags::EnableUndefReplacements);

    jpeg.pushFrontToFileOnce("jpeglib.h", R"(
#ifdef _WIN32
#include <windows.h>
#endif
)");

    jpeg.replaceInFileOnce("jmorecfg.h",
        "extern type",
        "extern JPEG_API type");
}

void check(Checker &c)
{
    auto &s = c.addSet("jpeg");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("stddef.h");
    s.checkTypeSize("unsigned char");
    s.checkTypeSize("unsigned short");
}
