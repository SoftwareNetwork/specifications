void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.expected_lite", "0.2.0");
    t += Git("https://github.com/martinmoene/expected-lite", "v{v}");
}
