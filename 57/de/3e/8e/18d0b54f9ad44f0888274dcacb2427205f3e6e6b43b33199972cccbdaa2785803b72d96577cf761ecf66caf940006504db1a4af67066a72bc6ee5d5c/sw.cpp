void build(Solution &s)
{
    auto &lua = s.addTarget<LibraryTarget>("lua", "5.5.0");
    lua += RemoteFile("https://www.lua.org/ftp/lua-{v}.tar.gz");

    if (lua.getBuildSettings().TargetOS.Type != OSType::Windows)
        lua.ExportAllSymbols = true;

    lua +=
        "src/.*\\.h.*"_rr,
        "src/lapi.c",
        "src/lauxlib.c",
        "src/lbaselib.c",
        "src/lcode.c",
        "src/lcorolib.c",
        "src/lctype.c",
        "src/ldblib.c",
        "src/ldebug.c",
        "src/ldo.c",
        "src/ldump.c",
        "src/lfunc.c",
        "src/lgc.c",
        "src/linit.c",
        "src/liolib.c",
        "src/llex.c",
        "src/lmathlib.c",
        "src/lmem.c",
        "src/loadlib.c",
        "src/lobject.c",
        "src/lopcodes.c",
        "src/loslib.c",
        "src/lparser.c",
        "src/lstate.c",
        "src/lstring.c",
        "src/lstrlib.c",
        "src/ltable.c",
        "src/ltablib.c",
        "src/ltm.c",
        "src/lundump.c",
        "src/lutf8lib.c",
        "src/lvm.c",
        "src/lzio.c";

    lua.Public +=
        "src"_id;

    lua.Private += "LUA_LIB"_d;
    if (lua.getBuildSettings().TargetOS.Type == OSType::Windows)
        lua.Public += sw::Shared, "LUA_BUILD_AS_DLL"_d;
}
