void build(Solution &s)
{
    auto &tinyxml = s.addTarget<StaticLibraryTarget>("leethomason.tinyxml", "2.6.2");
    tinyxml += RemoteFile("http://freefr.dl.sourceforge.net/project/tinyxml/tinyxml/{v}/tinyxml_{M}_{m}_{p}.zip");

    tinyxml +=
        "tinystr.cpp",
        "tinystr.h",
        "tinyxml.cpp",
        "tinyxml.h",
        "tinyxmlerror.cpp",
        "tinyxmlparser.cpp";

    tinyxml.Public += "TIXML_USE_STL"_d;
}
