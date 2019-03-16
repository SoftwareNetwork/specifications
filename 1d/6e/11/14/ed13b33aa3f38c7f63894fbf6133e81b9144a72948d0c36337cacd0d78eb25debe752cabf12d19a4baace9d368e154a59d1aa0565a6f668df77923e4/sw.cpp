void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.value_ptr_lite", "0.1.0");
    t += Git("https://github.com/martinmoene/value-ptr-lite", "v{v}");
}
