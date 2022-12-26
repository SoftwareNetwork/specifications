void build(Solution &s)
{
    auto &t = s.addLibrary("xorg.proto", "2022.2.0");
    t += Git("https://gitlab.freedesktop.org/xorg/proto/xorgproto", "xorgproto-{M}.{m}");

    t.setChecks("x");

    t += "include/.*\\.h"_rr;

    if (t.Variables["HAVE_FD_SET_FDS_BITS"] == 1)
        t.Variables["USE_FDS_BITS"] = "fds_bits";
    else if (t.Variables["HAVE_FD_SET___FDS_BITS"] == 1)
        t.Variables["USE_FDS_BITS"] = "__fds_bits";
    else if (!t.DryRun && t.getBuildSettings().TargetOS.is(OSType::Linux))
        throw std::runtime_error("Your fd_set is too weird.");
    t.configureFile("include/X11/Xpoll.h.in", "X11/Xpoll.h");
}

void check(Checker &c)
{
    auto &s = c.addSet("x");

    auto add_headers = [](auto &c)
    {
        for (auto &h : {
            "sys/param.h",
            "sys/select.h",
            "sys/types.h",
            "sys/time.h",
            })
            c.Parameters.Includes.push_back(h);
    };

    add_headers(s.checkStructMemberExists("fd_set", "fds_bits"));
    add_headers(s.checkStructMemberExists("fd_set", "__fds_bits"));
}
