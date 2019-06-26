void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("xpdf", "4.1.1");
    t += RemoteFile("https://xpdfreader-dl.s3.amazonaws.com/xpdf-{M}.{m:02}.{p:02}.tar.gz");

    t += "aconf2.h";
    t += "fofi/.*"_rr;
    t += "goo/.*"_rr;
    t += "splash/.*"_rr;
    t += "xpdf/.*"_rr;

    t += "."_idir;
    t += "fofi"_idir;
    t += "goo"_idir;
    t += "splash"_idir;

    t += "org.sw.demo.freetype"_dep;

    t.Variables["HAVE_FREETYPE_H"] = 1;
    t.Variables["HAVE_LCMS"] = 1;
    t.Variables["HAVE_SPLASH"] = 1;

    /*
    #cmakedefine A4_PAPER
    #cmakedefine NO_TEXT_SELECT
    #cmakedefine01 OPI_SUPPORT
    #cmakedefine01 MULTITHREADED
    #cmakedefine01 USE_EXCEPTIONS
    #cmakedefine01 USE_FIXEDPOINT
    #cmakedefine01 SPLASH_CMYK
    #cmakedefine01 SPLASH_DEVICEN
    #cmakedefine HIGHLIGHTED_REGIONS
    @SYSTEM_XPDFRC_DEFINE@
    #cmakedefine01 HAVE_MKSTEMP
    #cmakedefine01 HAVE_MKSTEMPS
    #cmakedefine HAVE_POPEN
    #cmakedefine01 HAVE_STD_SORT
    #cmakedefine01 HAVE_FSEEKO
    #cmakedefine01 HAVE_FSEEK64
    #cmakedefine01 HAVE_FSEEKI64
    #cmakedefine01 HAVE_DTYPE4_H
    #cmakedefine01 HAVE_PAPER_H
    */

    t.configureFile("aconf.h.in", "aconf.h");
}
