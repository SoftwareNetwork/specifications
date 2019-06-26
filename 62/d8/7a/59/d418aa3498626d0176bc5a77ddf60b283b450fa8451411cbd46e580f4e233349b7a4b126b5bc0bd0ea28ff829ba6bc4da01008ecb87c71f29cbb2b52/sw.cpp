void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("pdziepak.codegen", "master");
    t += Git("https://github.com/pdziepak/codegen");
}
