void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.status_value_lite", "1.0.0");
    t += Git("https://github.com/martinmoene/status-value-lite", "v{v}");
}
