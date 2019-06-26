void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("TheLandfill.cpp_cli", "master");
    t += Git("https://github.com/TheLandfill/cpp_cli");
}
