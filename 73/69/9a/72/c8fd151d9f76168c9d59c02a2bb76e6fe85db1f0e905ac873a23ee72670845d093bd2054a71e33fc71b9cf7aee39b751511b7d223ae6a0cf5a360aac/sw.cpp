void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("Philip_Trettner.cpp_std_fwd", "master");
    t += Git("https://github.com/Philip-Trettner/cpp-std-fwd");
}
