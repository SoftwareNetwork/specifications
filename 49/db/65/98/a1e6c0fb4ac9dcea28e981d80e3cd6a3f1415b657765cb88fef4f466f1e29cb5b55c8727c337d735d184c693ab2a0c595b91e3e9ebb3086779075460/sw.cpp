#pragma sw require pub.egorpugin.primitives.filesystem-master

void build(Solution &s)
{
    auto &p = s.addProject("cyrus", "2.1.27");
    p += Git("https://github.com/cyrusimap/cyrus-sasl", "cyrus-sasl-{v}");

    auto &t = p.addTarget<Library>("sasl");

    t += "include/.*"_rr;
    t += "lib/.*"_rr;
    t += "common/.*"_rr;

    t -= "lib/getsubopt.c";
    t -= "lib/getaddrinfo.c";
    t -= "lib/getnameinfo.c";
    t -= "lib/snprintf.c";

    t += "common"_idir;
    t.Public += "include"_idir;

    t += "plugins/anonymous.c";
    t += "plugins/digestmd5.c";
    t += "plugins/scram.c";
    t += "plugins/ntlm.c";
    t += "plugins/plain.c";
    t -= "win32/include/.*"_rr;

    t += "GCC_FALLTHROUGH="_def;

    if (t.getSettings().TargetOS.Type == OSType::Windows)
    {
        t -= "lib/dlopen.c";
        t += "win32/include"_idir;
        t += sw::Shared, "WIN32"_def;
        t += sw::Shared, "LIBSASL_EXPORTS"_def;
        t += "ws2_32.lib"_slib, "advapi32.lib"_slib;
    }
    else
    {
        t -= "lib/windlopen.c";
    }

    t.Public += "org.sw.demo.openssl.crypto-*.*.*.*"_dep;

    if (t.DryRun)
        return;

    auto dummy = t.BinaryDir / "private/sw_copy_headers.txt";
    if (!fs::exists(dummy))
    {
        auto dir1 = t.BinaryDir / "sasl";
        fs::create_directories(dir1);
        for (auto &f : fs::directory_iterator(t.SourceDir / "include"))
            fs::copy_file(f, dir1 / f.path().filename(), fs::copy_options::overwrite_existing);
        write_file(dummy, "");
    }
}
