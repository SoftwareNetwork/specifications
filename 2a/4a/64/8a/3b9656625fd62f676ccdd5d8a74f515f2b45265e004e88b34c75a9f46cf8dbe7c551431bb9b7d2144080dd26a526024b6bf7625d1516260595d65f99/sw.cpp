void build(Solution &s)
{
    auto &winflexbison = s.addProject("lexxmark.winflexbison", "master");
    winflexbison += Git("https://github.com/lexxmark/winflexbison", "", "master");

    auto &common = winflexbison.addTarget<StaticLibraryTarget>("common");
    common += "common/.*"_rr;
    common -= "common/m4/lib/regcomp.c";
    common -= "common/m4/lib/regexec.c";
    common -= ".*\\.def"_rr;
    common.Public += "common/m4/lib"_idir;
    common.Public += "common/misc"_idir;

    auto &flex = winflexbison.addTarget<ExecutableTarget>("flex");
    flex += "flex/.*"_rr;
    flex -= "flex/src/libmain.c";
    flex -= "flex/src/libyywrap.c";
    flex += common;

    auto &bison = winflexbison.addTarget<ExecutableTarget>("bison");
    bison -= "bison/data/.*"_rr;
    bison += "bison/data/m4sugar/.*"_rr;
    bison += "bison/src/.*"_rr;
    bison.Public += "bison/src"_idir;
    bison += common;
    bison.replaceInFileOnce("bison/src/config.h", "data", normalize_path(bison.SourceDir / "bison/data/"));
    bison.replaceInFileOnce("bison/src/main.c", "if (!last_divider)", "");
    bison.replaceInFileOnce("bison/src/main.c", "free(local_pkgdatadir);", "");
}
