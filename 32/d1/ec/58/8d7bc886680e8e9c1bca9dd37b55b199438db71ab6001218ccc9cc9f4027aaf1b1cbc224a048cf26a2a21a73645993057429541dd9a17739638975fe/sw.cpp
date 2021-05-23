void build(Solution &s)
{
    auto &expat = s.addTarget<LibraryTarget>("expat", "2.4.0");
    expat += Git("https://github.com/libexpat/libexpat", "R_{M}_{m}_{p}");
    //expat.SourceDir /= "expat";
    expat.setRootDirectory("expat");

    expat.setChecks("expat");

    expat +=
        "expat_config.h.cmake",
        "lib/.*\\.c"_rr,
        "lib/.*\\.h"_rr,
        "lib/libexpat.def";

    expat.Public +=
        "lib"_id;

    expat += "HAVE_EXPAT_CONFIG_H"_d;
    expat.Public += sw::Static, "XML_STATIC"_d;
    expat -= sw::Static, "lib/libexpat.def";

    if (expat.getBuildSettings().TargetOS.Type == OSType::Windows)
        expat += "WIN32"_d;
    else
    {
        expat += "XML_ENABLE_VISIBILITY"_def;
        expat += "XML_DEV_URANDOM"_d;
    }

    expat.Variables["XML_CONTEXT_BYTES"] = "1024";
    expat.Variables["XML_DTD"] = "1";
    expat.Variables["XML_NS"] = "1";
    expat.Variables["HAVE_MEMMOVE"] = 1;

    if (expat.Variables["WORDS_BIGENDIAN"] == 1)
        expat.Variables["BYTEORDER"] = "4321";
    else
        expat.Variables["BYTEORDER"] = "1234";

    if (expat.Variables["HAVE_SYS_TYPES_H"] == 0)
    {
        expat.Variables["OFF_T"] = "long";
        expat.Variables["SIZE_T"] = "unsigned";
    }
    else
    {
        expat.Variables["OFF_T"] = expat.Variables["HAVE_OFF_T"];
        expat.Variables["SIZE_T"] = expat.Variables["HAVE_SIZE_T"];
    }

    expat.configureFile("expat_config.h.cmake", "expat_config.h");
}

void check(Checker &c)
{
    auto &s = c.addSet("expat");
    s.checkIncludeExists("dlfcn.h");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("memory.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("unistd.h");
    s.checkTypeSize("size_t");
    s.checkTypeSize("void *");
    {
        auto &c = s.checkSymbolExists("off_t");
        c.Parameters.Includes.push_back("sys/types.h");
    }
    {
        auto &c = s.checkSymbolExists("size_t");
        c.Parameters.Includes.push_back("sys/types.h");
    }
}
