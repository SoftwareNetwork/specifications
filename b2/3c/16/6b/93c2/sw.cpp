void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &webp = s.addTarget<LibraryTarget>("webp", "0.6.0");
    webp.Source = Git("https://github.com/webmproject/libwebp", "v{v}");

    webp +=
        "src/.*\\.c"_rr,
        "src/.*\\.h"_rr;

    webp.Public +=
        "src"_id;

}
