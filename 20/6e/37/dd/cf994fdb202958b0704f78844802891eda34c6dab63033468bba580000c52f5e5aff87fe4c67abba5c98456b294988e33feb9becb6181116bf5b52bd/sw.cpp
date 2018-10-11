void build(Solution &s)
{
    auto &ns = s.addStaticLibrary("google.nsync", "1.20.0");
    ns += Git("https://github.com/google/nsync", "{v}");

    ns += "internal/.*"_rr;
    ns += "public/.*"_rr;

    ns += "internal"_idir;
    ns.Public += "public"_idir;

    ns -= "platform/.*"_rr;
    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        ns += "platform/win32/.*"_rr;
        ns += "platform/win32"_idir;
    }
    if (s.Settings.Native.CompilerType == CompilerType::MSVC)
    {
        ns += "platform/msvc/.*"_rr;
        ns += "platform/msvc"_idir;
    }
    if (s.Settings.TargetOS.Arch == ArchType::x86)
    {
        ns += "platform/x86_32/.*"_rr;
        ns += "platform/x86_32"_idir;
    }
    if (s.Settings.TargetOS.Arch == ArchType::x86_64)
    {
        ns += "platform/x86_64/.*"_rr;
        ns += "platform/x86_64"_idir;
    }
}
