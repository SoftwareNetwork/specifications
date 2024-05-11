static int merge_files(path i1, path i2, path o)
{
    auto s1 = read_file(i1);
    auto s2 = read_file(i2);
    write_file(o, s1 + "\n" + s2);
    return 0;
}
SW_DEFINE_VISIBLE_FUNCTION_JUMPPAD2(merge_files)

void build(Solution &s)
{
    auto &sql = s.addProject("sqlcipher", "4.5.7");
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
                << ("-T" + to_printable_string(lempar))
                << cmd::in(parse2y)
                << cmd::end()
                << cmd::out(parse2)
                << cmd::out(parse2h);
            copy(sqlcipher, parse2h, parseh);
        }

        {
            auto c = sqlcipher.addCommand(SW_VISIBLE_FUNCTION(merge_files));
            c << cmd::in(parseh);
            c << cmd::in("src/vdbe.c");
            c << cmd::out(tmp1);
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
                << ("-T" + to_printable_string(lempar))
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
            sqlcipher.patch("tool/mksqlite3h.tcl", "$PWD/mksourceid", to_printable_string(normalize_path(mksourceid.getOutputFile())));

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

        sqlcipher.pushFrontToFileOnce("ext/rtree/rtree.c", "#include <stdlib.h>");
        sqlcipher.pushFrontToFileOnce("src/crypto.c", "#include <sqliteInt.h>");
        sqlcipher.pushFrontToFileOnce("src/crypto_impl.c", "#include <sqliteInt.h>");
        sqlcipher.pushFrontToFileOnce("src/crypto_openssl.c", "#include <sqliteInt.h>\n#include \"sqlcipher.h\"");
    }

    auto &shell = sql.addTarget<ExecutableTarget>("shell");
    {
        shell += sqlcipher;

        shell +=
            "SQLITE_ENABLE_JSON1"_def,
            "SQLITE_ENABLE_FTS4"_def,
            //SQLITE_ENABLE_FTS5
            "SQLITE_ENABLE_RTREE"_def,
            "SQLITE_ENABLE_EXPLAIN_COMMENTS"_def,
            "SQLITE_ENABLE_UNKNOWN_SQL_FUNCTION"_def,
            "SQLITE_ENABLE_STMTVTAB"_def,
            "SQLITE_ENABLE_DBPAGE_VTAB"_def,
            "SQLITE_ENABLE_DBSTAT_VTAB"_def,
            "SQLITE_ENABLE_BYTECODE_VTAB"_def,
            "SQLITE_ENABLE_OFFSET_SQL_FUNC"_def,
            "SQLITE_ENABLE_DESERIALIZE"_def
            ;

        auto sqlcipher_src = sqlcipher.SourceDir;
        auto sqlcipher_bdir = to_printable_string(normalize_path(sqlcipher.BinaryDir));
        auto sqlcipher_bpdir = to_printable_string(normalize_path(sqlcipher.BinaryPrivateDir));

        {
            auto c = shell.addCommand();
            c << cmd::prog("org.sw.demo.tcl.sh"_dep)
                << cmd::wdir(shell.SourceDir)
                << cmd::in("tool/mkshellc.tcl")
                << cmd::std_out("shell.c")
                << cmd::end()
                << cmd::in("src/shell.c.in")
                << cmd::in("ext/misc/appendvfs.c", cmd::Skip)
                << cmd::in("ext/misc/completion.c", cmd::Skip)
                << cmd::in("ext/misc/decimal.c", cmd::Skip)
                << cmd::in("ext/misc/fileio.c", cmd::Skip)
                << cmd::in("ext/misc/ieee754.c", cmd::Skip)
                << cmd::in("ext/misc/series.c", cmd::Skip)
                << cmd::in("ext/misc/shathree.c", cmd::Skip)
                << cmd::in("ext/misc/sqlar.c", cmd::Skip)
                << cmd::in("ext/misc/uint.c", cmd::Skip)
                << cmd::in("ext/expert/sqlite3expert.c", cmd::Skip)
                << cmd::in("ext/expert/sqlite3expert.h", cmd::Skip)
                << cmd::in("ext/misc/zipfile.c", cmd::Skip)
                << cmd::in("ext/misc/memtrace.c", cmd::Skip)
                << cmd::in("src/test_windirent.c", cmd::Skip)
                ;
        }
        {
            shell.patch("tool/mksqlite3c.tcl", "tsrc/sqlite3.h", sqlcipher_bdir + "/sqlite3.h");
            shell.patch("tool/mksqlite3c.tcl", "tsrc/parse.c", sqlcipher_bpdir + "/parse2.c");
            // todo: replace other tsrc->...
            // or find another way
            // because tsrc/sqlite3.h is met for the second time

            auto c = shell.addCommand();
            c << cmd::prog("org.sw.demo.tcl.sh"_dep)
                << cmd::wdir(shell.SourceDir)
                << cmd::in("tool/mksqlite3c.tcl")
                << cmd::end()
                << cmd::out("sqlite3.c")
                ;
        }
    }
}
