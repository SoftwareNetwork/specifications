void build(Solution &s)
{
    auto &t = s.addLibrary("SanderMertens.flecs", "1.3.0");
    t += Git("https://github.com/SanderMertens/flecs", "v{M}.{m}");

    t += "FLECS_IMPL"_def;
    t.Public += sw::Static, "FLECS_STATIC"_def;
}
