void build(Solution &s)
{
    auto &gif = s.addTarget<LibraryTarget>("gif", "5.2.2");
    gif += RemoteFile("https://sourceforge.net/projects/giflib/files/giflib-{v}.tar.gz");

    gif.ExportAllSymbols = true;

    gif += "dgif_lib.c";
    gif += "egif_lib.c";
    gif += "getarg.c";
    gif += "gifalloc.c";
    gif += "gif_err.c";
    gif += "gif_font.c";
    gif += "gif_hash.c";
    gif += "openbsd-reallocarray.c";
    gif += "qprintf.c";
    gif += "quantize.c";
    gif += ".*\\.h"_rr;

    if (gif.getBuildSettings().TargetOS.is(OSType::Windows))
        gif += "strtok_r=strtok_s"_def;
    if (gif.getCompilerType() == CompilerType::MSVC)
        gif.writeFileOnce(gif.BinaryPrivateDir / "unistd.h");
}
