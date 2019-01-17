void build(Solution &s)
{
    auto &p = s.addProject("google.breakpad", "master");
    p += Git("https://github.com/google/breakpad", "", "master");

    auto &windows = p.addTarget<StaticLibraryTarget>("common.windows");
    windows.setChecks("breakpad");

    windows +=
        "src/client/windows/common/.*\\.h"_rr,
        "src/common/[^/]*\\.h"_rr,
        "src/common/windows/.*\\.h"_rr,
        "src/common/windows/guid_string.*"_rr,
        "src/google_breakpad/.*"_rr;

    windows -=
        ".*_unittest.cc"_rr;

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        windows.Public += "UNICODE"_d;
    }

    //
    auto &client = p.addTarget<StaticLibraryTarget>("client.windows.crash_generation.client");
    client.setChecks("breakpad");

    client +=
        "src/client/windows/crash_generation/crash_generation_client.*"_rr;

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        client.Public += "UNICODE"_d;
    }

    client.Public += windows;

    //
    auto &server = p.addTarget<StaticLibraryTarget>("client.windows.crash_generation.server");
    server.setChecks("breakpad");

    server +=
        "src/client/windows/crash_generation/client_info.*"_rr,
        "src/client/windows/crash_generation/crash_generation_server.*"_rr,
        "src/client/windows/crash_generation/minidump_generator.*"_rr;

    server.Public +=
        "src/client/windows/crash_generation"_id;

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        server.Public += "UNICODE"_d;
    }

    server.Public += client;

    //
    auto &handler = p.addTarget<StaticLibraryTarget>("client.windows.handler");
    handler.setChecks("breakpad");

    handler +=
        "src/client/windows/handler/exception_handler.cc",
        "src/client/windows/handler/exception_handler.h";

    handler.Public +=
        "src/client/windows/handler"_id;

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        handler.Public += "UNICODE"_d;
    }

    handler.Public += client;
}

void check(Checker &c)
{
    auto &s = c.addSet("breakpad");
    s.checkFunctionExists("arc4random");
    s.checkFunctionExists("getrandom");
    s.checkIncludeExists("a.out.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("sys/random.h");
    s.checkTypeSize("size_t");
    s.checkTypeSize("void *");
    s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <float.h>
    int main() {return 0;}
    )sw_xxx");
}
