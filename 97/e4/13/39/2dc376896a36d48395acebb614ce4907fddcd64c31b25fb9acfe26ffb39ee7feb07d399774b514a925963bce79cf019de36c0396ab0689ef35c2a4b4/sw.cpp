void build(Solution &s)
{
    auto &libressl = s.addProject("libressl", "4.1.0");
    libressl += RemoteFile("https://ftp.openbsd.org/pub/OpenBSD/LibreSSL/libressl-{v}.tar.gz");

    auto &crypto = libressl.addTarget<LibraryTarget>("crypto");
    {
        crypto.setChecks("crypto");
        crypto.ExportAllSymbols = true;

        crypto +=
            "crypto/.*\\.[hc]"_rr,
            "include/.*\\.h"_rr;

        crypto -=
            "crypto/.*aix.*"_rr,
            "crypto/.*freebsd.*"_rr,
            "crypto/.*hpux.*"_rr,
            "crypto/.*linux.*"_rr,
            "crypto/.*netbsd.*"_rr,
            "crypto/.*osx.*"_rr,
            "crypto/.*posix.*"_rr,
            "crypto/.*solaris.*"_rr,
            "crypto/.*unimpl.*"_rr,
            "crypto/.*win.*"_rr
            ;

        crypto += "crypto/asn1/a_time_posix.c"_rr;
        crypto += "WIN32_LEAN_AND_MEAN"_def;
        crypto += "advapi32.lib"_slib;
        crypto += "ws2_32.lib"_slib;

        crypto -= "crypto/armcap.c";
        crypto -= "crypto/bio/bss_log.c";
        //crypto -= "crypto/engine/eng_all.c";
        crypto -= "crypto/compat/explicit_bzero.c";
        crypto -= "crypto/chacha/chacha-merged.c";
        //crypto -= "crypto/des/ncbc_enc.c";
        crypto -= "crypto/poly1305/poly1305-donna.c";
        crypto -= "crypto/crypto_lock.c";
        crypto -= "crypto/ui/ui_openssl.c";
        crypto -= "crypto/bn/arch/amd64/bn_arch.c";

        crypto += "LIBRESSL_INTERNAL"_def;
        crypto += "OPENSSL_NO_ASM"_def;
        crypto += "OPENSSL_NO_HW"_def;
        crypto += "HAVE_LITTLE_ENDIAN"_def;

        crypto +=
            "include/compat"_id,
            "crypto/bio"_id,
            "crypto/bn"_id,
            "crypto/bytestring"_id,
            "crypto/err"_id,
            "crypto/evp"_id,
            "crypto/bn"_id,
            "crypto/ec"_id,
            "crypto/ecdh"_id,
            "crypto/ecdsa"_id,
            "crypto"_id,
            "crypto/modes"_id,
            "crypto/dsa"_id,
            "crypto/x509"_id,
            "crypto/dh"_id,
            "crypto/hmac"_id,
            "crypto/conf"_id,
            "crypto/sha"_id,
            "crypto/ocsp"_id,
            "crypto/curve25519"_id,
            "crypto/pkcs12"_id,
            "crypto/rsa"_id,
            "crypto/stack"_id,
            "crypto/asn1"_id
            ;
        crypto +=
            "crypto/arch/amd64"_id,
            "crypto/bn/arch/amd64"_id
            ;

        crypto.Public +=
            "include"_id;

        crypto.Public += "__BEGIN_HIDDEN_DECLS="_d;
        crypto.Public += "__END_HIDDEN_DECLS="_d;

        if (crypto.getBuildSettings().TargetOS.Type == OSType::Windows || crypto.getBuildSettings().TargetOS.Type == OSType::Mingw) {
            crypto += "bcrypt.lib"_slib;
        }
    }

    return;

    auto &ssl = libressl.addTarget<LibraryTarget>("ssl");
    {
        ssl.setChecks("crypto");
        ssl.ExportAllSymbols = true;

        ssl += "ssl/.*\\.[hc]"_rr;
        ssl.Public += "ssl"_id;
        ssl.Public += crypto;

        //ssl.writeFileOnce("unistd.h");
    }

    auto &tls = libressl.addTarget<LibraryTarget>("tls");
    {
        tls.setChecks("crypto");
        tls.ExportAllSymbols = true;

        tls += "tls/.*\\.[hc]"_rr;
        tls.Public += "tls"_id;
        tls.Public += ssl;
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("crypto");
    s.checkFunctionExists("arc4random_buf");
    s.checkFunctionExists("asprintf");
    s.checkFunctionExists("explicit_bzero");
    s.checkFunctionExists("getauxval");
    s.checkFunctionExists("getentropy");
    s.checkFunctionExists("inet_pton");
    s.checkFunctionExists("reallocarray");
    s.checkFunctionExists("strcasecmp");
    s.checkFunctionExists("strlcat");
    s.checkFunctionExists("strlcpy");
    s.checkFunctionExists("strndup");
    s.checkFunctionExists("strnlen");
    s.checkFunctionExists("strsep");
    s.checkFunctionExists("timegm");
    s.checkFunctionExists("timingsafe_bcmp");
    s.checkFunctionExists("timingsafe_memcmp");
    s.checkIncludeExists("err.h");
    s.checkTypeSize("size_t");
    s.checkTypeSize("void *");
}
