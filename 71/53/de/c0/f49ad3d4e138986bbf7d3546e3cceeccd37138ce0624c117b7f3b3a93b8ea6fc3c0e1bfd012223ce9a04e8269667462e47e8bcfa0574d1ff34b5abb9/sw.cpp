void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("j_jorge.weak_function", "master");
    t += Git("https://github.com/j-jorge/weak-function");
}
