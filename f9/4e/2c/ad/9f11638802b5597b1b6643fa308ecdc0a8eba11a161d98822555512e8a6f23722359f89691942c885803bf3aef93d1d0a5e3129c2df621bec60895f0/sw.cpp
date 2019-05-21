void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("ricab.scope_guard", "master");
    t += Git("https://github.com/ricab/scope_guard");
    t += "scope_guard.hpp";
}
