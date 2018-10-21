void build(Solution &s)
{
    auto &getopt_port = s.addTarget<StaticLibraryTarget>("kimgr.getopt_port", "master");
    getopt_port += Git("https://github.com/kimgr/getopt_port", "", "{v}");

    getopt_port +=
        "getopt.c",
        "getopt.h";
}
