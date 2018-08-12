void build(Solution &s)
{
    auto &wr = s.addTarget<StaticLibraryTarget>("giovannidicanio.winreg3", "master");
    wr += Git("https://github.com/GiovanniDicanio/WinReg", "", "{b}");
    wr.setRootDirectory("WinReg/WinReg");
    wr += "WinReg.hpp";
}
