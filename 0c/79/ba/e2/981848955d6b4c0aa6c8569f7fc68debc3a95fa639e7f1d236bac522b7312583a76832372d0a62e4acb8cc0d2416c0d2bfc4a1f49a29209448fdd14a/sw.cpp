void build(Solution &s)
{
    auto &t = s.addLibrary("OSGeo.libgeotiff", "1.7.4");
    t += Git("https://github.com/OSGeo/libgeotiff");

    {
        t.setRootDirectory("libgeotiff");
        t.setChecks("libgeotiff");

        t.ApiName = "GTIF_DLL";
        t += "libxtiff/.*\\.[hc]"_r;
        t += ".*\\.[hc]"_r;
        t += ".*\\.inc"_r;

        t += "GTIFAtof=GTIFAtof"_def;

        t.Public += "libxtiff"_idir;
        t.Public += "."_idir;

        t.Public += "org.sw.demo.tiff"_dep;
        t.Public += "org.sw.demo.OSGeo.PROJ"_dep;

        t.configureFile("geo_config.h.in", "geo_config.h");
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("libgeotiff");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("strings.h");
    s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <float.h>
    int main() {return 0;}
    )sw_xxx");

}
