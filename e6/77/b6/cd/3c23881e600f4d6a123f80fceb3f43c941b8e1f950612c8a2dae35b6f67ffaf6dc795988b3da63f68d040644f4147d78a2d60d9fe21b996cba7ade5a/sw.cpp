void build(Solution &s)
{
    auto &apr = s.addTarget<LibraryTarget>("apache.apr", "1.6.5");
    apr += Git("https://github.com/apache/apr", "{v}");

    apr +=
        "include/.*"_rr,

        "atomic/.*"_rr,
        "dso/.*"_rr,
        "encoding/.*"_rr,
        "file_io/.*"_rr,
        "locks/.*"_rr,
        "memory/.*"_rr,
        "misc/.*"_rr,
        "mmap/.*"_rr,
        "network_io/.*"_rr,
        "passwd/.*"_rr,
        "poll/.*"_rr,
        "random/.*"_rr,
        "shmem/.*"_rr,
        "strings/.*"_rr,
        "tables/.*"_rr,
        "threadproc/.*"_rr,
        "time/.*"_rr,
        "user/.*"_rr
        ;

    apr.Public += "include"_id;
    apr += "include/private"_id;

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        apr -= ".*aix.*"_rr;
        apr -= ".*beos.*"_rr;
        apr -= ".*netware.*"_rr;
        apr -= ".*os2.*"_rr;
        apr -= ".*os390.*"_rr;
        apr -= ".*unix.*"_rr;
        apr -= "misc/win32/apr_app.c";

        apr += "include/arch/win32"_id;
        apr += "include/arch/unix"_id;

        apr.Public += "mswsock.lib"_slib;
        apr.Public += "rpcrt4.lib"_slib;
        apr.Public += "ws2_32.lib"_slib;
        apr.Public += "advapi32.lib"_slib;
        apr.Public += "shell32.lib"_slib;
    }
    else
    {
        apr -= ".*aix.*"_rr;
        apr -= ".*beos.*"_rr;
        apr -= ".*netware.*"_rr;
        apr -= ".*os2.*"_rr;
        apr -= ".*os390.*"_rr;
        apr -= ".*win32.*"_rr;

        apr += "include/arch/unix"_id;
    }
    apr += "memory/.*"_rr;
    apr += "file_io/unix/fullrw.*"_rr;
    apr += "file_io/unix/filepath_util.*"_rr;
    apr += "file_io/unix/mktemp.*"_rr;
    apr += "file_io/unix/tempdir.*"_rr;
    apr += "network_io/unix/sockaddr.*"_rr;
    apr += "network_io/unix/inet_.*"_rr;
    apr += "poll/unix/.*"_rr;
    apr += "misc/unix/errorcodes.c";

    apr += sw::Shared, "APR_DECLARE_EXPORT"_d;
    apr.Public += sw::Static, "APR_DECLARE_STATIC"_d;

    apr.Variables["apr_have_ipv6_10"] = 1;
    apr.Variables["win32_winnt_str"] = "0x0601";

    // .hw - w for windows
    apr.configureFile("include/apr.hwc", "apr.h");

    {
        auto &g = apr.addExecutable("tools.gen_test_char");
        g += "tools/gen_test_char.c";

        auto c = apr.addCommand();
        c << cmd::prog(g)
            << cmd::std_out("apr_escape_test_char.h");
    }
}
