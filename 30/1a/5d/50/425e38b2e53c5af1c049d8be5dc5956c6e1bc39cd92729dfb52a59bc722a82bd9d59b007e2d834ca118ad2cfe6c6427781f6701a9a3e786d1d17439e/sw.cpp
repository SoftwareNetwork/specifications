void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("google.cpu_features", "0.4.1");
    t += Git("https://github.com/google/cpu_features", "v{v}");

    t += "include/.*"_rr;
    t += "src/.*"_rr;

    if (t.getBuildSettings().TargetOS.Type == OSType::Windows ||
        t.getBuildSettings().TargetOS.Type == OSType::Mingw
        )
    {
        t -= "src/hwcaps.c";
        t -= "src/unix_features_aggregator.c";
    }

    t -= "src/.*aarch64.*"_rr;
    t -= "src/.*arm.*"_rr;
    t -= "src/.*mips.*"_rr;
    t -= "src/.*ppc.*"_rr;
    t -= "src/.*x86.*"_rr;

    if (0);
    else if (
        t.getBuildSettings().TargetOS.Arch == ArchType::aarch64 ||
        t.getBuildSettings().TargetOS.Arch == ArchType::aarch64_be
        )
        t += "src/.*aarch64.*"_rr;
    else if (
        t.getBuildSettings().TargetOS.Arch == ArchType::arm ||
        t.getBuildSettings().TargetOS.Arch == ArchType::armeb
        )
        t += "src/.*arm.*"_rr;
    else if (
        t.getBuildSettings().TargetOS.Arch == ArchType::mips ||
        t.getBuildSettings().TargetOS.Arch == ArchType::mips64 ||
        t.getBuildSettings().TargetOS.Arch == ArchType::mips64el ||
        t.getBuildSettings().TargetOS.Arch == ArchType::mipsel
        )
        t += "src/.*mips.*"_rr;
    else if (
        t.getBuildSettings().TargetOS.Arch == ArchType::ppc ||
        t.getBuildSettings().TargetOS.Arch == ArchType::ppc64 ||
        t.getBuildSettings().TargetOS.Arch == ArchType::ppc64le
        )
        t += "src/.*ppc.*"_rr;
    else if (
        t.getBuildSettings().TargetOS.Arch == ArchType::x86 ||
        t.getBuildSettings().TargetOS.Arch == ArchType::x86_64
        )
        t += "src/.*x86.*"_rr;

    t += "STACK_LINE_READER_BUFFER_SIZE=1024"_def;
}
