void build(Solution &s)
{
    auto &glm = s.addTarget<LibraryTarget>("g_truc.glm", "0.9.9.3");
    glm += Git("https://github.com/g-truc/glm");
    glm += "glm/.*"_rr;
}
