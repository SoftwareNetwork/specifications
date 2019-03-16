void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.value_ptr_lite", "master");
    t += Git("https://github.com/martinmoene/value-ptr-lite");
}
