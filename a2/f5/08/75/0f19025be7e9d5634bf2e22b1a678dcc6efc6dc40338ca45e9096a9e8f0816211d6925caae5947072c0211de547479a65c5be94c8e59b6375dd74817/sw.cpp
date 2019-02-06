void build(Solution &s)
{
    auto &vorbis = s.addProject("xiph.vorbis", "1.3.6");
    vorbis += RemoteFile("https://ftp.osuosl.org/pub/xiph/releases/vorbis/libvorbis-{v}.tar.xz");

    auto &libvorbis = vorbis.addTarget<LibraryTarget>("libvorbis");
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
    libvorbis.Public += "org.sw.demo.xiph.libogg-1"_dep;

    auto &vorbisfile = vorbis.addTarget<LibraryTarget>("vorbisfile");
    vorbisfile +=
        "include/.*\\.h"_rr,
        "lib/misc.h",
        "lib/os.h",
        "lib/vorbisfile.c",
        "win32/vorbisfile.def";
    vorbisfile.Public += libvorbis;
}
