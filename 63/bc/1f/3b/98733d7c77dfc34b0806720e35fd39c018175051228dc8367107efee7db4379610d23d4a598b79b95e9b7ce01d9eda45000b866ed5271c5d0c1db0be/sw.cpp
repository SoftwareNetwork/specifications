void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.string_view_lite", "1.1.0");
    t += Git("https://github.com/martinmoene/string-view-lite", "v{v}");
}
