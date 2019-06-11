void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("msgpack", "3.2.0");
    t += Git("https://github.com/msgpack/msgpack-c", "cpp-{v}");
    t.ApiName = "MSGPACK_DLLEXPORT";
}
