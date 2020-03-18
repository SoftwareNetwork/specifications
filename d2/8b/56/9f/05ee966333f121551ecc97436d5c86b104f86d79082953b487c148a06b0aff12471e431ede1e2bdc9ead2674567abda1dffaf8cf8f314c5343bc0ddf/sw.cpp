void build(Solution &s)
{
    auto &sql = s.addProject("sqlcipher", "4.3.0");
    sql += Git("https://github.com/sqlcipher/sqlcipher", "v{v}");

    auto &lemon = sql.addTarget<ExecutableTarget>("lemon");
    lemon += "tool/lemon.c";
    lemon -= "tool/lempar.c";

    auto &mkkeywordhash = sql.addTarget<ExecutableTarget>("mkkeywordhash");
    //mkkeywordhash.Scope = TargetScope::Tool;
    mkkeywordhash += "tool/mkkeywordhash.c";
    mkkeywordhash.Private += "SQLCIPHER_CRYPTO_OPENSSL=1"_d;

    auto &mksourceid = sql.addTarget<ExecutableTarget>("mksourceid");
    //mkkeywordhash.Scope = TargetScope::Tool;
    mksourceid += "tool/mksourceid.c";

    auto &sqlcipher = sql.addTarget<LibraryTarget>("sqlcipher");
    {
        sqlcipher.ApiNames.insert("SQLITE_API");
        sqlcipher +=
            "VERSION",
            "ext/async/.*"_rr,
            "ext/fts3/.*"_rr,
            "ext/fts5/.*"_rr,
            "ext/rtree/.*"_rr,
            "ext/session/.*"_rr,
            "ext/userauth/.*"_rr,
            "manifest.*"_rr,
            "src/.*"_rr,
            "tool/.*"_rr;

        sqlcipher -=
            ".*rc$"_rr,
            "ext/session/.*test.*"_rr,
            "ext/session/changeset.*"_rr,
            "ext/rtree/geopoly.*"_rr,
            "src/tcl.*"_rr,
            "src/test.*"_rr,
            "tool/.*"_rr;

        sqlcipher.Public +=
            "src"_id,
            "ext/fts3"_id,
            "ext/rtree"_id,
            "ext/fts5"_id;

        sqlcipher.Private += "BUILD_sqlite"_d;
        sqlcipher.Private += "SQLCIPHER_CRYPTO_OPENSSL"_d;
        sqlcipher.Private += "SQLITE_CORE"_d;
        sqlcipher.Private += "SQLITE_ENABLE_ASYNCIO"_d;
        sqlcipher.Private += "SQLITE_ENABLE_FTS3"_d;
        sqlcipher.Private += "SQLITE_ENABLE_FTS5"_d;
        sqlcipher.Private += "SQLITE_ENABLE_RTREE"_d;
        sqlcipher.Private += "SQLITE_ENABLE_GEOPOLY"_d;
        sqlcipher.Private += "SQLITE_ENABLE_SESSION"_d;
        sqlcipher.Private += "SQLITE_HAS_CODEC"_d;
        sqlcipher.Private += "SQLITE_THREADSAFE"_d;
        sqlcipher.Private += "SQLITE_USER_AUTHENTICATION"_d;
        if (sqlcipher.getBuildSettings().TargetOS.Type != OSType::Windows)
        {
            sqlcipher.Public += "dl"_slib;
        }

        if (sqlcipher.getBuildSettings().Native.ConfigurationType == ConfigurationType::Debug)
            sqlcipher += "SQLITE_DEBUG"_d;

        sqlcipher.Public += "org.sw.demo.openssl.crypto"_dep;
        sqlcipher.Public += "org.sw.demo.tcl.lib"_dep;
        sqlcipher.addDummyDependency(mksourceid);

        sqlcipher.patch("ext/fts5/fts5_unicode2.c", "u8", "unsigned char");
        sqlcipher.patch("ext/fts5/fts5_unicode2.c", "u16", "unsigned short");
        sqlcipher.patch("ext/fts5/fts5_unicode2.c", "u32", "unsigned int");

        auto copy = [](auto &t, const path &in, const path &out)
        {
            auto c = t.addCommand(SW_VISIBLE_BUILTIN_FUNCTION(copy_file));
            c << cmd::in(in) << cmd::out(out);
            return c;
        };

        auto lempar = lemon.SourceDir / "tool/lempar.c";
        auto parse2y = sqlcipher.BinaryPrivateDir / "parse2.y";
        auto parse2 = sqlcipher.BinaryPrivateDir / "parse2.c";
        auto parse2h = sqlcipher.BinaryPrivateDir / "parse2.h";
        auto parse = sqlcipher.BinaryPrivateDir / "parse.c";
        auto parseh = sqlcipher.BinaryPrivateDir / "parse.h";
        auto opcodesh = sqlcipher.BinaryPrivateDir / "opcodes.h";
        auto opcodes = sqlcipher.BinaryPrivateDir / "opcodes.c";
        auto tmp1 = sqlcipher.BinaryPrivateDir / "tmp1";
        auto keywordhashh = sqlcipher.BinaryPrivateDir / "keywordhash.h";
        auto sqlite3h = sqlcipher.BinaryDir / "sqlite3.h";
        auto fts5parse2y = sqlcipher.BinaryPrivateDir / "fts5parse2.y";
        auto fts5parse2 = sqlcipher.BinaryPrivateDir / "fts5parse2.c";
        auto fts5parse2h = sqlcipher.BinaryPrivateDir / "fts5parse2.h";
        auto fts5parseh = sqlcipher.BinaryPrivateDir / "fts5parse.h";

        copy(sqlcipher, sqlcipher.SourceDir / "src/parse.y", parse2y);
        copy(sqlcipher, sqlcipher.SourceDir / "ext/fts5/fts5parse.y", fts5parse2y);

        {
            auto c = sqlcipher.addCommand();
            c << cmd::prog(lemon)
                << "-s" << "-DSQLITE_ENABLE_FOO=1" << "-DSQLITE_OMIT_FOO=1"
                << ("-T" + lempar.u8string())
                << cmd::in(parse2y)
                << cmd::end()
                << cmd::out(parse2)
                << cmd::out(parse2h);
            copy(sqlcipher, parse2h, parseh);
        }

        if (sqlcipher.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            auto c = sqlcipher.addCommand();
            c << "cmd"
                << "/c"
                << ("type " + normalize_path_windows(parseh) + " " + normalize_path_windows(sqlcipher.SourceDir / "src/vdbe.c"))
                << cmd::std_out(tmp1)
                << cmd::end()
                << cmd::in(parseh)
                //<< cmd::in("src/vdbe.c")
                ;
        }
        else
        {
            auto c = sqlcipher.addCommand();
            c << "cat"
                << cmd::in(parseh)
                << cmd::in("src/vdbe.c")
                << cmd::std_out(tmp1);
        }

        {
            auto c = sqlcipher.addCommand();
            c << cmd::prog(mkkeywordhash)
                << cmd::std_out(keywordhashh);
        }

        {
            auto c = sqlcipher.addCommand();
            c << cmd::prog(lemon)
                << "-s" << "-DSQLITE_ENABLE_FOO=1" << "-DSQLITE_OMIT_FOO=1"
                << ("-T" + lempar.u8string())
                << cmd::in(fts5parse2y)
                << cmd::end()
                << cmd::out(fts5parse2)
                << cmd::out(fts5parse2h);
            copy(sqlcipher, fts5parse2h, fts5parseh);
            sqlcipher += fts5parseh;
        }

        {
            auto c = sqlcipher.addCommand();
            c << cmd::prog("org.sw.demo.tcl.sh"_dep)
                << cmd::in("tool/mkopcodeh.tcl")
                << cmd::std_in(tmp1)
                << cmd::std_out(opcodesh);
        }

        {
            auto c = sqlcipher.addCommand();
            c << cmd::prog("org.sw.demo.tcl.sh"_dep)
                << cmd::in("tool/mkopcodec.tcl")
                << cmd::in(opcodesh)
                << cmd::std_out(opcodes);
        }

        {
            sqlcipher.patch("tool/mksqlite3h.tcl", "$PWD/mksourceid", normalize_path(mksourceid.getOutputFile()));

            auto c = sqlcipher.addCommand();
            c << cmd::prog("org.sw.demo.tcl.sh"_dep)
                << cmd::in("tool/mksqlite3h.tcl")
                << normalize_path(sqlcipher.SourceDir)
                << cmd::std_out(sqlite3h)
                << cmd::end()
                << cmd::in("src/sqlite.h.in")
                << cmd::in(mksourceid.getOutputFile())
                ;
        }

        sqlcipher.pushFrontToFileOnce("src/crypto.c", "#include <sqlite3.h>");
        sqlcipher.pushFrontToFileOnce("src/crypto_impl.c", "#include <sqlite3.h>");
        sqlcipher.pushFrontToFileOnce("src/crypto_openssl.c", "#include <sqlite3.h>\n#include \"sqlcipher.h\"");
    }
}
