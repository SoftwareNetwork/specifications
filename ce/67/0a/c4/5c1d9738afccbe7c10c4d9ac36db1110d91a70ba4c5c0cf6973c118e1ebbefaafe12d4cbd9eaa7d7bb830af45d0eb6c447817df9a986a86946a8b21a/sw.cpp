void build(Solution &s)
{
    auto &webp = s.addTarget<LibraryTarget>("webmproject.webp", "1.6.0");
    webp += Git("https://github.com/webmproject/libwebp", "v{v}");

    webp.ApiName = "WEBP_API";
    webp +=
        "sharpyuv/.*\\.[hc]"_rr,
        "src/.*\\.[hc]"_rr
        ;

    webp += "."_id;
    webp.Public += "src"_id;

    if (webp.getCompilerType() == CompilerType::Clang || webp.getCompilerType() == CompilerType::ClangCl) {
        if (webp.getBuildSettings().TargetOS.Arch != ArchType::aarch64) {
            webp.CompileOptions.push_back("-mavx2");
        }
    }

    webp.replaceInFileOnce("src/webp/types.h", "#ifndef WEBP_EXTERN", "#define WEBP_EXTERN extern " + webp.ApiName + "\n#ifndef WEBP_EXTERN");
}
