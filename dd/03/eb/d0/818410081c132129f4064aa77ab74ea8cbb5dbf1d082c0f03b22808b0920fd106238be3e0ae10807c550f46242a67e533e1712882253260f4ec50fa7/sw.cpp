void build(Solution &s)
{
    auto &lib = s.addTarget<LibraryTarget>("emcrisostomo.fswatch.lib", "1.14.0");
    lib += Git("https://github.com/emcrisostomo/fswatch");

    lib.setRootDirectory("libfswatch");
    lib += "src/libfswatch"_id;

    lib += "HAVE_CXX_MUTEX"_def;

    lib += "HAVE_WINDOWS"_def;
    lib -= "src/libfswatch/c++/poll_monitor.cpp";
    lib -= "src/libfswatch/c++/fsevents_monitor.cpp";
    lib -= "src/libfswatch/c++/inotify_monitor.cpp";
    lib -= "src/libfswatch/c++/kqueue_monitor.cpp";

    lib.Public += "org.sw.demo.tronkko.dirent-master"_dep;
    lib.Public += "org.sw.demo.kimgr.getopt_port-master"_dep;
}
