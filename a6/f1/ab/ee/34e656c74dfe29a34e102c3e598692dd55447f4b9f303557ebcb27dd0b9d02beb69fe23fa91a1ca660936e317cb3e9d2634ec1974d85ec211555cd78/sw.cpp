void build(Solution &s)
{
    auto &p = s.addProject("eteran.cpp_utilities", "master");
    p += Git("https://github.com/eteran/cpp-utilities");

    for (auto d : {
             "algorithm",
             "arena",
             "bitset",
             "bitwise",
             "container",
             "fixed",
             "hash",
             "logger",
             "pprint",
             "range",
             "string",
             "thread_pool",
             "uint128",
             "uuid",
         })
    {
        auto &t = p.addTarget<StaticLibrary>(d);
        t.setRootDirectory(d);
    }
}
