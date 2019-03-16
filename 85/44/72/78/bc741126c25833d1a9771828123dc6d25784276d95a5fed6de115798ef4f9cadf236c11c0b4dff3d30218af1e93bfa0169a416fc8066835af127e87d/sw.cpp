void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.observer_ptr_lite", "0.2.0");
    t += Git("https://github.com/martinmoene/observer-ptr-lite", "v{v}");
}
