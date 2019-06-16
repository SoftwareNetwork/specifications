void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("RichieSams.FiberTaskingLib", "master");
    t += Git("https://github.com/RichieSams/FiberTaskingLib");

    t += "include/.*"_rr;
    t += "source/.*"_rr;
    t += "third_party/boost_context/include/.*"_rr;

    t.Public += "include"_id;
    t.Public += "third_party/boost_context/include"_id;
}
