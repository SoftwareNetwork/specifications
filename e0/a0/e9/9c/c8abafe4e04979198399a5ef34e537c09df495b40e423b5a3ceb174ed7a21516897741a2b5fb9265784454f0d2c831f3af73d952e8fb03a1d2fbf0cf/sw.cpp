void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.observer_ptr_lite", "master");
    t += Git("https://github.com/martinmoene/observer-ptr-lite");
}
