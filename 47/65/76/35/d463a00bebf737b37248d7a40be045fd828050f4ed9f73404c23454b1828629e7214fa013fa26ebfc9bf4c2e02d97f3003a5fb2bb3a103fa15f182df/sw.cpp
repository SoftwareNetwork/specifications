void build(Solution &s)
{
    auto &xlnt = s.addTarget<LibraryTarget>("tfussell.xlnt", "1.5.0");
    xlnt += Git("https://github.com/tfussell/xlnt", "v{v}");

    auto &libstudxml = xlnt.addTarget<LibraryTarget>("libstudxml");
    {
        libstudxml.setRootDirectory("third-party/libstudxml");
        libstudxml += "libstudxml/.*"_rr;
        libstudxml -= "libstudxml/details/expat/.*"_rr;
        libstudxml -= "libstudxml/parser.ixx";
        libstudxml -= "libstudxml/serializer.ixx";

        libstudxml += sw::Shared, "LIBSTUDXML_SHARED_BUILD"_d;
        libstudxml.Interface += sw::Shared, "LIBSTUDXML_SHARED"_d;
        libstudxml.Public += sw::Static, "LIBSTUDXML_STATIC_BUILD"_d;
        libstudxml.Interface += sw::Shared, "LIBSTUDXML_STATIC"_d;

        libstudxml.Public += "org.sw.demo.expat"_dep;
        libstudxml.Public += "LIBSTUDXML_EXTERNAL_EXPAT"_def;
    }

    {
        xlnt.ApiNames.insert("XLNT_API");
        xlnt.Public += "org.sw.demo.ledger.utfcpp-master"_dep;
        xlnt.Public += "org.sw.demo.richgel999.miniz"_dep;
        xlnt.Public += libstudxml;
    }
}
