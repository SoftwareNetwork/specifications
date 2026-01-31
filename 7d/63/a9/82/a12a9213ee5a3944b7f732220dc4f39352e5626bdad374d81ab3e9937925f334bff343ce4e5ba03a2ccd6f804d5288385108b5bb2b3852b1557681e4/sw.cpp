void build(Solution &s)
{
    auto &t = s.addStaticLibrary("akheron.jansson", "2.15.0");
    t += Git("https://github.com/akheron/jansson");

    t.setChecks("jansson");

    t.Variables["JSON_INLINE"] = "";
    t.Variables["JSON_INT_T"] = "int";
    //t.Variables["JSON_STRTOINT"] = "int";
    //t.Variables["JSON_INTEGER_FORMAT"] = "int";
    t.Variables["JSON_HAVE_LOCALECONV"] = 0;
    t.Variables["JSON_HAVE_ATOMIC_BUILTINS"] = 0;
    t.Variables["JSON_HAVE_SYNC_BUILTINS"] = 0;

    t.configureFile("cmake/jansson_config.h.cmake", "jansson_config.h");
    t.configureFile("cmake/jansson_private_config.h.cmake", "jansson_private_config.h");
}

void check(Checker &c)
{
    auto &s = c.addSet("jansson");
    s.checkFunctionExists("localeconv"); // add locale.h
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("locale.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("sys/types.h");
}
