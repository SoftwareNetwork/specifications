void build(Solution &s)
{
    auto &openexr = s.addProject("AcademySoftwareFoundation.openexr", "3.3.4");
    openexr += Git("https://github.com/AcademySoftwareFoundation/openexr", "v{v}");

    auto &Config = openexr.addLibrary("Config");
    {
        Config.HeaderOnly = true;

        Config.configureFile("cmake/IexConfigInternal.h.in", "IexConfigInternal.h");

        Config.Variables["OPENEXR_VERSION_STRING"] = "\"" + Config.Variables["PACKAGE_VERSION"].toString() + "\"";
        Config.Variables["OPENEXR_PACKAGE_STRING"] = "\"IlmBase " + Config.Variables["PACKAGE_VERSION"].toString() + "\"";
        Config.Variables["OPENEXR_VERSION_MAJOR"] = Config.Variables["PACKAGE_VERSION_MAJOR"];
        Config.Variables["OPENEXR_VERSION_MINOR"] = Config.Variables["PACKAGE_VERSION_MINOR"];
        Config.Variables["OPENEXR_VERSION_PATCH"] = Config.Variables["PACKAGE_VERSION_PATCH"];
        Config.Variables["OPENEXR_IMF_NAMESPACE"] = "Imf";
        Config.Variables["OPENEXR_INTERNAL_IMF_NAMESPACE"] = "Imf";
        Config.configureFile("cmake/OpenEXRConfig.h.in", "OpenEXRConfig.h");
        Config.configureFile("cmake/OpenEXRConfigInternal.h.in", "OpenEXRConfigInternal.h");

        Config.Variables["IEX_INTERNAL_NAMESPACE"] = "Iex";
        Config.Variables["IEX_NAMESPACE"] = "Iex";
        Config.configureFile("cmake/IexConfig.h.in", "IexConfig.h");

        Config.Variables["ILMTHREAD_INTERNAL_NAMESPACE"] = "IlmThread";
        Config.Variables["ILMTHREAD_NAMESPACE"] = "IlmThread";
        Config.Variables["ILMTHREAD_THREADING_ENABLED"] = 1;
        if (!(Config.getBuildSettings().TargetOS.Type == OSType::Windows ||
            Config.getBuildSettings().TargetOS.Type == OSType::Mingw))
            Config.Variables["ILMTHREAD_HAVE_POSIX_SEMAPHORES"] = 1;
        Config.configureFile("cmake/IlmThreadConfig.h.in", "IlmThreadConfig.h");
    }

    auto &Iex = openexr.addLibrary("Iex");
    {
        Iex.setRootDirectory("src/lib/Iex");
        Iex.Public += sw::Shared, "OPENEXR_DLL"_def;
        Iex += sw::Shared, "IEX_EXPORTS"_def;
        Iex.Public += Config;
    }

    auto &IlmThread = openexr.addLibrary("IlmThread");
    {
        IlmThread.setRootDirectory("src/lib/IlmThread");
        IlmThread.Public += sw::Shared, "OPENEXR_DLL"_def;
        IlmThread += sw::Shared, "ILMTHREAD_EXPORTS"_def;
        IlmThread.Public += Iex;
    }

    auto &OpenEXR = openexr.addLibrary("OpenEXR");
    {
        OpenEXR.setRootDirectory("src/lib/OpenEXR");
        OpenEXR -= "b44ExpLogTable.cpp", "dwaLookups.cpp";
        OpenEXR.Public += sw::Shared, "OPENEXR_DLL"_def;
        OpenEXR += sw::Shared, "OPENEXR_EXPORTS"_def;
        OpenEXR.Public += IlmThread, "org.sw.demo.AcademySoftwareFoundation.Imath"_dep;
        OpenEXR += "org.sw.demo.madler.zlib"_dep;
    }
}
