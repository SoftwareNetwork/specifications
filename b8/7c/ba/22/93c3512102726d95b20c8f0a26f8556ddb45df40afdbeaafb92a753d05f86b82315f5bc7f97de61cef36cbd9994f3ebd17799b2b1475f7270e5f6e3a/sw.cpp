void build(Solution &s)
{
    auto &libyuv = s.addTarget<LibraryTarget>("chromium.libyuv", "master");
    libyuv += Git("https://chromium.googlesource.com/libyuv/libyuv", "", "{v}");

    libyuv.Private += sw::Shared, "LIBYUV_BUILDING_SHARED_LIBRARY"_d;
    libyuv.Public += sw::Shared, "LIBYUV_USING_SHARED_LIBRARY"_d;

    libyuv.replaceInFileOnce("include/libyuv/convert_from.h", "#define H420ToH010 I420ToI010", "#define H420ToH010 I420ToI010\nLIBYUV_API");
}
