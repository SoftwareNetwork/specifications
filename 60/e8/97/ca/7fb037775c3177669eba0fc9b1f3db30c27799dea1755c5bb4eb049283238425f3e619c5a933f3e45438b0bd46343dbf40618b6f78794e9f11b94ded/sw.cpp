void build(Solution &s)
{
    auto &opengl = s.addTarget<LibraryTarget>("find.opengl", "master");
    opengl += EmptySource();
    opengl.AutoDetectOptions = false;
    opengl.Empty = true;
    if (opengl.getBuildSettings().TargetOS.Type == OSType::Windows)
        opengl.Public += "opengl32.lib"_slib;
    if (opengl.getBuildSettings().TargetOS.isApple())
        opengl.Public += "OpenGL"_framework;
}
