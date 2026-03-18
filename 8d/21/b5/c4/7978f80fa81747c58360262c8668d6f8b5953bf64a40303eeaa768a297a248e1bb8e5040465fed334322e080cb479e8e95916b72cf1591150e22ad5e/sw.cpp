void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("_7zip._7z", "26.0.4");
    t += RemoteFile("https://www.7-zip.org/a/7z{M}0{m}-src.7z");
    t.setRootDirectory("C");
    t += ".*\\.[hc]"_r;
}
