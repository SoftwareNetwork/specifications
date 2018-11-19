void build(Solution &s)
{
    auto &gif = s.addTarget<StaticLibraryTarget>("gif", "5.1.4");
    gif += RemoteFile("https://downloads.sourceforge.net/project/giflib/giflib-{v}.tar.bz2");

    gif +=
        "lib/.*\\.c"_rr,
        "lib/.*\\.h"_rr;

    gif.Public +=
        "lib"_id;

    if (s.Settings.Native.CompilerType == CompilerType::MSVC)
        gif.writeFileOnce("unistd.h");
}