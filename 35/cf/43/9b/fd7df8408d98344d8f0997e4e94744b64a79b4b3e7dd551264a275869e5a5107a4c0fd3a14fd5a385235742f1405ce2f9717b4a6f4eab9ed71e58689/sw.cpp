void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("gsoap", "2.8.138");
    t += RemoteFile("https://sourceforge.net/projects/gsoap2/files/gsoap_{v}.zip/download");
    t.setRootDirectory("gsoap");
    t += "stdsoap2\\.[hc]"_r;
    //t.Public += "SOAP_STD_EXPORTS"_def;
    //t += "SOAP_STD_API"_api;
    t += "ws2_32.lib"_slib;
}
