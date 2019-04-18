void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("rainers.cv2pdb", "master");
    t += Git("https://github.com/rainers/cv2pdb");
}
