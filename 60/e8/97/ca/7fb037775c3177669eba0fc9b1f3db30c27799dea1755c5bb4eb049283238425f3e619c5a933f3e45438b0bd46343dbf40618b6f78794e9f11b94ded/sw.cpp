void build(Solution &s)
{
    auto &opengl = s.addTarget<LibraryTarget>("opengl", "master");
    opengl.AutoDetectOptions = false;
    opengl.Empty = true;
    if (s.Settings.TargetOS.Type == OSType::Windows)
        opengl.Public += "opengl32.lib"_lib;
}
