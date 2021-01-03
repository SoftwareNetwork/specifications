void build(Solution &s)
{
    auto &webp = s.addTarget<LibraryTarget>("webmproject.webp", "1.1.0");
    webp += Git("https://github.com/webmproject/libwebp", "v{v}");

    webp.ApiName = "WEBP_API";
    webp +=
        "src/.*\\.c"_rr,
        "src/.*\\.h"_rr;

    webp += "."_id;
    webp.Public += "src"_id;

    webp.replaceInFileOnce("src/webp/types.h", "#ifndef WEBP_EXTERN", "#define WEBP_EXTERN extern " + webp.ApiName + "\n#ifndef WEBP_EXTERN");
}
