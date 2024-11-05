void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("msgpack", "7.0.0");
    t += Git("https://github.com/msgpack/msgpack-c", "cpp-{v}");
    t.ApiName = "MSGPACK_DLLEXPORT";
}
