void build(Solution &s)
{
    auto &xlnt = s.addTarget<LibraryTarget>("tfussell.xlnt", "1.3.0");
    xlnt += Git("https://github.com/tfussell/xlnt", "v{v}");

    auto &libstudxml = xlnt.addTarget<LibraryTarget>("libstudxml");
    {
        libstudxml.setRootDirectory("third-party/libstudxml");
        libstudxml += "xml/.*"_rr;
        libstudxml -= "xml/details/expat/.*"_rr;

        libstudxml.Public += "LIBSTUDXML_EXTERNAL_EXPAT"_d;
        libstudxml += sw::Shared, "LIBSTUDXML_DYNAMIC_LIB"_d;
        libstudxml.Public += sw::Static, "LIBSTUDXML_STATIC_LIB"_d;

        libstudxml.Public += "org.sw.demo.expat-2"_dep;

        libstudxml.patch("xml/details/config-vc.h",
            "#define LIBSTUDXML_STATIC_LIB 1", "//#define  LIBSTUDXML_STATIC_LIB 1");
    }

    {
        xlnt.ApiNames.insert("XLNT_API");
        xlnt.Public += "org.sw.demo.ledger.utfcpp-master"_dep;
        xlnt.Public += libstudxml;
    }
}
