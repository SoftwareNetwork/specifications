void build(Solution &s)
{
    auto &glm = s.addTarget<StaticLibraryTarget>("g_truc.glm", "1.0.2");
    glm += Git("https://github.com/g-truc/glm");
    glm += "glm/.*"_rr;
}
