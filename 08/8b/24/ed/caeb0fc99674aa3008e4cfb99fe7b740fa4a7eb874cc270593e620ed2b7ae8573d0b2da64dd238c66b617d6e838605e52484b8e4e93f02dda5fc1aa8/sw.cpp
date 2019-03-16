void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.optional_fun_lite", "master");
    t += Git("https://github.com/martinmoene/optional-fun-lite");
}
