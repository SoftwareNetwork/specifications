void build(Solution &s)
{
    auto &snappy = s.addTarget<LibraryTarget>("google.snappy", "1.2.0");
    snappy += Git("https://github.com/google/snappy", "{v}");

    snappy.setChecks("snappy");

    snappy.ExportAllSymbols = true;
    snappy +=
        "snappy-c.cc",
        "snappy-c.h",
        "snappy-internal.h",
        "snappy-sinksource.cc",
        "snappy-sinksource.h",
        "snappy-stubs-internal.cc",
        "snappy-stubs-internal.h",
        "snappy-stubs-public.h.in",
        "snappy.cc",
        "snappy.h";

    snappy.Variables["HAVE_LIBZ"] = 1;
    snappy.Variables["HAVE_LIBLZO2"] = 1;

    snappy.Variables["HAVE_STDINT_H_01"] = snappy.Variables["HAVE_STDINT_H"];
    snappy.Variables["HAVE_STDDEF_H_01"] = snappy.Variables["HAVE_STDDEF_H"];
    snappy.Variables["HAVE_SYS_UIO_H_01"] = snappy.Variables["HAVE_SYS_UIO_H"];

    snappy.configureFile(
        "snappy-stubs-public.h.in",
        "snappy-stubs-public.h");

    snappy.Public += "org.sw.demo.madler.zlib"_dep;
    snappy.Public += "org.sw.demo.oberhumer.lzo.lzo"_dep;
}

void check(Checker &c)
{
    auto &s = c.addSet("snappy");
    s.checkIncludeExists("byteswap.h");
    s.checkIncludeExists("stddef.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("sys/endian.h");
    s.checkIncludeExists("sys/mman.h");
    s.checkIncludeExists("sys/resource.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/uio.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("windows.h");
    s.checkTypeSize("size_t");
    s.checkTypeSize("void *");
}
