void build(Solution &s)
{
    auto &wr = s.addLibrary("giovannidicanio.winreg", "2.1.0");
    wr += Git("https://github.com/GiovanniDicanio/WinReg", "v{v}");
    wr.setRootDirectory("WinReg");
    wr.ApiName = "WINREG_API";
    wr += "WinReg.[ch]pp"_r;
    wr += "advapi32.lib"_slib;
}
