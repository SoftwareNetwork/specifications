void build(Solution &s)
{
    auto &t = s.addLibrary("facebook.experimental.libunifex", "master");
    t += Git("https://github.com/facebookexperimental/libunifex");

    t += cpplatest;

    // the configured options and settings for unifex
    /*#cmakedefine01 UNIFEX_NO_MEMORY_RESOURCE
    #cmakedefine UNIFEX_MEMORY_RESOURCE_HEADER <@UNIFEX_MEMORY_RESOURCE_HEADER@>
    #cmakedefine UNIFEX_MEMORY_RESOURCE_NAMESPACE @UNIFEX_MEMORY_RESOURCE_NAMESPACE@
    #cmakedefine01 UNIFEX_NO_COROUTINES
    #cmakedefine UNIFEX_COROUTINES_HEADER <@UNIFEX_COROUTINES_HEADER@>
    #cmakedefine UNIFEX_COROUTINES_NAMESPACE @UNIFEX_COROUTINES_NAMESPACE@*/

    t.Variables["libunifex_VERSION_MAJOR"] = 0;
    t.Variables["libunifex_VERSION_MINOR"] = 1;
    t.Variables["UNIFEX_NO_EPOLL"] = 1;
    t.configureFile("include/unifex/config.hpp.in", "unifex/config.hpp");
}
