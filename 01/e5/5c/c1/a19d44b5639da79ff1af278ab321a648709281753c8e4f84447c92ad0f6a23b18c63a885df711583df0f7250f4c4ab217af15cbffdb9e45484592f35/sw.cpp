#pragma sw require pub.egorpugin.primitives.filesystem-master

void build(Solution &s)
{
    auto &p = s.addProject("copperspice", "1.5.2");
    p += Git("https://github.com/copperspice/copperspice", "cs-{v}");

    auto read_files_from_cmake = [](const path &fn, const String &var) -> Strings
    {
        if (!fs::exists(fn))
            return {};
        auto str = read_file(fn);
        auto p = str.find("set(" + var);
        if (p != str.npos)
        {
            auto b = str.find("}", p);
            auto e = str.find(")", b);
            auto strs = split_string(str.substr(b + 1, e - b - 1), " \r\n\t");
            for (auto &s : strs)
            {
                static const String cmsd = "${CMAKE_CURRENT_SOURCE_DIR}/";
                if (s.find(cmsd) == 0)
                    s = s.substr(cmsd.size());
                if (s.find("/") == 0)
                    s = s.substr(1);
            }
            return strs;
        }
        return {};
    };

    auto macro_generate_public = [&read_files_from_cmake]
    (auto &t, const String &var, const String &module, const Strings &dirs, const String &prefix)
    {
        for (auto &d : dirs)
        {
            for (auto &f : read_files_from_cmake(t.SourceDir / prefix / d / (d + ".cmake"), var))
                t.writeFileOnce(path(module) / path(f).filename(),
                    "#include <" + normalize_path(t.SourceDir / prefix / d / (boost::to_lower_copy(f) + ".h")) + ">");
        }
    };

    auto macro_generate_private = [&read_files_from_cmake]
    (auto &t, const String &var, const String &module, const Strings &dirs, const String &prefix)
    {
        for (auto &d : dirs)
        {
            for (auto &f : read_files_from_cmake(t.SourceDir / prefix / d / (d + ".cmake"), var))
                t.configureFile(t.SourceDir / prefix / f,
                    t.BinaryPrivateDir / path(module) / "private" / path(f).filename(),
                    ConfigureFlags::CopyOnly);
        }
    };

    auto macro_generate_misc = [&read_files_from_cmake]
    (auto &t, const String &var, const path &module, const Strings &dirs, const String &prefix)
    {
        for (auto &d : dirs)
        {
            for (auto &f : read_files_from_cmake(t.SourceDir / prefix / d / (d + ".cmake"), var))
                t.configureFile(t.SourceDir / prefix / f,
                    path(module) / path(f).filename(),
                    ConfigureFlags::CopyOnly);
        }
    };

    auto add_files = [&macro_generate_public, &macro_generate_private, &macro_generate_misc]
    (auto &t, const String &d)
    {
        Strings dirs;
        for (auto &d : fs::directory_iterator(t.SourceDir / ("src/" + d)))
        {
            if (fs::is_directory(d))
                dirs.push_back(d.path().filename().u8string());
        }

        auto fcap = d;
        fcap[0] = toupper(fcap[0]);
        auto cap = boost::to_upper_copy(d);

        t += IncludeDirectory(t.BinaryPrivateDir / ("Qt" + fcap) / "private");
        t.Public += IncludeDirectory(t.BinaryDir / ("Qt" + fcap));
        for (auto &p : dirs)
            t += IncludeDirectory("src/" + d + "/" + p);
        macro_generate_public(t, cap + "_PUBLIC_INCLUDES", "Qt" + fcap, dirs, "src/" + d);
        macro_generate_private(t, cap + "_PRIVATE_INCLUDES", "Qt" + fcap, dirs, "src/" + d);
        macro_generate_misc(t, cap + "_INCLUDES", "Qt" + fcap, dirs, "src/" + d);

        return dirs;
    };

    auto remove_platform_files = [](auto &core)
    {
        core -= ".*unix.*"_rr;
        core -= ".*mac.*"_rr;
        core -= ".*linux.*"_rr;
        core -= ".*glib.*"_rr;
    };

    auto &harfbuzz = p.addTarget<StaticLibrary>("third_party.harfbuzz");
    {
        harfbuzz.setRootDirectory("src/3rdparty/harfbuzz");
        harfbuzz -= "src/.*"_rr;
        harfbuzz +=
            "src/harfbuzz.c",
            "src/harfbuzz-impl.c",
            "src/harfbuzz-stream.c",
            "src/harfbuzz-shaper-all.cpp";
        if (s.Settings.TargetOS.Type == OSType::Windows)
        {
            harfbuzz.Private += "UNICODE"_d;
            harfbuzz.Public += "WIN32"_d;
        }
    }

    auto &wintab = p.addTarget<StaticLibraryTarget>("third_party.wintab");
    wintab.setRootDirectory("src/3rdparty/wintab");

    auto &core = p.addTarget<Library>("core");
    {
        core.setChecks("cs");

        core += "src/core/.*"_rr;

        remove_platform_files(core);
        core -= ".*nacl.*"_rr;
        core -= ".*iconv.*"_rr;
        core -= ".*kqueue.*"_rr;
        core -= ".*inotify.*"_rr;
        core -= ".*dnotify.*"_rr;
        core -= ".*fsevents.*"_rr;
        core -= ".*fork.*"_rr;
        core -= ".*qelapsedtimer_generic.cpp"_rr;
        core -= ".*qcrashhandler.cpp"_rr;
        core -= "src/core/tools/qunicodetables.cpp";

        core += "QT_BUILD_CORE_LIB"_def;
        core += "BUILDING_LIB_CS_STRING="_def;
        core += "BUILDING_LIB_CS_SIGNAL="_def;
        core += "BUILD_DATE=\"\""_def;

        core.Protected += "org.sw.demo.madler.zlib"_dep;
        core += "org.sw.demo.unicode.icu.i18n"_dep;
        core.Protected += harfbuzz;

        if (s.Settings.TargetOS.Type == OSType::Windows)
        {
            core.Protected += "NOMINMAX"_def;
            core.Public += "UNICODE="_def;
            core += "ws2_32.lib"_slib;
            core += "user32.lib"_slib;
            core += "ole32.lib"_slib;
        }

        auto &v = p.pkg.version;
        core.Variables["HEX_VERSION"] = (v.getMajor() << 16) + (v.getMinor() << 8) + v.getPatch();

        core.configureFile("src/core/global/cs_build_info.h.in", "cs_build_info.h");
        core.configureFile("cmake/qt-acconfig.h.cmake", "qt-acconfig.h");
        core.patch("src/core/tools/qlocale_icu.cpp", "reinterpret_cast<UChar *>", "(UChar *)");
        core.patch("src/core/tools/qlocale_icu.cpp", "reinterpret_cast<const UChar *>", "(const UChar *)");

        core.Protected += IncludeDirectory(core.BinaryPrivateDir / "QtCore" / "private");
        auto dirs = add_files(core, "core");
        macro_generate_misc(core, "CORE_INCLUDES", "QtCore", dirs, "src/core");
        macro_generate_misc(core, "CORE_REGEX_INCLUDES", "QtCore/regex", dirs, "src/core");
    }

    auto add_target = [&p, &core, &add_files, &remove_platform_files](const String &name) -> decltype(auto)
    {
        auto &t = p.addTarget<Library>(name);
        t += FileRegex("src/" + name, ".*", true);
        remove_platform_files(t);
        t += Definition("QT_BUILD_" + boost::to_upper_copy(name) + "_LIB");
        t.Public += core;
        t.CompileOptions.push_back("-bigobj");

        add_files(t, name);
        return t;
    };

    auto &xml = add_target("xml");

    return;

    auto &gui = add_target("gui");
    gui -= ".*x11.*"_rr;
    gui -= ".*qpa.*"_rr;
    gui -= ".*qws.*"_rr;
    gui += wintab;
    gui += "org.sw.demo.freetype"_dep;
    gui += "org.sw.demo.jpeg"_dep;
    gui += "org.sw.demo.mng"_dep;
    gui += "org.sw.demo.tiff"_dep;
    gui += "org.sw.demo.google.angle.egl"_dep;
}

void check(Checker &c)
{
    auto &s = c.addSet("cs");
    s.checkIncludeExists("cups/cups.h");
    s.checkIncludeExists("dirent.h");
    s.checkIncludeExists("dlfcn.h");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("features.h");
    s.checkIncludeExists("grp.h");
    s.checkIncludeExists("libpq-fe.h");
    s.checkIncludeExists("mysql.h");
    s.checkIncludeExists("netinet/in.h");
    s.checkIncludeExists("net/if.h");
    s.checkIncludeExists("pg_config.h");
    s.checkIncludeExists("pthread.h");
    s.checkIncludeExists("pwd.h");
    s.checkIncludeExists("signal.h");
    s.checkIncludeExists("sys/ioctl.h");
    s.checkIncludeExists("sys/ipc.h");
    s.checkIncludeExists("sys/param.h");
    s.checkIncludeExists("sys/shm.h");
    s.checkIncludeExists("sys/socket.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("sys/wait.h");
    s.checkIncludeExists("unistd.h");
    s.checkTypeSize("size_t");
    s.checkLibraryFunctionExists("mysqlclient", "my_init");
    s.checkLibraryFunctionExists("pq", "PQconnectdbParams");
}
