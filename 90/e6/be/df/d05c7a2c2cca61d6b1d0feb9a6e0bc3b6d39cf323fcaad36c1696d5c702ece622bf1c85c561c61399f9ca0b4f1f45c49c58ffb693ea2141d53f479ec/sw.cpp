void build(Solution &s)
{
    auto &td = s.addProject("telegram.td", "1.8.56");
    td += Git("https://github.com/tdlib/td", "", "master");

    auto &generate_mime_types_gperf = td.addTarget<ExecutableTarget>("generate_mime_types_gperf");
    generate_mime_types_gperf += "tdutils/generate/generate_mime_types_gperf.cpp";

    auto cppstd = cpp17;

    auto &utils = td.addTarget<StaticLibraryTarget>("utils");
    {
        utils += cppstd;
        utils +=
            "tdutils/generate/.*"_rr,
            "tdutils/td/utils/.*"_rr;
        utils -=
            "tdutils/generate/.*"_rr;
        utils.Public += "tdutils"_idir;
        if (utils.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            utils += "Mswsock.lib"_slib;
            utils += "Normaliz.lib"_slib;
            utils += "psapi.lib"_slib;
        }

        utils.Public += "org.sw.demo.madler.zlib"_dep;
        utils.Public += "org.sw.demo.openssl.crypto"_dep;

        utils.Variables["TD_HAVE_OPENSSL"] = 1;
        utils.Variables["TD_HAVE_ZLIB"] = 1;
        utils.configureFile("tdutils/td/utils/config.h.in", "td/utils/config.h");
        utils.writeFileOnce("auto/git_info.h", "#define GIT_COMMIT \"\"\n#define GIT_DIRTY false");
        utils.writeFileOnce("auto/git_info.cpp", R"(
namespace td {
    char const *get_git_commit_hash(void) {return "";}
}
)");
        utils += "auto/git_info.cpp";

        {
            auto c = utils.addCommand();
            c << cmd::prog(generate_mime_types_gperf)
                << cmd::wdir(utils.BinaryDir)
                << cmd::in("tdutils/generate/mime_types.txt")
                << cmd::out("auto/mime_type_to_extension.gperf")
                << cmd::out("auto/extension_to_mime_type.gperf")
                ;
        }

        utils -= "org.sw.demo.gnu.gperf"_dep;
        if (utils.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            auto gperf_cmd = [&utils](const String &s)
            {
                auto c = utils.addCommand();
                c << cmd::prog("org.sw.demo.gnu.gperf"_dep)
                    << cmd::wdir(utils.BinaryDir)
                    << "-m100"
                    << ("--output-file=auto/" + s + ".cpp")
                    << cmd::in(utils.BinaryDir / ("auto/" + s + ".gperf"))
                    << cmd::end()
                    << cmd::out("auto/" + s + ".cpp")
                    ;
            };
            gperf_cmd("mime_type_to_extension");
            gperf_cmd("extension_to_mime_type");
        }
        else
        {
            //set(MIME_TYPE_TO_EXTENSION_CMD ${gperf} -m100 auto/mime_type_to_extension.gperf | grep -v __gnu_inline__ > auto/mime_type_to_extension.cpp)
            //set(EXTENSION_TO_MIME_TYPE_CMD ${gperf} -m100 auto/extension_to_mime_type.gperf | grep -v __gnu_inline__ > auto/extension_to_mime_type.cpp)
        }
    }

    auto &actor = td.addTarget<StaticLibraryTarget>("actor");
    {
        actor += cppstd;
        actor += "tdactor/td/actor/.*"_rr;
        actor.Public += "tdactor"_idir;
        actor.Public += utils;
        //actor.pushFrontToFileOnce("tdactor/td/actor/impl/ConcurrentScheduler.cpp", "#include \"td/actor/actor.h\"");
    }

    auto &sqlite = td.addTarget<StaticLibraryTarget>("sqlite");
    {
        sqlite += "sqlite/.*"_rr;
        sqlite.Public += "sqlite"_idir;

        sqlite +=
            "OMIT_MEMLOCK"_def,
            "SQLITE_DEFAULT_MEMSTATUS=0"_def,
            "SQLITE_DEFAULT_RECURSIVE_TRIGGERS=1"_def,
            "SQLITE_DEFAULT_SYNCHRONOUS=1"_def,
            "SQLITE_DISABLE_LFS"_def,
            "SQLITE_ENABLE_FTS5"_def,
            "SQLITE_HAS_CODEC"_def,
            "SQLITE_OMIT_DECLTYPE"_def,
            "SQLITE_OMIT_DEPRECATED"_def,
            "SQLITE_OMIT_DESERIALIZE"_def,
            "SQLITE_OMIT_LOAD_EXTENSION"_def,
            "SQLITE_OMIT_PROGRESS_CALLBACK"_def,
            //"SQLITE_OMIT_SHARED_CACHE"_def,
            "SQLITE_TEMP_STORE=2"_def
            ;
        sqlite.Public += utils;
    }

    auto &db = td.addTarget<StaticLibraryTarget>("db");
    {
        db += cppstd;
        db += "tddb/td/db/.*"_rr;
        db.Public += "tddb"_idir;
        db.Public += actor, sqlite;
    }

    auto &tl = td.addTarget<StaticLibraryTarget>("tl");
    tl += "tdtl/td/tl/.*"_rr;
    tl.Public += "tdtl"_idir;

    auto &generate_clib = td.addTarget<ExecutableTarget>("generate.clib");
    {
        generate_clib +=
            "td/generate/generate_c.cpp",
            "td/generate/tl_writer_c.h";
        generate_clib.Public += tl;
    }

    auto &generate_json = td.addTarget<ExecutableTarget>("generate.json");
    {
        generate_json +=
            "td/generate/generate_json.cpp",
            "td/generate/tl_json_converter.cpp",
            "td/generate/tl_json_converter.h",
            "td/generate/tl_writer_c.h";
        generate_json.Public += tl;
        generate_json.Public += utils;
    }

    auto &generate_lib = td.addTarget<StaticLibraryTarget>("generate.lib");
    {
        generate_lib +=
            "td/generate/tl_writer_cpp.cpp",
            "td/generate/tl_writer_h.cpp",
            "td/generate/tl_writer_hpp.cpp",
            "td/generate/tl_writer_jni_cpp.cpp",
            "td/generate/tl_writer_jni_h.cpp",
            "td/generate/tl_writer_td.cpp",
            "td/generate/tl_writer_cpp.h",
            "td/generate/tl_writer_h.h",
            "td/generate/tl_writer_hpp.h",
            "td/generate/tl_writer_jni_cpp.h",
            "td/generate/tl_writer_jni_h.h",
            "td/generate/tl_writer_td.h"
            ;
        generate_lib.Public += "td/generate"_idir;
        generate_lib.Public += tl;
        generate_lib.Public += utils;
    }
    auto &generate_common = td.addTarget<ExecutableTarget>("generate.common");
    {
        generate_common +=
            "td/generate/generate_common.cpp"
            ;
        generate_common.Public += generate_lib;
    }
    auto &generate_mtproto = td.addTarget<ExecutableTarget>("generate.mtproto");
    {
        generate_mtproto +=
            "td/generate/generate_mtproto.cpp"
            ;
        generate_mtproto.Public += generate_lib;
    }

    auto &tl_parser = td.addTarget<ExecutableTarget>("generate.tl_parser");
    {
        tl_parser +=
            "td/generate/tl-parser/.*"_rr;
    }

    auto setup_bigobj = [](auto &t)
    {
        if (t.getCompilerType() == CompilerType::MSVC)
            t.CompileOptions.push_back("-bigobj");
        //else if (t.getCompilerType() != CompilerType::GNU)
            //t.CompileOptions.push_back("-Wa,-mbig-obj");
    };

    auto &scheme = td.addTarget<StaticLibraryTarget>("scheme");
    {
        auto &t = scheme;
        t +=
            "td/tl/.*\\.h"_rr;
        t.Public += utils;
        t.Public += "."_idir;
        t.Public += IncludeDirectory(t.BinaryDir / "auto");
        setup_bigobj(t);

        {
            auto generate_tlo = [&t, &tl_parser](const String &name)
            {
                auto c = t.addCommand();
                c << cmd::prog(tl_parser)
                    << "-e"
                    << cmd::out("auto/tlo/" + name + ".tlo")
                    << cmd::in("td/generate/scheme/" + name + ".tl")
                    << cmd::end()
                    ;
            };
            generate_tlo("e2e_api");
            generate_tlo("td_api");
            generate_tlo("telegram_api");
            generate_tlo("secret_api");
            generate_tlo("mtproto_api");
        }

        auto add_file2 = [](auto &files, auto &&name) {
            files.insert(name + ".h"s);
            files.insert(name + ".cpp"s);
        };
        auto add_file3 = [](auto &files, auto &&name) {
            files.insert(name + ".h"s);
            files.insert(name + ".hpp"s);
            files.insert(name + ".cpp"s);
        };
        auto add_file10 = [](auto &files, auto &&name) {
            files.insert(name + ".h"s);
            files.insert(name + ".hpp"s);
            for (int i = 0; i < 10; ++i) {
                files.insert(std::format("{}_{}.cpp", name, i));
            }
        };

        {

            Files tl_td_auto_common;
            add_file3(tl_td_auto_common, "auto/td/telegram/e2e_api");
            add_file3(tl_td_auto_common, "auto/td/telegram/secret_api");
            add_file10(tl_td_auto_common, "auto/td/telegram/td_api");
            add_file10(tl_td_auto_common, "auto/td/telegram/telegram_api");

            t.addCommand()
                << cmd::prog(generate_common)
                << cmd::wdir(t.BinaryDir / "auto")
                << cmd::end()
                << cmd::in("auto/tlo/e2e_api.tlo")
                << cmd::in("auto/tlo/td_api.tlo")
                << cmd::in("auto/tlo/mtproto_api.tlo")
                << cmd::in("auto/tlo/telegram_api.tlo")
                << cmd::in("auto/tlo/secret_api.tlo")
                << cmd::out(tl_td_auto_common)
                ;

            Files tl_td_auto_mtproto{
                "auto/td/mtproto/mtproto_api.cpp",
                "auto/td/mtproto/mtproto_api.h",
                "auto/td/mtproto/mtproto_api.hpp",
            };
            t.addCommand()
                << cmd::prog(generate_mtproto)
                << cmd::wdir(t.BinaryDir / "auto")
                << cmd::end()
                << cmd::in("auto/tlo/e2e_api.tlo")
                << cmd::in("auto/tlo/td_api.tlo")
                << cmd::in("auto/tlo/mtproto_api.tlo")
                << cmd::in("auto/tlo/telegram_api.tlo")
                << cmd::in("auto/tlo/secret_api.tlo")
                << cmd::out(tl_td_auto_mtproto)
                ;
        }

        {
            Files tl_td_auto;
            add_file10(tl_td_auto, "auto/td/telegram/td_api_json");
            tl_td_auto.erase("auto/td/telegram/td_api_json.hpp");

            auto c = t.addCommand();
            c << cmd::prog(generate_json)
                << cmd::wdir(t.BinaryDir / "auto")
                << cmd::end()
                << cmd::in(t.BinaryDir / "auto/tlo/td_api.tlo")
                << cmd::out(tl_td_auto)
                ;
        }

        {
            Files tl_td_auto{
                "auto/td/telegram/td_tdc_api.cpp",
                "auto/td/telegram/td_tdc_api.h",
                "auto/td/telegram/td_tdc_api_inner.h",
            };

            auto c = t.addCommand();
            c << cmd::prog(generate_clib)
                << cmd::wdir(t.BinaryDir / "auto")
                << cmd::end()
                << cmd::in(t.BinaryDir / "auto/tlo/td_api.tlo")
                << cmd::out(tl_td_auto)
                ;
        }
    }

    auto &e2e = td.addTarget<StaticLibraryTarget>("e2e");
    {
        e2e += cppstd;
        e2e += "tde2e/td/e2e/.*"_rr;
        e2e.Public += "tde2e"_idir;
        e2e.Public += scheme;
    }

    auto &net = td.addTarget<StaticLibraryTarget>("net");
    {
        net += cppstd;
        net += "tdnet/td/net/.*"_rr;
        net.Public += "tdnet"_idir;
        net.Public += "org.sw.demo.openssl.ssl"_dep;
        net.Public += actor;
    }

    auto &memprof = td.addTarget<StaticLibraryTarget>("memprof");
    memprof += "memprof/.*"_rr;
    memprof.Public += utils;

    //
    auto &core = td.addTarget<StaticLibraryTarget>("core");
    {
        core += cppstd;
        core +=
            "td/mtproto/.*"_rr,
            "td/telegram/.*"_rr;
        core -=
            "td/telegram/ClientDotNet\\..*"_rr,
            "td/telegram/ClientJson\\..*"_rr,
            "td/telegram/Client\\..*"_rr,
            //"td/telegram/LogDotNet\\..*"_rr,
            "td/telegram/Log\\..*"_rr,
            "td/telegram/cli\\..*"_rr,
            "td/telegram/td_c_client\\..*"_rr,
            "td/telegram/td_emscripten\\..*"_rr,
            "td/telegram/td_json_client\\..*"_rr,
            "td/telegram/td_log\\..*"_rr;
        core.Public += "."_idir;
        setup_bigobj(core);
        if (core.getBuildSettings().TargetOS.Type != OSType::Windows)
            core.Public += "atomic"_slib;

        core.Public += scheme;
        core.Public += memprof;
        core.Public += net;
        core.Public += db;
        core.Public += e2e;
    }

    auto &client = td.addTarget<StaticLibraryTarget>("client");
    client +=
        "td/telegram/Client\\..*"_rr,
        "td/telegram/Log\\..*"_rr;
    client.Public += core;
}
