void build(Solution &s)
{
    auto &wr = s.addLibrary("giovannidicanio.winreg", "4.1.2");
    wr += Git("https://github.com/GiovanniDicanio/WinReg", "v{v}");
    wr.setRootDirectory("WinReg");
    //wr.ApiName = "WINREG_API";
    wr += "WinReg.hpp";
    wr += "advapi32.lib"_slib;
}