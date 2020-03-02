void build(Solution &s)
{
    auto &t = s.addStaticLibrary("irrlicht.irrXML", "1.2.0");
    t += RemoteFile("https://netix.dl.sourceforge.net/project/irrlicht/irrXML%20SDK/{M}.{m}/irrxml-{M}.{m}.zip");
}
