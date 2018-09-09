void build(Solution &s)
{
    auto &mng = s.addTarget<LibraryTarget>("mng", "2.0.3");
    mng += RemoteFile("https://netix.dl.sourceforge.net/project/libmng/libmng-devel/{v}/libmng-{v}.tar.xz");

    mng += "[^/]*\\.[hc]"_rr;
    mng.removeFile("config.h");

    mng.Public += "MNG_VERSION_DLL=1"_d;
    Definition d;
    d.d = "MNG_VERSION_MAJOR=" + mng.Variables["PACKAGE_VERSION_MAJOR"].toString();
    mng.Public += d;
    d.d = "MNG_VERSION_MINOR=" + mng.Variables["PACKAGE_VERSION_MINOR"].toString();
    mng.Public += d;
    d.d = "MNG_VERSION_RELEASE=" + mng.Variables["PACKAGE_VERSION_PATCH"].toString();
    mng.Public += d;
    mng.Public += "MNG_VERSION_SO=1"_d;
    mng.Interface += sw::Shared, "MNG_USE_DLL"_d;
    mng.Private += sw::Shared, "MNG_BUILD_DLL"_d;

    mng.Public += "org.sw.demo.jpeg-9"_dep;
    mng.Public += "org.sw.demo.madler.zlib-1"_dep;

    mng.writeFileOnce(mng.BinaryPrivateDir / "config.h");
    mng.replaceInFileOnce("libmng_types.h", "__stdcall", "");
}
