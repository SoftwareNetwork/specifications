void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("fukuchi.libqrencode", "4.0.2");
    t += Git("https://github.com/fukuchi/libqrencode", "v{v}");

    t += ".*\\.[hc]"_r;
    t -= "qrenc.c";

    t += "STATIC_IN_RELEASE=static"_def;

    t += Definition("VERSION=\""s + t.getPackage().toString() + "\"");
    t += Definition("MAJOR_VERSION="s + std::to_string(t.getPackage().getVersion().getMajor()));
    t += Definition("MINOR_VERSION="s + std::to_string(t.getPackage().getVersion().getMinor()));
    t += Definition("MICRO_VERSION="s + std::to_string(t.getPackage().getVersion().getPatch()));
}
