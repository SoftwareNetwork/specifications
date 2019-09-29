void build(Solution &s)
{
    auto &wr = s.addTarget<StaticLibraryTarget>("giovannidicanio.winreg", "1.2.1");
    wr += Git("https://github.com/GiovanniDicanio/WinReg", "v{v}");
    wr.setRootDirectory("WinReg/WinReg");
    wr += "WinReg.hpp";
}
