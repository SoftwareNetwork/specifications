void build(Solution &s)
{
    auto &t = s.addStaticLibrary("libmad", "0.15.1.1");
    char letter = 'a' + t.getPackage().getVersion().getTweak();
    t += RemoteFile("https://sourceforge.net/projects/mad/files/libmad/{M}.{m}.{p}"s + letter + "/libmad-{M}.{m}.{p}"s + letter + ".tar.gz");

    t += ".*\\.[hc]"_r;
    t += ".*\\.in"_r;
    t += ".*\\.dat"_r;

    t -= "minimad.c";

    if (t.getBuildSettings().TargetOS.Arch == ArchType::x86_64)
        t.Public += "FPM_64BIT"_def;
    else
        t.Public += "FPM_INTEL"_def;
}
