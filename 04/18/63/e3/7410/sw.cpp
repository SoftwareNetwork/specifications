void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &gif = s.addTarget<StaticLibraryTarget>("gif", "5.1.4");
    gif.Source = RemoteFile("https://downloads.sourceforge.net/project/giflib/giflib-{v}.tar.bz2");

    gif +=
        "lib/.*\\.c"_rr,
        "lib/.*\\.h"_rr;

    gif.Public +=
        "lib"_id;
}