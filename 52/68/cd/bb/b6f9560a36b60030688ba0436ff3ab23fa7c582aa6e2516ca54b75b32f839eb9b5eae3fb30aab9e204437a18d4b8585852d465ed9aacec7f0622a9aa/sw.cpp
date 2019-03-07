void build(Solution &s)
{
    auto &t = s.addTarget<Library>("ctabin.libzippp", "2.1.0");
    t += Git("https://github.com/ctabin/libzippp", "", "libzippp-v{M}.{m}-1.5.1");

    if (s.Settings.TargetOS.Type == OSType::Windows)
        t.Public += "WIN32"_def;

    t.ApiName = "LIBZIPPP_API";
    t.patch("src/libzippp.h", "#define LIBZIPPP_API", "//#define  LIBZIPPP_API");

    t += "org.sw.demo.nih_at.libzip"_dep;
}
