void build(Solution &s)
{
    auto &rapidxml = s.addTarget<LibraryTarget>("rapidxml", "1.13.0");
    rapidxml += RemoteFile("http://freefr.dl.sourceforge.net/project/rapidxml/rapidxml/rapidxml%20{M}.{m}/rapidxml-{M}.{m}.zip");
    rapidxml += ".*\\.hpp"_rr;
}
