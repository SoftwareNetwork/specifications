void build(Solution &s)
{
    auto &ns = s.addStaticLibrary("google.nsync", "1.20.1");
    ns += Git("https://github.com/google/nsync", "{v}");

    auto &apply_files = [](auto &t)
    {
        t -=
            ".*test.*.cc"_rr,
            ".*/test_.*.cc"_rr
            ;
    };

    ns += "internal/.*"_rr;
    ns += "public/.*"_rr;

    ns -= "platform/.*"_rr;
    ns += "platform/c\\+\\+11/.*"_rr;

    /*ns +=
        "platform/posix/src/nsync_panic.c",
        "platform/posix/src/per_thread_waiter.c",
        "platform/posix/src/time_rep.c",
        "platform/posix/src/yield.c";
    ns +=
        "platform/c++11/src/per_thread_waiter.cc",
        "platform/c++11/src/yield.cc",
        "platform/c++11/src/time_rep_timespec.cc",
        "platform/c++11/src/nsync_panic.cc";*/

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        ns += "platform/win32/.*"_rr;
        ns += "platform/c++11/src/nsync_semaphore_mutex.cc";
        ns -= "platform/win32/src/pthread_cond_timedwait_win32.c";
        ns += "platform/c++11"_idir;
        ns += "platform/win32"_idir;
    }
    if (s.Settings.Native.CompilerType == CompilerType::MSVC)
    {
        ns += "platform/msvc/.*"_rr;
        ns += "platform/msvc"_idir;
    }
    if (s.Settings.TargetOS.Arch == ArchType::x86)
    {
        ns += "platform/x86_32/.*"_rr;
        ns += "platform/x86_32"_idir;
    }
    if (s.Settings.TargetOS.Arch == ArchType::x86_64)
    {
        ns += "platform/x86_64/.*"_rr;
        ns += "platform/x86_64"_idir;
    }
    apply_files(ns);

    ns += "platform/posix"_idir;
    ns.Public += "public"_idir;
    ns += "internal"_idir;

    ns +=
        "NSYNC_USE_CPP11_TIMEPOINT"_def,
        "NSYNC_ATOMIC_CPP11"_def;

    for (auto *f : ns.gatherSourceFiles())
        f->BuildAs = NativeSourceFile::CPP;
}
