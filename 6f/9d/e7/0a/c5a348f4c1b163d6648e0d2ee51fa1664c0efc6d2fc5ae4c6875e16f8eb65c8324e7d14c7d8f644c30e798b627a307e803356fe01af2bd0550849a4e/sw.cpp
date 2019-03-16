void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.expected_lite", "master");
    t += Git("https://github.com/martinmoene/expected-lite");
}
