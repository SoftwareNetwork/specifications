void build(Solution &s)
{
    auto &wr = s.addLibrary("giovannidicanio.winreg", "14.0.0");
    wr += Git("https://github.com/GiovanniDicanio/WinReg", "v{v}");
    wr.setRootDirectory("Include/WinReg");
    //wr.ApiName = "WINREG_API";
    wr += ".*"_rr;
    wr += "advapi32.lib"_slib;
}
