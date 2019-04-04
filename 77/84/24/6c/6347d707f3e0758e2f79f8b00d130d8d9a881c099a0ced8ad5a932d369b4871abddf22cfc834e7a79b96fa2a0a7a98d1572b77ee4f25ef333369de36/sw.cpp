void build(Solution &s)
{
    auto &p = s.addTarget<StaticLibraryTarget>("spotify.json", "1.0.1");
    p += Git("https://github.com/spotify/spotify-json", "v{v}");
}
