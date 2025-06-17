void build(Solution &s)
{
    auto &t = s.addLibrary("assimp", "6.0.2");
    t += Git("https://github.com/assimp/assimp", "v{v}");

    t += cpp17;
    t += "include/.*"_rr;
    t += "code/.*"_rr;
    t += "contrib/clipper/.*"_rr;
    t += "contrib/zip/src/.*"_rr;
    t += "contrib/poly2tri/poly2tri/.*"_rr;
    t -= "code/AssetLib/IFC/IFCReaderGen_4.cpp";

    t.Public += "include"_idir;
    t += "code"_idir;
    //t += "code/Step"_idir;
    t += "."_idir;

    t += "ASSIMP_BUILD_NO_C4D_IMPORTER"_def;

    //
    t += "NOMINMAX"_def;

    t += sw::Shared, "ASSIMP_BUILD_DLL_EXPORT"_def;
    t.Interface += sw::Shared, "ASSIMP_DLL"_def;

    t.writeFileOnce("AssimpPCH.h");
    t.configureFile("include/assimp/config.h.in", "assimp/config.h");

    t.Variables["GIT_COMMIT_HASH"] = 0;
    t.Variables["ASSIMP_VERSION_MAJOR"] = t.Variables["PACKAGE_VERSION_MAJOR"];
    t.Variables["ASSIMP_VERSION_MINOR"] = t.Variables["PACKAGE_VERSION_MINOR"];
    t.Variables["ASSIMP_VERSION_PATCH"] = t.Variables["PACKAGE_VERSION_PATCH"];
    t.Variables["ASSIMP_PACKAGE_VERSION"] = t.Variables["PACKAGE_VERSION"];
    t.configureFile("include/assimp/revision.h.in", "assimp/revision.h");

    t += "ASSIMP_BUILD_NO_OWN_ZLIB"_def;
    t += "org.sw.demo.madler.zlib"_dep;
    t += "org.sw.demo.madler.minizip"_dep;
    t += "org.sw.demo.miloyip.rapidjson"_dep;
    t += "org.sw.demo.irrlicht.irrXML"_dep;
    t += "org.sw.demo.kimkulling.openddl_parser-master"_dep;
    t += "org.sw.demo.zeux.pugixml"_dep;
    t += "org.sw.demo.stb.all-master"_dep;
    t += "org.sw.demo.nemtrif.utfcpp"_dep;
    t += "org.sw.demo.lighttransport.tinyusdz-dev"_dep;

    if (t.getCompilerType() == CompilerType::MSVC)
        t.CompileOptions.push_back("-bigobj");
}
