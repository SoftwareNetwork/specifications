void build(Solution &s)
{
    auto &wr = s.addLibrary("giovannidicanio.winreg", "14.1.1");
    wr += Git("https://github.com/GiovanniDicanio/WinReg", "v{v}");
    wr.setRootDirectory("Include/WinReg");
    //wr.ApiName = "WINREG_API";
    wr += ".*"_rr;
    wr += "advapi32.lib"_slib;
}
