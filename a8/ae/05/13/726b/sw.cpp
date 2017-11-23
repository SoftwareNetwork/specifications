void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");

    auto &openexr = s.addProject("openexr", "2.2.0");
    openexr.Source = RemoteFile("http://download.savannah.nongnu.org/releases/openexr/openexr-{v}.tar.gz");

    auto &ilmbase = openexr.addDirectory("ilmbase");
    ilmbase.Source = RemoteFile("http://download.savannah.nongnu.org/releases/openexr/ilmbase-{v}.tar.gz");

    auto &elut = ilmbase.addTarget<ExecutableTarget>("elut");
    elut += "Half/eLut.cpp";

    auto &tofloat = ilmbase.addTarget<ExecutableTarget>("tofloat");
    tofloat += "Half/toFloat.cpp";

    auto &ilmbase_config = ilmbase.addTarget<LibraryTarget>("config");
    ilmbase_config += "config.windows/.*"_rr;

    const auto ibc = ilmbase_config.BinaryDir / "IlmBaseConfig.h";
    auto c = std::make_shared<ExecuteCommand>([ibc, &ilmbase_config]()
    {
        std::string s;
        if (ilmbase_config.Settings.TargetOS.Type == OSType::Windows)
            s = read_file(ilmbase_config.SourceDir / "config.windows" / "IlmBaseConfig.h");
        else if (ilmbase_config.Settings.TargetOS.Type == OSType::Macos)
        {
            s += "#define HAVE_PTHREAD 1\n";
        }
        else
        {
            s += R"(#define HAVE_PTHREAD 1
#define ILMBASE_HAVE_LARGE_STACK  1
#define HAVE_POSIX_SEMAPHORES 1
#define ILMBASE_HAVE_CONTROL_REGISTER_SUPPORT 1
)";
        }

        s += R"(
#define ILMBASE_INTERNAL_NAMESPACE_CUSTOM 0
#define IMATH_INTERNAL_NAMESPACE Imath
#define IEX_INTERNAL_NAMESPACE Iex
#define ILMTHREAD_INTERNAL_NAMESPACE IlmThread

#define IMATH_NAMESPACE Imath
#define IEX_NAMESPACE Iex
#define ILMTHREAD_NAMESPACE IlmThread
#define ILMBASE_VERSION_STRING ")" + ilmbase_config.Variables["PACKAGE_VERSION"] + R"("
#define ILMBASE_PACKAGE_STRING "IlmBase )" + ilmbase_config.Variables["PACKAGE_VERSION"] + R"("
#define ILMBASE_VERSION_MAJOR )" + ilmbase_config.Variables["PACKAGE_VERSION_MAJOR"] + R"(
#define ILMBASE_VERSION_MINOR )" + ilmbase_config.Variables["PACKAGE_VERSION_MINOR"] + R"(
#define ILMBASE_VERSION_PATCH )" + ilmbase_config.Variables["PACKAGE_VERSION_PATCH"] + R"(

// Version as a single hex number, e.g. 0x01000300 == 1.0.3
#define ILMBASE_VERSION_HEX ((ILMBASE_VERSION_MAJOR << 24) | \\
                            (ILMBASE_VERSION_MINOR << 16) | \\
                            (ILMBASE_VERSION_PATCH <<  8))
)";

        write_file(ibc, s);
    });
    c->addOutput(ibc);
    ilmbase_config += ibc;

    auto &half = ilmbase.addTarget<LibraryTarget>("half");

    half +=
        "Half/.*\\.h"_rr,
        "Half/half.cpp";

    half.Public +=
        "Half"_id;

    half.Private += sw::Shared, "HALF_EXPORTS"_d;
    half.Public += sw::Shared, "OPENEXR_DLL"_d;

    half.Public += ilmbase_config;
    auto ed = half + elut;
    ed->Dummy = true;
    auto etf = half + tofloat;
    etf->Dummy = true;

    {
        auto c = std::make_shared<Command>();
        c->program = elut.getOutputFile();
        c->redirectStdout(half.BinaryDir / "eLut.h");
        half += half.BinaryDir / "eLut.h";
    }

    {
        auto c = std::make_shared<Command>();
        c->program = tofloat.getOutputFile();
        c->redirectStdout(half.BinaryDir / "toFloat.h");
        half += half.BinaryDir / "toFloat.h";
    }

    auto &b44explogtable = openexr.addTarget<ExecutableTarget>("b44explogtable");
    b44explogtable += "IlmImf/b44ExpLogTable.cpp";
    b44explogtable.Public += half;

    auto &openexr_config = openexr.addTarget<LibraryTarget>("config");
    openexr_config += "config.windows/.*"_rr;

    const auto oc = openexr_config.BinaryDir / "OpenEXRConfig.h";
    auto c2 = std::make_shared<ExecuteCommand>([oc, &openexr_config]()
    {
        std::string s;
        if (openexr_config.Settings.TargetOS.Type == OSType::Windows)
        {
            s = read_file(openexr_config.SourceDir / "config.windows" / "OpenEXRConfig.h");
            s += "#define OPENEXR_IMF_HAVE_COMPLETE_IOMANIP 1";
        }
        else if (openexr_config.Settings.TargetOS.Type == OSType::Macos)
        {
            s += R"(#define OPENEXR_IMF_HAVE_DARWIN 1
#define OPENEXR_IMF_HAVE_COMPLETE_IOMANIP 1
#include <string.h>
)";
        }
        else
        {
            s += R"(#define OPENEXR_IMF_HAVE_LINUX_PROCFS 1
#define OPENEXR_IMF_HAVE_COMPLETE_IOMANIP 1
#define OPENEXR_IMF_HAVE_LARGE_STACK 1
)";
        }

        s += R"(
