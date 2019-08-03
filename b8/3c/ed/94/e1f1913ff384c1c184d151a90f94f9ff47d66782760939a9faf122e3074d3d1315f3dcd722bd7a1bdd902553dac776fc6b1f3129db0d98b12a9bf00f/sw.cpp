void build(Solution &s)
{
    auto &glm = s.addTarget<StaticLibraryTarget>("g_truc.glm", "0.9.9.5");
    glm += Git("https://github.com/g-truc/glm");
    glm += "glm/.*"_rr;
}
