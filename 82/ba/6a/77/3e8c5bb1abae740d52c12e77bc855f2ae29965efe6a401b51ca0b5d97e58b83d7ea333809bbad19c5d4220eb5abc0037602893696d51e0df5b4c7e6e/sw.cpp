void build(Solution &s)
{
    auto &t = s.addLibrary("libgit2", "1.8.4");
    t += Git("https://github.com/libgit2/libgit2");

    t += "include/.*"_rr;
    t += "src/libgit2/.*"_rr;
    t += "src/util/.*"_rr;
    t += "deps/xdiff/.*"_rr;

    t -= "src/util/hash/builtin.c";
    t -= "src/util/hash/collisiondetect.c";
    t -= "src/util/hash/common_crypto.c";
    t -= "src/util/hash/rfc6234/.*"_rr;
    t -= "src/util/hash/sha1dc/.*"_rr;

    if (t.getBuildSettings().TargetOS.Type == OSType::Windows) {
        t -= "src/util/unix/.*"_rr;
        t.Variables["GIT_IO_WSAPOLL"] = 1;
        t.Variables["GIT_QSORT_MSC"] = 1;
        t += "Secur32.lib"_slib;
    } else {
        //t += c99;
        t -= "src/util/hash/win32.*"_rr;
        t -= "src/util/win32.*"_rr;
        t.Variables["GIT_IO_POLL"] = 1;
        t.Variables["GIT_QSORT_GNU"] = 1;
        t += "_GNU_SOURCE"_def;
    }

    t.Public += "include"_idir;
    t += "src/libgit2"_idir;
    t += "src/util"_idir;
    t += "deps/xdiff"_idir;

    t += "GIT_HTTPPARSER_LLHTTP"_def;
    t += "deps/llhttp/.*"_rr;
    t += "deps/llhttp"_idir;

    t.Variables["GIT_THREADS"] = 1;
    //t.Variables["GIT_ARCH_32"] = 1;
    t.Variables["GIT_ARCH_64"] = 1;
    t.Variables["GIT_REGEX_PCRE2"] = 1;
    t.Variables["GIT_QSORT_C11"] = 1;
    t.Variables["GIT_HTTPS"] = 1;
    t.Variables["GIT_OPENSSL"] = 1;
    t.Variables["GIT_SHA1_OPENSSL"] = 1;
    t.Variables["GIT_SHA256_OPENSSL"] = 1;
    t.configureFile("src/util/git2_features.h.in", "git2_features.h");

/*
#cmakedefine GIT_USE_ICONV 1
#cmakedefine GIT_USE_NSEC 1
#cmakedefine GIT_USE_STAT_MTIM 1
#cmakedefine GIT_USE_STAT_MTIMESPEC 1
#cmakedefine GIT_USE_STAT_MTIME_NSEC 1
#cmakedefine GIT_USE_FUTIMENS 1

#cmakedefine GIT_REGEX_REGCOMP_L
#cmakedefine GIT_REGEX_REGCOMP
#cmakedefine GIT_REGEX_PCRE
#cmakedefine GIT_REGEX_PCRE2
#cmakedefine GIT_REGEX_BUILTIN 1

#cmakedefine GIT_QSORT_BSD
#cmakedefine GIT_QSORT_GNU
#cmakedefine GIT_QSORT_C11
#cmakedefine GIT_QSORT_MSC

#cmakedefine GIT_SSH 1
#cmakedefine GIT_SSH_MEMORY_CREDENTIALS 1

#cmakedefine GIT_NTLM 1
#cmakedefine GIT_GSSAPI 1
#cmakedefine GIT_GSSFRAMEWORK 1

#cmakedefine GIT_WINHTTP 1
#cmakedefine GIT_HTTPS 1
#cmakedefine GIT_OPENSSL 1
#cmakedefine GIT_OPENSSL_DYNAMIC 1
#cmakedefine GIT_SECURE_TRANSPORT 1
#cmakedefine GIT_MBEDTLS 1
#cmakedefine GIT_SCHANNEL 1

#cmakedefine GIT_SHA1_COLLISIONDETECT 1
#cmakedefine GIT_SHA1_WIN32 1
#cmakedefine GIT_SHA1_COMMON_CRYPTO 1
#cmakedefine GIT_SHA1_OPENSSL 1
#cmakedefine GIT_SHA1_OPENSSL_DYNAMIC 1
#cmakedefine GIT_SHA1_MBEDTLS 1

#cmakedefine GIT_SHA256_BUILTIN 1
#cmakedefine GIT_SHA256_WIN32 1
#cmakedefine GIT_SHA256_COMMON_CRYPTO 1
#cmakedefine GIT_SHA256_OPENSSL 1
#cmakedefine GIT_SHA256_OPENSSL_DYNAMIC 1
#cmakedefine GIT_SHA256_MBEDTLS 1

#cmakedefine GIT_RAND_GETENTROPY 1
#cmakedefine GIT_RAND_GETLOADAVG 1

#cmakedefine GIT_IO_POLL 1
#cmakedefine GIT_IO_WSAPOLL 1
#cmakedefine GIT_IO_SELECT 1
*/

    t += "org.sw.demo.madler.zlib"_dep;
    t += "org.sw.demo.openssl.ssl"_dep;
    t += "org.sw.demo.pcre.pcre8"_dep;
    //t += "GIT_HTTPPARSER_HTTPPARSER"_def;
    //t += "org.sw.demo.nodejs.http_parser"_dep;
}
