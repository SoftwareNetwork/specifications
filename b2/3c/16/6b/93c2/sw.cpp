void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &webp = s.addTarget<LibraryTarget>("webp", "0.6.0");
    webp.Source = Git("https://github.com/webmproject/libwebp", "v{v}");
    webp.ApiName = "WEBP_API";

    webp +=
        "src/.*\\.c"_rr,
        "src/.*\\.h"_rr;

    webp.Public +=
        "src"_id;

    webp.replaceInFileOnce("src/webp/types.h",
        "#ifndef WEBP_EXTERN",
        "#define WEBP_EXTERN(type) extern WEBP_API type\n#ifndef WEBP_EXTERN");
}