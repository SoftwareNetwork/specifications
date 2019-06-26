void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("ADVRHumanoids.MatLogger2", "master");
    t += Git("https://github.com/ADVRHumanoids/MatLogger2");
}
