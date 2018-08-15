void build(Solution &s)
{
    auto &glew = s.addTarget<LibraryTarget>("glew", "2.1.0");
    glew += RemoteFile("https://freefr.dl.sourceforge.net/project/glew/glew/{v}/glew-{v}.tgz");

    glew +=
        "include/.*"_rr,
        "src/glew.c";

    glew.Private += sw::Shared, "GLEW_BUILD"_d;
    glew.Public += sw::Static, "GLEW_STATIC"_d;
    glew.Public += "org.sw.demo.find.opengl-master"_dep;
}
