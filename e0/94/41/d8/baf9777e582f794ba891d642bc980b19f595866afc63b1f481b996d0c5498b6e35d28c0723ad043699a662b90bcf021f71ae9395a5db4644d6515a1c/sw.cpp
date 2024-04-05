void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("msgpack", "6.1.1");
    t += Git("https://github.com/msgpack/msgpack-c", "cpp-{v}");
    t.ApiName = "MSGPACK_DLLEXPORT";
}
