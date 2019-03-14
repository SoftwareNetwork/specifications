void build(Solution &s)
{
    auto &p = s.addStaticLibrary("SanderMertens.flecs", "master");
    p += Git("https://github.com/SanderMertens/flecs");
}
