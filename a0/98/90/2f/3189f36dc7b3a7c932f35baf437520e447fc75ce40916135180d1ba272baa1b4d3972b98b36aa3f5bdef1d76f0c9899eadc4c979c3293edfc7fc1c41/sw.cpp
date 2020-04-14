void build(Solution &s)
{
    auto &wr = s.addTarget<StaticLibraryTarget>("giovannidicanio.winreg", "2.0.0");
    wr += Git("https://github.com/GiovanniDicanio/WinReg", "v{M}.{m}{po}");
    wr.setRootDirectory("WinReg/WinReg");
    wr += "WinReg.[ch]pp"_r;
}
