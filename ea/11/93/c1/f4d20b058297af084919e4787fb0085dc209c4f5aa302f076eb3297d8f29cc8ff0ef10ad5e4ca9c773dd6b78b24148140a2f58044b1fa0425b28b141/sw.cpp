void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.status_value_lite", "master");
    t += Git("https://github.com/martinmoene/status-value-lite");
}
