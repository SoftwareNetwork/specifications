void build(Solution &s)
{
    auto &t = s.addTarget<Library>("zeromq.cppzmq", "4.11.0");
    t += Git("https://github.com/zeromq/cppzmq", "v{v}");

    t += ".*\\.hpp"_r;
    t.Public += "org.sw.demo.zeromq.cppzmq"_dep;
}
