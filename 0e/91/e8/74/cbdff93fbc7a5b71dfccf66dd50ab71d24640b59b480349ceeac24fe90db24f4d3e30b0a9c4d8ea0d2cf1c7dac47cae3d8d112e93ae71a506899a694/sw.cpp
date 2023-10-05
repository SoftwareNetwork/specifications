void build(Solution &s)
{
    auto &webp = s.addTarget<LibraryTarget>("webmproject.webp", "1.3.2");
    webp += Git("https://github.com/webmproject/libwebp", "v{v}");

    webp.ApiName = "WEBP_API";
    webp +=
        "sharpyuv/.*\\.[hc]"_rr,
        "src/.*\\.[hc]"_rr
        ;

    webp += "."_id;
    webp.Public += "src"_id;

    webp.replaceInFileOnce("src/webp/types.h", "#ifndef WEBP_EXTERN", "#define WEBP_EXTERN extern " + webp.ApiName + "\n#ifndef WEBP_EXTERN");
}
