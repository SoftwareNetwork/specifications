void build(Solution &s)
{
    auto &t = s.addLibrary("akheron.jansson", "2.12.0");
    t += Git("https://github.com/akheron/jansson", "v{M}.{m}");

    t.setChecks("jansson", true);

    t += "src/.*"_rr;

    if (t.getBuildSettings().TargetOS.is(OSType::Windows))
        t += "USE_WINDOWS_CRYPTOAPI"_def;
    else if (t.getBuildSettings().TargetOS.is(OSType::Linux))
        t += "USE_URANDOM"_def;

    t.Variables["json_have_long_long"] = 1;
    t.Variables["json_have_atomic_builtins"] = 0;
    t.Variables["json_have_sync_builtins"] = 0;
    t.Variables["json_have_localeconv"] = 0;

    t.configureFile("src/jansson_config.h.in", "jansson_config.h");
    t.writeFileOnce("jansson_private_config.h");
}

void check(Checker &c)
{
    auto &s = c.addSet("jansson");
    s.checkIncludeExists("endian.h");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("locale.h");
    s.checkIncludeExists("sched.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("sys/param.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/types.h");
    s.checkTypeSize("int32_t");
    s.checkTypeSize("uint32_t");
    s.checkTypeSize("uint16_t");
    s.checkTypeSize("uint8_t");
    s.checkTypeSize("long_long_int");
    s.checkFunctionExists("close");
    s.checkFunctionExists("getpid");
    s.checkFunctionExists("gettimeofday");
    s.checkFunctionExists("localeconv");
    s.checkFunctionExists("open");
    s.checkFunctionExists("read");
    s.checkFunctionExists("sched_yield");
    s.checkFunctionExists("strtoll");
}
