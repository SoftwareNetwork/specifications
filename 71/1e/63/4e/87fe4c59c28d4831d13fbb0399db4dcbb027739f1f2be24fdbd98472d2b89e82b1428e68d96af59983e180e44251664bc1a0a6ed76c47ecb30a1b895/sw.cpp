void build(Solution &s)
{
    auto &t = s.addStaticLibrary("catchorg.catch2", "3.9.1");
    t += Git("https://github.com/catchorg/Catch2", "v{v}");

    t += cpp17;
    t.WholeArchive = true;
    t += "src/.*"_rr;

    //t.Variables["CATCH_CONFIG_ANDROID_LOGWRITE"] = "";
    //t.Variables["CATCH_CONFIG_COLOUR_WIN32"] = "";
    //t.Variables["CATCH_CONFIG_COUNTER"] = "";

    //t.Variables["CATCH_CONFIG_CPP11_TO_STRING"] = "";
    //t.Variables["CATCH_CONFIG_CPP17_BYTE"] = "";
    //t.Variables["CATCH_CONFIG_CPP17_OPTIONAL"] = "";
    //t.Variables["CATCH_CONFIG_CPP17_STRING_VIEW"] = "";
    //t.Variables["CATCH_CONFIG_CPP17_UNCAUGHT_EXCEPTIONS"] = "";
    //t.Variables["CATCH_CONFIG_CPP17_VARIANT"] = "";

    t.Variables["CATCH_CONFIG_DEFAULT_REPORTER"] = "console";
    t.Variables["CATCH_CONFIG_CONSOLE_WIDTH"] = "80";
    //t.Variables["CATCH_CONFIG_FALLBACK_STRINGIFIER"] = "";
    t.configureFile("src/catch2/catch_user_config.hpp.in", "catch2/catch_user_config.hpp");
}
