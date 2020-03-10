void build(Solution &s)
{
    auto &p = s.addProject("mingw.w64.libraries", "master");
    p += Git("https://git.code.sf.net/p/mingw-w64/mingw-w64");

    auto &t = p.addLibrary("winpthreads");
    {
        t.setSourceDirectory("mingw-w64-libraries/winpthreads");

        t.ApiName = "WINPTHREAD_API";

        t += "include/.*"_rr;
        t += "src/.*"_rr;

        t["src/cond.c"].args.push_back("-D_TIMESPEC_DEFINED");
        t["src/clock.c"].args.push_back("-D_TIMESPEC_DEFINED");
        t["src/nanosleep.c"].args.push_back("-D_TIMESPEC_DEFINED");

        t.patch("include/pthread.h", "#if defined DLL_EXPORT", "/*#if  defined DLL_EXPORT");
        t.patch("include/pthread.h", "/* #define WINPTHREAD_DBG 1 */", "*//*  #define WINPTHREAD_DBG 1 */");
        t.patch("include/pthread.h",
            "int    WINPTHREAD_API pthread_delay_np (const struct timespec *interval);",
            "//int     WINPTHREAD_API pthread_delay_np (const struct timespec *interval);");
        t.patch("include/pthread.h",
            "#endif /* WIN_PTHREADS_H */",
            "int     WINPTHREAD_API pthread_delay_np (const struct timespec *interval);\n#endif  /* WIN_PTHREADS_H */"
        );
    }
}
