void build(Solution &s)
{
    auto &t = s.addTarget<Library>("zeromq.libzmq", "4.3.2");
    t += Git("https://github.com/zeromq/libzmq", "v{v}");

    t += sw::Shared, "DLL_EXPORT"_def;
    t.Public += sw::Static, "ZMQ_STATIC"_def;

    t.Variables["@ZMQ_CACHELINE_SIZE@"] = 64;

    t.Variables["ZMQ_USE_CV_IMPL_STL11"] = 1;
    //t.Variables["ZMQ_USE_CV_IMPL_WIN32API"] = 1;
    //t.Variables["ZMQ_USE_CV_IMPL_PTHREADS"] = 1;

    t.Variables["ZMQ_POLL_BASED_ON_POLL"] = 1;

    if (s.getBuildSettings().TargetOS.is(OSType::Windows))
    {
        t.Variables["ZMQ_IOTHREAD_POLLER_USE_SELECT"] = 1;
        t.Variables["ZMQ_HAVE_WINDOWS"] = 1;

        t += "ws2_32.lib"_slib, "Iphlpapi.lib"_slib, "Advapi32.lib"_slib;
    }
    else
    {
        t.Variables["ZMQ_IOTHREAD_POLLER_USE_EPOLL"] = 1;
        t.Variables["ZMQ_IOTHREAD_POLLER_USE_EPOLL_CLOEXEC"] = 1;
    }

    t.configureFile("builds/cmake/platform.hpp.in", "platform.hpp");
}
