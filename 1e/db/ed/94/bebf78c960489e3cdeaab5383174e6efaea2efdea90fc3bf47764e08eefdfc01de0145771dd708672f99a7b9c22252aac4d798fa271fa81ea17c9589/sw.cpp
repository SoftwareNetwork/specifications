void build(Solution &s)
{
    auto &glm = s.addTarget<StaticLibraryTarget>("g_truc.glm", "0.9.9.6");
    glm += Git("https://github.com/g-truc/glm");
    glm += "glm/.*"_rr;
}
