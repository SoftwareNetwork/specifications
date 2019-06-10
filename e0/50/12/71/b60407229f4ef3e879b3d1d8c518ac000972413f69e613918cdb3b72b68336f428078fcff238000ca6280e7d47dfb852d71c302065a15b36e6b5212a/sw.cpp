void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("davidstone.concurrent_queue", "master");
    t += Git("https://github.com/davidstone/concurrent-queue");

    t.Public += "org.sw.demo.boost.program_options"_dep;
    t.Public += "org.sw.demo.boost.thread"_dep;
}
