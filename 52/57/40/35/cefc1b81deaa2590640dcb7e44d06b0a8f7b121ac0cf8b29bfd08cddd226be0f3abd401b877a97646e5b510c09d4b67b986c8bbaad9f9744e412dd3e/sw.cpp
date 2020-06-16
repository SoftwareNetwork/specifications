void build(Solution &s)
{
    auto &openexr = s.addProject("openexr", "2.4.2");
    openexr += Git("https://github.com/openexr/openexr", "v{v}");

    auto &ilmbase = openexr.addProject("ilmbase");

    auto &Half = ilmbase.addLibrary("Half");
    auto &Iex = ilmbase.addLibrary("Iex");
    auto &IexMath = ilmbase.addLibrary("IexMath");
    auto &Imath = ilmbase.addLibrary("Imath");
    auto &IlmThread = ilmbase.addLibrary("IlmThread");

    {
        auto &config = ilmbase.addLibrary("config");
        {
            config += "IlmBase/config.*"_rr;
            config.Variables["ILMBASE_VERSION_STRING"] = "\"" + config.Variables["PACKAGE_VERSION"].toString() + "\"";
            config.Variables["ILMBASE_PACKAGE_STRING"] = "\"IlmBase " + config.Variables["PACKAGE_VERSION"].toString() + "\"";
            config.Variables["ILMBASE_VERSION_MAJOR"] = config.Variables["PACKAGE_VERSION_MAJOR"];
            config.Variables["ILMBASE_VERSION_MINOR"] = config.Variables["PACKAGE_VERSION_MINOR"];
            config.Variables["ILMBASE_VERSION_PATCH"] = config.Variables["PACKAGE_VERSION_PATCH"];

            config.Variables["ILMBASE_INTERNAL_NAMESPACE_CUSTOM"] = 0;
            config.Variables["IMATH_INTERNAL_NAMESPACE"] = "Imath";
            config.Variables["IEX_INTERNAL_NAMESPACE"] = "Iex";
            config.Variables["ILMTHREAD_INTERNAL_NAMESPACE"] = "IlmThread";
            config.Variables["IMATH_NAMESPACE"] = "Imath";
            config.Variables["IEX_NAMESPACE"] = "Iex";
            config.Variables["ILMTHREAD_NAMESPACE"] = "IlmThread";

            if (config.getBuildSettings().TargetOS.Type == OSType::Windows ||
                config.getBuildSettings().TargetOS.Type == OSType::Mingw)
            {
                config.Public += "PLATFORM_WINDOWS"_def;
            }
            else
            {
                config += "HAVE_PTHREAD=1"_v;
                if (config.getBuildSettings().TargetOS.Type != OSType::Macos)
                {
                    config += "ILMBASE_HAVE_LARGE_STACK=1"_v;
                    config += "HAVE_POSIX_SEMAPHORES=1"_v;
                    config += "ILMBASE_HAVE_CONTROL_REGISTER_SUPPORT=1"_v;
                }
            }
            config.configureFile("IlmBase/config/IlmBaseConfig.h.in", "IlmBaseConfig.h", ConfigureFlags::EnableUndefReplacements);
            config.configureFile("IlmBase/config/IlmBaseConfigInternal.h.in", "IlmBaseConfigInternal.h", ConfigureFlags::EnableUndefReplacements);
        }

        {
            auto &eLut = ilmbase.addExecutable("eLut");
            eLut += "IlmBase/Half/eLut.cpp";
            //eLut.Scope = TargetScope::Tool;

            auto &toFloat = ilmbase.addExecutable("toFloat");
            toFloat += "IlmBase/Half/toFloat.cpp";
            //toFloat.Scope = TargetScope::Tool;

            Half += "IlmBase/Half/half.*"_rr;
            //Half -= "IlmBase/Half/eLut.cpp", "IlmBase/Half/toFloat.cpp";
            Half += sw::Shared, "HALF_EXPORTS"_def;
            Half.Public += sw::Shared, "OPENEXR_DLL"_def;
            Half.Public += config;
            Half.Public += "IlmBase/Half"_idir;

            {
                auto c = Half.addCommand();
                c << cmd::prog(eLut)
                    << cmd::std_out("eLut.h");
            }
            {
                auto c = Half.addCommand();
                c << cmd::prog(toFloat)
                    << cmd::std_out("toFloat.h");
            }
        }

        Iex += "IlmBase/Iex/Iex.*"_rr;
        Iex += sw::Shared, "IEX_EXPORTS"_def;
        Iex.Public += sw::Shared, "OPENEXR_DLL"_def;
        Iex.Public += Half;
        Iex.Public += "IlmBase/Iex"_idir;

        IexMath += "IlmBase/IexMath/IexMath.*"_rr;
        IexMath += sw::Shared, "IEXMATH_EXPORTS"_def;
        IexMath.Public += sw::Shared, "OPENEXR_DLL"_def;
        IexMath.Public += Iex;
        IexMath.Public += "IlmBase/IexMath"_idir;

        Imath += "IlmBase/Imath/Imath.*"_rr;
        Imath += sw::Shared, "IMATH_EXPORTS"_def;
        Imath.Public += sw::Shared, "OPENEXR_DLL"_def;
        Imath.Public += Iex;
        Imath.Public += "IlmBase/Imath"_idir;

        IlmThread += "IlmBase/IlmThread/IlmThread.*"_rr;
        IlmThread += sw::Shared, "ILMTHREAD_EXPORTS"_def;
        IlmThread.Public += sw::Shared, "OPENEXR_DLL"_def;
        IlmThread.Public += Iex;
        IlmThread.Public += "IlmBase/IlmThread"_idir;
    }

    //
    auto &config = openexr.addLibrary("config");
    {
        config += "OpenEXR/config.*"_rr;
        config.Variables["OPENEXR_VERSION_STRING"] = "\"" + config.Variables["PACKAGE_VERSION"].toString() + "\"";
        config.Variables["OPENEXR_PACKAGE_STRING"] = "\"IlmBase " + config.Variables["PACKAGE_VERSION"].toString() + "\"";
        config.Variables["OPENEXR_VERSION_MAJOR"] = config.Variables["PACKAGE_VERSION_MAJOR"];
        config.Variables["OPENEXR_VERSION_MINOR"] = config.Variables["PACKAGE_VERSION_MINOR"];
        config.Variables["OPENEXR_VERSION_PATCH"] = config.Variables["PACKAGE_VERSION_PATCH"];

        config.Variables["OPENEXR_INTERNAL_NAMESPACE_CUSTOM"] = 0;
        config.Variables["OPENEXR_IMF_NAMESPACE"] = "Imf";
        config.Variables["OPENEXR_IMF_INTERNAL_NAMESPACE"] = "Imf";

        config += "OPENEXR_IMF_HAVE_COMPLETE_IOMANIP=1"_v;

        if (config.getBuildSettings().TargetOS.Type == OSType::Windows ||
            config.getBuildSettings().TargetOS.Type == OSType::Mingw
            )
        {
            config.Public += "HAVE_COMPLETE_IOMANIP"_def;
        }
        else
        {
            config += "HAVE_PTHREAD=1"_v;
            if (config.getBuildSettings().TargetOS.Type != OSType::Macos)
            {
                config += "OPENEXR_IMF_HAVE_LARGE_STACK=1"_v;
                config += "HAVE_POSIX_SEMAPHORES=1"_v;
                config += "OPENEXR_IMF_HAVE_LINUX_PROCFS=1"_v;
                config += "OPENEXR_IMF_HAVE_COMPLETE_IOMANIP=1"_v;
                config += "OPENEXR_IMF_HAVE_LARGE_STACK=1"_v;
            }
        }
        if (config.getBuildSettings().TargetOS.Type == OSType::Macos)
        {
            config += "OPENEXR_IMF_HAVE_DARWIN=1"_v;
        }
        config.configureFile("OpenEXR/config/OpenEXRConfig.h.in", "OpenEXRConfig.h", ConfigureFlags::EnableUndefReplacements);
        config.configureFile("OpenEXR/config/OpenEXRConfigInternal.h.in", "OpenEXRConfigInternal.h", ConfigureFlags::EnableUndefReplacements);
    }

    auto &IlmImf = openexr.addLibrary("IlmImf");
    {
        auto &b44ExpLogTable = openexr.addExecutable("b44ExpLogTable");
        b44ExpLogTable += "OpenEXR/IlmImf/b44ExpLogTable.cpp";
        b44ExpLogTable += Half;
        //b44ExpLogTable.Scope = TargetScope::Tool;

        auto &dwaLookups = openexr.addExecutable("dwaLookups");
        dwaLookups += "OpenEXR/IlmImf/dwaLookups.cpp";
        dwaLookups += Half, config, IlmThread;
        (dwaLookups + IlmImf)->IncludeDirectoriesOnly = true;
        //dwaLookups.Scope = TargetScope::Tool;

        IlmImf += "OpenEXR/IlmImf/Imf.*"_rr;
        //IlmImf -= "OpenEXR/IlmImf/b44ExpLogTable.cpp", "OpenEXR/IlmImf/dwaLookups.cpp";
        IlmImf += sw::Shared, "ILMIMF_EXPORTS"_def;
        IlmImf.Public += sw::Shared, "OPENEXR_DLL"_def;
        IlmImf.Public += config, IexMath, IlmThread, Imath,
            "org.sw.demo.madler.zlib"_dep;
        IlmImf.Public += "OpenEXR/IlmImf"_idir;

        {
            auto c = IlmImf.addCommand();
            c << cmd::prog(b44ExpLogTable)
                << cmd::std_out("b44ExpLogTable.h");
        }
        {
            auto c = IlmImf.addCommand();
            c << cmd::prog(dwaLookups)
                << cmd::std_out("dwaLookups.h");
        }
    }
}
