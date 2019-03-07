void build(Solution &s)
{
    auto &t = s.addTarget<Library>("nih_at.libzip", "1.5.1");
    t += Git("https://github.com/nih-at/libzip", "", "rel-{M}-{m}-{p}");

    t.setChecks("libzip", true);

    t.ApiName = "ZIP_EXTERN";

    t += "cmake-zipconf.h.in";
    t += "lib/.*"_rr;

    t -= "lib/.*uwp.*"_rr;
    t -= "lib/zip_crypto.*"_rr;
    t -= "lib/zip_random_.*"_rr;

    t += "HAVE_LIBBZ2"_def;

    t.Public += "lib"_id;

    t += "HAVE_OPENSSL"_def;
    t += "lib/zip_crypto_op.*"_rr;
    t += "org.sw.demo.openssl.crypto-*.*.*.*"_dep;

    t += "org.sw.demo.bzip2"_dep;
    t += "org.sw.demo.madler.zlib"_dep;

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        //t += "HAVE_WINDOWS_CRYPTO"_def;
        t -= "lib/.*unix.*"_rr;
        t -= "lib/zip_source_file.c";
        //t += "lib/zip_crypto_wi.*"_rr;
        //t += "advapi32.lib"_slib;
    }
    else
    {
        t -= "lib/.*win.*"_rr;
    }

    t.Variables["LIBZIP_VERSION"] = "PACKAGE_VERSION";
    t.Variables["LIBZIP_VERSION_MAJOR"] = "PACKAGE_VERSION_MAJOR";
    t.Variables["LIBZIP_VERSION_MINOR"] = "PACKAGE_VERSION_MINOR";
    t.Variables["LIBZIP_VERSION_MICRO"] = "PACKAGE_VERSION_PATCH";

    t.Variables["LIBZIP_TYPES_INCLUDE"] = R"(
#include <stdint.h>
)";

    auto add_type = [&t](String s)
    {
        t.Variables["ZIP_" + boost::to_upper_copy(s)] = s;
        t.Variables["ZIP_" + boost::to_upper_copy("u" + s)] = "u" + s;
    };

    for (auto s : {8,16,32,64})
        add_type("int" + std::to_string(s) + "_t");

    t.configureFile("cmake-zipconf.h.in", "zipconf.h");
}

void check(Checker &c)
{
    {
        auto &s = c.addSet("libzip");
        s.checkFunctionExists("_chmod");
        s.checkFunctionExists("_close");
        s.checkFunctionExists("_dup");
        s.checkFunctionExists("_fdopen");
        s.checkFunctionExists("_fileno");
        s.checkFunctionExists("_open");
        s.checkFunctionExists("_setmode");
        s.checkFunctionExists("_snprintf");
        s.checkFunctionExists("_strdup");
        s.checkFunctionExists("_stricmp");
        s.checkFunctionExists("_strtoi64");
        s.checkFunctionExists("_strtoui64");
        s.checkFunctionExists("_umask");
        s.checkFunctionExists("_unlink");
        s.checkFunctionExists("clonefile");
        s.checkFunctionExists("explicit_bzero");
        s.checkFunctionExists("explicit_memset");
        s.checkFunctionExists("fileno");
        s.checkFunctionExists("fseeko");
        s.checkFunctionExists("ftello");
        s.checkFunctionExists("getprogname");
        s.checkFunctionExists("open");
        s.checkFunctionExists("mkstemp");
        s.checkFunctionExists("setmode");
        s.checkFunctionExists("snprintf");
        s.checkFunctionExists("strcasecmp");
        s.checkFunctionExists("strdup");
        s.checkFunctionExists("stricmp");
        s.checkFunctionExists("strtoll");
        s.checkFunctionExists("strtoull");

        s.checkIncludeExists("stdbool.h");
        s.checkIncludeExists("strings.h");
        s.checkIncludeExists("unistd.h");

        s.checkIncludeExists("inttypes.h").Definitions.insert("HAVE_INTTYPES_H_LIBZIP");
        s.checkIncludeExists("stdint.h").Definitions.insert("HAVE_STDINT_H_LIBZIP");
        s.checkIncludeExists("sys/types.h").Definitions.insert("HAVE_SYS_TYPES_H_LIBZIP");

        auto &f = s.checkIncludeExists("fts.h").Parameters;
        f.Includes.push_back("sys/types.h");
        f.Includes.push_back("sys/stat.h");

        s.checkTypeSize("__int8").Definitions.insert("__INT8_LIBZIP");
        s.checkTypeSize("int8_t").Definitions.insert("INT8_T_LIBZIP");
        s.checkTypeSize("uint8_t").Definitions.insert("UINT8_T_LIBZIP");
        s.checkTypeSize("__int16").Definitions.insert("__INT16_LIBZIP");
        s.checkTypeSize("int16_t").Definitions.insert("INT16_T_LIBZIP");
        s.checkTypeSize("uint16_t").Definitions.insert("UINT16_T_LIBZIP");
        s.checkTypeSize("__int32").Definitions.insert("__INT32_LIBZIP");
        s.checkTypeSize("int32_t").Definitions.insert("INT32_T_LIBZIP");
        s.checkTypeSize("uint32_t").Definitions.insert("UINT32_T_LIBZIP");
        s.checkTypeSize("__int64").Definitions.insert("__INT64_LIBZIP");
        s.checkTypeSize("int64_t").Definitions.insert("INT64_T_LIBZIP");
        s.checkTypeSize("uint64_t").Definitions.insert("UINT64_T_LIBZIP");
        s.checkTypeSize("short").Definitions.insert("SHORT_LIBZIP");
        s.checkTypeSize("int").Definitions.insert("INT_LIBZIP");
        s.checkTypeSize("long").Definitions.insert("LONG_LIBZIP");
        s.checkTypeSize("long long").Definitions.insert("LONG_LONG_LIBZIP");
        s.checkTypeSize("off_t").Definitions.insert("SIZEOF_OFF_T");
        s.checkTypeSize("size_t").Definitions.insert("SIZE_T_LIBZIP");
        s.checkTypeSize("ssize_t").Definitions.insert("SSIZE_T_LIBZIP");
    }
}
