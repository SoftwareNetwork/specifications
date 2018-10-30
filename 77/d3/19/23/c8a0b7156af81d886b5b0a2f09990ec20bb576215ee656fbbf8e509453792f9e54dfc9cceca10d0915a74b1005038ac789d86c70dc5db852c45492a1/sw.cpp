void build(Solution &s)
{
    auto &libvorbis = s.addTarget<LibraryTarget>("xiph.vorbis.libvorbis", "1.3.6");
    libvorbis += RemoteFile("https://ftp.osuosl.org/pub/xiph/releases/vorbis/libvorbis-{v}.tar.xz");

    libvorbis +=
        "include/.*\\.h"_rr,
        "lib/.*\\.c"_rr,
        "lib/.*\\.h"_rr,
        "win32/.*\\.def"_rr;
    libvorbis -= "win32/vorbisenc.def";
    libvorbis -= "win32/vorbisfile.def";

    libvorbis -=
        "lib/barkmel.c",
        "lib/psytune.c",
        "lib/tone.c",
        "lib/vorbisfile.c";

    libvorbis.Private +=
        "lib"_id;

    libvorbis.Public +=
        "include"_id;

    libvorbis.Public += "org.sw.demo.xiph.libogg-1"_dep;
}
