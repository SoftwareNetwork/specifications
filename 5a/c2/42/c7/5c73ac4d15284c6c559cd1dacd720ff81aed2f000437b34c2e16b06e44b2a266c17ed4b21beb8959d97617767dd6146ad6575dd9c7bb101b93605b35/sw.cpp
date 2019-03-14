void build(Solution &s)
{
    auto &jpeg = s.addTarget<LibraryTarget>("jpeg", "9.3.0");
    jpeg += RemoteFile("http://www.ijg.org/files/jpegsr9c.zip");
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

    if (s.Settings.Native.CompilerType == CompilerType::MSVC)
        jpeg.configureFile("jconfig.vc", "jconfig.h");
    else
        jpeg.configureFile("jconfig.cfg", "jconfig.h");

    jpeg.replaceInFileOnce("jmorecfg.h",
        "extern type",
        "extern JPEG_API type");
}
