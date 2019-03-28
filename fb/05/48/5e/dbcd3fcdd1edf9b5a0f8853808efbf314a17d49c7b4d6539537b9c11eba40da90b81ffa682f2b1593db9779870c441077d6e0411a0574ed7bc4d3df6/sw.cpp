void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("octobanana.belle", "0.5.1");
    t += Git("https://github.com/octobanana/belle");
}
