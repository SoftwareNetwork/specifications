void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &glew = s.addTarget<LibraryTarget>("glew", "2.1.0");
    glew.Source = RemoteFile("http://freefr.dl.sourceforge.net/project/glew/glew/{v}/glew-{v}.tgz");

    glew +=
        "include/.*"_rr,
        "src/glew.c";

    glew.Private += sw::Shared, "GLEW_BUILD"_d;
    glew.Public += sw::Static, "GLEW_STATIC"_d;

    if (s.Settings.TargetOS.Type == OSType::Windows)
        glew.Public += "opengl32.lib"_lib;
}
