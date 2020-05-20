void build(Build &s)
{
    auto &t = s.addLibrary("baical.P7.client", "5.5.0");
    t += RemoteFile("http://baical.net/files/libP7Client_v{M}.{m}.zip");

    t += "Headers/.*"_rr;
    t += "Shared/.*"_rr;
    t += "Sources/.*"_rr;

    t.Public += "Headers"_idir;
    t.Public += "Shared"_idir;
    if (t.getBuildSettings().TargetOS.is(OSType::Windows))
        t.Public += "Shared/Platforms/Windows_x86"_idir;
    else
        t.Public += "Shared/Platforms/Linux_x86"_idir;

    if (t.getBuildSettings().TargetOS.is(OSType::Windows))
    {
        t += "ws2_32.lib"_slib;
    }
}
