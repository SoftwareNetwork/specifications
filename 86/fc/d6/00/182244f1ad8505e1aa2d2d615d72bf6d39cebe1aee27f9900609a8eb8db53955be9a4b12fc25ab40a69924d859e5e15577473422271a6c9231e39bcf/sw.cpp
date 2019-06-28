void build(Solution &s)
{
    auto &p = s.addTarget<LibraryTarget>("stdcpp_audio.libstdaudio", "master");
    p += Git("https://github.com/stdcpp-audio/libstdaudio");
}