#define OPENEXR_IMF_INTERNAL_NAMESPACE_CUSTOM 0
#define OPENEXR_IMF_INTERNAL_NAMESPACE Imf
#define OPENEXR_IMF_NAMESPACE Imf

#define OPENEXR_VERSION_STRING ")" + openexr_config.Variables["PACKAGE_VERSION"] + R"("
#define OPENEXR_PACKAGE_STRING "IlmBase )" + openexr_config.Variables["PACKAGE_VERSION"] + R"("
#define OPENEXR_VERSION_MAJOR )" + openexr_config.Variables["PACKAGE_VERSION_MAJOR"] + R"(
#define OPENEXR_VERSION_MINOR )" + openexr_config.Variables["PACKAGE_VERSION_MINOR"] + R"(
#define OPENEXR_VERSION_PATCH )" + openexr_config.Variables["PACKAGE_VERSION_PATCH"] + R"(

// Version as a single hex number, e.g. 0x01000300 == 1.0.3
#define OPENEXR_VERSION_HEX ((OPENEXR_VERSION_MAJOR << 24) | \\
                                (OPENEXR_VERSION_MINOR << 16) | \\
                                (OPENEXR_VERSION_PATCH <<  8))
)";

        write_file(openexr_config.BinaryDir / "OpenEXRConfig.h", s);
    });
    c2->addOutput(oc);
    openexr_config += oc;

    //
    auto &iex = ilmbase.addTarget<LibraryTarget>("iex");

    iex +=
        "Iex/.*\\.cpp"_rr,
        "Iex/.*\\.h"_rr;

    iex.Public +=
        "Iex"_id;

    iex.Private += sw::Shared, "IEX_EXPORTS"_d;
    iex.Public += sw::Shared, "OPENEXR_DLL"_d;

    iex.Public += half;

    //
    auto &imath = ilmbase.addTarget<LibraryTarget>("imath");

    imath +=
        "Imath/.*\\.cpp"_rr,
        "Imath/.*\\.h"_rr;

    imath.Public +=
        "Imath"_id;

    imath.Private += sw::Shared, "IMATH_EXPORTS"_d;
    imath.Public += sw::Shared, "OPENEXR_DLL"_d;

    imath.Public += iex;

    //
    auto &ilmthread = ilmbase.addTarget<LibraryTarget>("ilmthread");

    ilmthread +=
        "IlmThread/.*\\.cpp"_rr,
        "IlmThread/.*\\.h"_rr;

    ilmthread.Public +=
        "IlmThread"_id;

    ilmthread.Private += sw::Shared, "ILMTHREAD_EXPORTS"_d;
    ilmthread.Public += sw::Shared, "OPENEXR_DLL"_d;

    ilmthread.Public += iex;

    if (sln.Settings.TargetOS.Type != OSType::Windows)
    {
        ilmthread -= "IlmThread/IlmThreadMutexWin32.cpp";
        ilmthread -= "IlmThread/IlmThreadSemaphoreWin32.cpp";
        ilmthread -= "IlmThread/IlmThreadWin32.cpp";
    }

    auto &dwalookups = openexr.addTarget<ExecutableTarget>("dwalookups");
    dwalookups += "IlmImf/dwaLookups.cpp";
    dwalookups["IlmImf/dwaLookups.cpp"].addDependencies(openexr_config.BinaryDir / "OpenEXRConfig.h");
    dwalookups.Public += imath;
    dwalookups.Public += ilmthread;
    dwalookups.Public += openexr_config;

    //
    auto &ilmimf = openexr.addTarget<LibraryTarget>("ilmimf");

    ilmimf +=
        "IlmImf/Imf.*\\.cpp"_rr,
        "IlmImf/Imf.*\\.h"_rr;

    ilmimf.Public +=
        "IlmImf"_id;

    ilmimf.Private += sw::Shared, "ILMIMF_EXPORTS"_d;
    ilmimf.Public += sw::Shared, "OPENEXR_DLL"_d;

    ilmimf.Public += ilmthread;
    ilmimf.Public += openexr_config;
    ilmimf.Public += imath;
    ilmimf.Public += "pub.cppan2.demo.madler.zlib-1"_dep;

    auto di = dwalookups + ilmimf;
    di->IncludeDirectoriesOnly = true;

    auto id = ilmimf + dwalookups;
    id->Dummy = true;
    auto ib = ilmimf + b44explogtable;
    ib->Dummy = true;

    {
        auto c = std::make_shared<Command>();
        c->program = dwalookups.getOutputFile();
        c->redirectStdout(ilmimf.BinaryDir / "dwaLookups.h");
        ilmimf += ilmimf.BinaryDir / "dwaLookups.h";
    }

    {
        auto c = std::make_shared<Command>();
        c->program = b44explogtable.getOutputFile();
        c->redirectStdout(ilmimf.BinaryDir / "b44ExpLogTable.h");
        ilmimf += ilmimf.BinaryDir / "b44ExpLogTable.h";
    }

    auto &ilmimfutil = openexr.addTarget<LibraryTarget>("ilmimfutil");

    ilmimfutil +=
        "IlmImfUtil/.*\\.cpp"_rr,
        "IlmImfUtil/.*\\.h"_rr;

    ilmimfutil.Public +=
        "IlmImfUtil"_id;

    ilmimfutil.Private += sw::Shared, "ILMIMF_EXPORTS"_d;
    ilmimfutil.Public += sw::Shared, "OPENEXR_DLL"_d;

    ilmimfutil.Public += ilmimf;
}
