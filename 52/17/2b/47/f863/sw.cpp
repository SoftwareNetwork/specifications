void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &wr = s.addTarget<StaticLibraryTarget>("giovannidicanio.winreg", "master");
    wr.Source = Git("https://github.com/GiovanniDicanio/WinReg", "", "{b}");
    wr.setRootDirectory("WinReg/WinReg");
    wr += "WinReg.hpp";
}