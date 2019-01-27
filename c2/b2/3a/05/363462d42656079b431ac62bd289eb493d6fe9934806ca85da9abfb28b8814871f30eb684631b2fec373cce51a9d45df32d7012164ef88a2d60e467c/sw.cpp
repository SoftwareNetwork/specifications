void build(Solution &s)
{
    auto &f = s.addTarget<FortranExecutable>("main.fortran");
    f += Git("https://github.com/SoftwareNetwork/sw", "", "master");
    f += "test/build/fortran/main.f";
}
