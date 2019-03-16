void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.invoke_lite", "master");
    t += Git("https://github.com/martinmoene/invoke-lite");
}
