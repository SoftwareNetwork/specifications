void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.string_view_lite", "master");
    t += Git("https://github.com/martinmoene/string-view-lite");
}
