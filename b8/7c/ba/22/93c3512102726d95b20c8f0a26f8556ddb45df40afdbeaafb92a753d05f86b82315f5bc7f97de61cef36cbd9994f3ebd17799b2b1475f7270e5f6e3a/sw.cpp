void build(Solution &s)
{
    auto &libyuv = s.addTarget<LibraryTarget>("chromium.libyuv", "master");
    libyuv += Git("https://chromium.googlesource.com/libyuv/libyuv", "", "{v}");

    libyuv.ApiName = "LIBYUV_API";

    //if (base.getBuildSettings().TargetOS.Arch == Arch::x86_64)
    //libyuv += "LIBYUV_DISABLE_X86"_def;
    //libyuv += "VISUALC_HAS_SSSE3"_def;
    //libyuv += "VISUALC_HAS_AVX2"_def;
}
