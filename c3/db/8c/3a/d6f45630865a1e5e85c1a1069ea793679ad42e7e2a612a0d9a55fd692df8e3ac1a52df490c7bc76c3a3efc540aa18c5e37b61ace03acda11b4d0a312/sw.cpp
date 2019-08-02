void build(Solution &s)
{
    auto &opengl = s.addTarget<LibraryTarget>("opengl", "master");
    opengl += EmptySource();
    opengl.AutoDetectOptions = false;
    opengl.Empty = true;
    if (opengl.getSettings().TargetOS.Type == OSType::Windows)
        opengl.Public += "opengl32.lib"_slib;
}
