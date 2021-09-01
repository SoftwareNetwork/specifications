void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("msgpack", "4.0.2");
    t += Git("https://github.com/msgpack/msgpack-c", "cpp-{v}");
    t.ApiName = "MSGPACK_DLLEXPORT";
}