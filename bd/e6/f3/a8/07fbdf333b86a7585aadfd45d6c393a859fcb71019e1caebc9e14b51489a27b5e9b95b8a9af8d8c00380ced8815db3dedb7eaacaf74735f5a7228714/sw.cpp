void build(Solution &s)
{
    auto &t = s.addStaticLibrary("BenjaminDobell.s3tc_dxt_decompression", "master");
    t += Git("https://github.com/Benjamin-Dobell/s3tc-dxt-decompression");
    t += "s3tc.h";
    t += "s3tc.cpp";
}
