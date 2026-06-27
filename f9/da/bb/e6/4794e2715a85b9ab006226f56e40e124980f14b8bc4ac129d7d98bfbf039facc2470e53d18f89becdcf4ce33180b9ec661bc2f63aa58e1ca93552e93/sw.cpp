#pragma sw require header org.sw.demo.perl.perl

void build(Solution &s)
{
    auto &openssl = s.addProject("openssl", "4.0.1");
    //openssl += Git("https://github.com/openssl/openssl", "openssl-{v}");
    openssl += RemoteFile("https://github.com/openssl/openssl/releases/download/openssl-{v}/openssl-{v}.tar.gz");

    auto &crypto = openssl.addTarget<LibraryTarget>("crypto");
    {
        // perl scripts, data and other files required by Configure
        // Configure read build.info files. Maybe we can remove perl dep if we read them ourselves?
        // build.info uses perl insertions, probably no we can't
        crypto -=
            "Configurations/.*"_rr,
            //"test/.*"_rr,
            "ssl/.*"_rr,

            "external/.*"_rr,
            "util/.*"_rr,
            "ssl/.*"_rr,

            "crypto/.*"_rr,
            //"engines/.*"_rr,
            "providers/.*"_rr,

            "ms/.*"_rr,
            "config.*"_r,
            ".*\\.in"_r,
            "VERSION.dat",
            ".*build\\.info"_rr,

            // for configure
            "apps/.*"_rr,
            "fuzz/.*"_rr
            ;

        crypto +=
            "crypto/.*\\.[hc]"_rr,
            //"engines/.*\\.[hc]"_rr,
            "providers/.*\\.[hc]"_rr,
            "include/.*\\.[hc]"_rr
            ;

        crypto -=
            "crypto/LPdir_.*"_rr,
            "crypto/.*/asm/.*"_rr,
            "crypto/.*_ppc.c"_rr,
            "crypto/.*_sparc.c"_rr,
            "crypto/.*riscv.*"_rr,
            //"crypto/.*legacy.*"_rr,
            "crypto/des/ncbc_enc.c",
            "crypto/lms/.*"_rr,
            "crypto/md2/.*"_rr,
            "crypto/ec/ecp_sm2p256.c",
            "crypto/rsa/rsa_acvp_test_params.c",
            "crypto/evp/legacy_md2.c",
            "crypto/rc5/.*"_rr,
            //"providers/.*"_rr,
            "providers/fips/.*"_rr,
            "providers/legacyprov.c",
            "providers/implementations/rands/seeding/rand_vxworks.c",
            "providers/implementations/rands/seeding/rand_vms.c",
            "providers/implementations/encode_decode/decode_lmsxdr2key.c",
            "providers/implementations/ciphers/cipher_rc5_hw.c",
            "providers/common/securitycheck_fips.c",
            "providers/implementations/digests/md2_prov.c",
            "providers/implementations/macs/blake2_mac_impl.c",
            "providers/implementations/signature/lms_signature.c",
            "providers/implementations/ciphers/cipher_rc5.c"
            //,
            //"engines/e_afalg.*"_rr,
            //"engines/e_devcrypto.c",
            //"engines/e_loader_attic.c"
            ;
        if (crypto.getBuildSettings().TargetOS.Arch == ArchType::aarch64)
            crypto -= "crypto/poly1305/poly1305_ieee754.c";

        //crypto += "providers/legacyprov.c";
        crypto += "providers/baseprov.c";
        crypto += "providers/defltprov.c";
        crypto += "providers/nullprov.c";
        crypto += "providers/prov_running.c";
        //crypto -= "providers/implementations/.*"_rr;
        crypto -= "providers/implementations/keymgmt/lms_kmgmt.c";
        crypto -= "providers/implementations/rands/fips_crng_test.c";
        crypto -= "providers/implementations/encode_decode/lms_codecs.c";

        crypto += "ssl/record/methods/tls_pad.c";
        crypto += "ssl/record/methods/ssl3_cbc.c";
        //crypto += "ssl/record/methods/ssl3_meth.c";
        //crypto += "ssl/record/methods/tls13_meth.c";

        crypto.Private +=
            "."_id,
            "crypto/modes"_id,
            "crypto/asn1"_id,
            "crypto/dsa"_id,
            "crypto/evp"_id,
            "crypto/ec/curve448"_id,
            "crypto/ec/curve448/arch_32"_id;

        crypto.Public += "include"_id;
        crypto += "providers/common/include"_id;
        crypto += "providers/fips/include"_id;
        crypto += "providers/implementations/include"_id;
        crypto += "providers/implementations/ciphers"_id;

        crypto.ExportAllSymbols = true;

        auto is_win_or_mingw = [](auto &os) {
            return
                os.Type == OSType::Windows ||
                os.Type == OSType::Mingw
            ;
        };
        auto win_or_mingw = is_win_or_mingw(crypto.getBuildSettings().TargetOS);

        auto add_perl_dep = [&](auto &&c, auto &&dep) {
            return add_perl_dependency(crypto, c, dep);
        };
        auto perl_command = [&]() {
            auto c = make_perl_command(crypto);
            c << "-I" + crypto.SourceDir.string();
            c << "-I" + crypto.BinaryDir.string();
            c << "-I" + crypto.SourceDir.string() + "/util/perl";
            c << "-I" + crypto.SourceDir.string() + "/external/perl/Text-Template-1.56/lib";
            add_perl_dep(c, "org.sw.demo.perl.packages.dist.PathTools.Cwd"_dep);
            add_perl_dep(c, "org.sw.demo.perl.packages.cpan.Encode"_dep);
            add_perl_dep(c, "org.sw.demo.perl.packages.dist.Storable"_dep);
            add_perl_dep(c, "org.sw.demo.perl.packages.ext.Fcntl"_dep);
            if (is_win_or_mingw(::sw::getHostOS())) {
                add_perl_dep(c, "org.sw.demo.perl.packages.cpan.Win32"_dep);
            } else {
                crypto -= "org.sw.demo.perl.packages.cpan.Win32"_dep;
            }
            add_perl_dep(c, "org.sw.demo.perl.packages.ext.File.Glob"_dep);
            add_perl_dep(c, "org.sw.demo.perl.packages.dist.IO"_dep);
            add_perl_dep(c, "org.sw.demo.perl.packages.ext.POSIX"_dep);
            //add_perl_dep(c, "org.sw.demo.perl.packages.cpan.Text.Template"_dep);
            // for generate2()
            add_perl_dep(c, "org.sw.demo.perl.packages.ext.re"_dep);
            return c;
        };

        {
            crypto.patch("build.info", "crypto ssl apps util tools fuzz providers doc", "crypto ssl providers");
            crypto.patch("build.info", "SUBDIRS=test", "# SUBDIRS= test");

            crypto.patch("Configure", "system($cmd);", "#system( $cmd);");
            crypto.patch("Configure", "exit 1 if $? != 0;", "#exit  1 if $? != 0;");
            crypto.patch("Configure", "elsif (/^--libdir=(.*)$/)", "elsif (/^--configdata_outname=(.*)$/) { $config{configdata_outname}=$1; } elsif  (/^--libdir=(.*)$/)");
            crypto.patch("Configure", "elsif  (/^--libdir=(.*)$/)", "elsif (/^--target=(.*)$/) { $target=$1; } elsif   (/^--libdir=(.*)$/)");
            crypto.patch("Configure", "'configdata.pm'", "$config{configdata_outname}");
            crypto.patch("Configure", "if (! do $checker_path) {", "if (0 && !  do $checker_path) {");

            auto c = perl_command();
            c << "Configure";
            c << cmd::out("configdata.pm", cmd::Prefix{"--configdata_outname="});
            // during cross compilation Configure cannot determine correct config
            if (!win_or_mingw) {
                auto os = "linux"s;
                auto arch = "x86_64"s;
                if (crypto.getBuildSettings().TargetOS.isApple()) {
                    os = "darwin64";
                }
                if (crypto.getBuildSettings().TargetOS.Arch == ArchType::aarch64) {
                    arch = "arm64";
                }
                auto target = std::format("--target={}-{}", os, arch);
                c << target;
            }

            // win only?
            if (!crypto.DryRun) {
                auto prog = crypto.findProgramByExtension(".c");
                if (!prog) {
                    throw SW_RUNTIME_ERROR("cant find C compiler");
                }
                c << cmd::env("CC", prog->file.string());
            }
            auto nasm = "org.sw.demo.nasm"_dep;
            nasm = crypto.addProgDependency(nasm);
            // win only?
            if (!crypto.DryRun) {
                auto dep = nasm;
                auto exe = crypto.getObjFile(dep, "bin");
                exe /= crypto.getMainBuild().getContext().resolve(dep->getPackage()).toString();
                if (win_or_mingw) {
                    //exe += ".exe";
                }
                auto exename = exe.filename().string();
                crypto.patch("Configurations/10-main.conf", "`nasmw", "`"s + normalize_string_copy(exename));
                crypto.patch("Configurations/10-main.conf", "`nasm", "`"s + normalize_string_copy(exename));
                crypto.patch("Configurations/10-main.conf", "AS        => \"nasm\"", "AS => \"" + normalize_string_copy(exename) + "\""s);
                crypto.patch("Configurations/10-main.conf", "? \"nasm\"", "? \"" + normalize_string_copy(exename) + "\"");
                crypto.patch("Configurations/10-main.conf", ": \"nasmw\"", ": \"" + normalize_string_copy(exename) + "\"");
            }
            c.addRuntimeDependency(nasm);
        }

        auto generate = [&](const std::string &f) {
            auto c = perl_command();
            c << "-I" << crypto.BinaryDir; // for configdata.pm
            c << "-Mconfigdata";
            c << "-MOpenSSL::paramnames";
            c << cmd::in("util/dofile.pl");
            c << "-oMakefile";
            c << cmd::in(f + ".in"s);
            c << cmd::end();
            c << cmd::in("util/perl/OpenSSL/paramnames.pm");
            c << cmd::std_out(path{f}.lexically_relative("include"));
            c << cmd::end();
            c << cmd::in("configdata.pm");
            // /usr/bin/perl "-I." "-Mconfigdata" "util/dofile.pl" "-oMakefile" include/crypto/bn_conf.h.in > include/crypto/bn_conf.h
            // /usr/bin/perl "-I." "-Iutil/perl" "-Mconfigdata" "-MOpenSSL::paramnames" "util/dofile.pl" "-oMakefile" include/openssl/core_names.h.in > include/openssl/core_names.h
        };
        auto generate2 = [&](const std::string &f) {
            auto in = f + ".in"s;
            auto out = f;
            auto inc = "include/"sv;
            auto p = out.find(inc);
            if (p != -1) {
                out = out.substr(p + inc.size());
            }

            auto c = perl_command();
            c << "-I" << crypto.BinaryDir; // for configdata.pm
            c << "-I" << crypto.SourceDir / "providers/common/der";
            c << "-Mconfigdata";
            c << "-Moids_to_c";
            c << cmd::in("util/dofile.pl");
            c << "-oMakefile";
            c << cmd::in(in);
            c << cmd::end();
            c << cmd::std_out(out);
            c << cmd::end();
            c << cmd::in("configdata.pm");
            // /usr/bin/perl "-I." "-Iproviders/common/der" "-Mconfigdata" "-Mconfigdata" "-Mconfigdata" "-Moids_to_c" "util/dofile.pl" "-oMakefile" providers/common/der/der_digests_gen.c.in > providers/common/der/der_digests_gen.c
        };
        for (auto &&i : {
            // special?
            "include/openssl/configuration.h",

            "include/crypto/dso_conf.h",
            "include/openssl/asn1.h",
            "include/openssl/asn1t.h",
            "include/openssl/bio.h",
            "include/openssl/cmp.h",
            "include/openssl/cms.h",
            "include/openssl/comp.h",
            "include/openssl/conf.h",
            "include/openssl/core_names.h",
            "include/openssl/crmf.h",
            "include/openssl/crypto.h",
            "include/openssl/ct.h",
            "include/openssl/err.h",
            "include/openssl/ess.h",
            "include/openssl/fipskey.h",
            "include/openssl/lhash.h",
            "include/openssl/ocsp.h",
            "include/openssl/opensslv.h",
            "include/openssl/pkcs12.h",
            "include/openssl/pkcs7.h",
            "include/openssl/safestack.h",
            "include/openssl/srp.h",
            "include/openssl/ssl.h",
            "include/openssl/ui.h",
            "include/openssl/x509.h",
            "include/openssl/x509_acert.h",
            "include/openssl/x509_vfy.h",
            "include/openssl/x509v3.h",
            }) {
            generate(i);
        }
        for (auto &&i : {
            "providers/common/der/der_digests_gen.c",
            "providers/common/der/der_dsa_gen.c",
            "providers/common/der/der_ec_gen.c",
            "providers/common/der/der_ecx_gen.c",
            "providers/common/der/der_hkdf_gen.c",
            "providers/common/der/der_ml_dsa_gen.c",
            "providers/common/der/der_rsa_gen.c",
            "providers/common/der/der_slh_dsa_gen.c",
            "providers/common/der/der_sm2_gen.c",
            "providers/common/der/der_wrap_gen.c",

            "providers/common/include/prov/der_digests.h",
            "providers/common/include/prov/der_dsa.h",
            "providers/common/include/prov/der_ec.h",
            "providers/common/include/prov/der_ecx.h",
            "providers/common/include/prov/der_hkdf.h",
            "providers/common/include/prov/der_ml_dsa.h",
            "providers/common/include/prov/der_rsa.h",
            "providers/common/include/prov/der_slh_dsa.h",
            "providers/common/include/prov/der_sm2.h",
            "providers/common/include/prov/der_wrap.h",

            "providers/implementations/asymciphers/rsa_enc.inc",
            "providers/implementations/asymciphers/sm2_enc.inc",

            "providers/implementations/ciphers/cipher_aes_xts.inc",
            "providers/implementations/ciphers/cipher_aes_gcm_siv.inc",
            "providers/implementations/ciphers/cipher_aes_ocb.inc",
            "providers/implementations/ciphers/cipher_aes_siv.inc",
            "providers/implementations/ciphers/cipher_aes_wrp.inc",
            "providers/implementations/ciphers/cipher_chacha20.inc",
            "providers/implementations/ciphers/cipher_chacha20_poly1305.inc",
            "providers/implementations/ciphers/ciphercommon.inc",
            "providers/implementations/ciphers/ciphercommon_ccm.inc",
            "providers/implementations/ciphers/ciphercommon_gcm.inc",
            "providers/implementations/ciphers/cipher_sm4_xts.inc",
            "providers/implementations/ciphers/cipher_null.inc",
            "providers/implementations/ciphers/cipher_rc4_hmac_md5.inc",

            "providers/implementations/digests/blake2_prov.inc",
            "providers/implementations/digests/digestcommon.inc",
            "providers/implementations/digests/sha2_prov.inc",
            "providers/implementations/digests/sha3_prov.inc",
            "providers/implementations/digests/mdc2_prov.inc",
            "providers/implementations/digests/ml_dsa_mu_prov.inc",
            "providers/implementations/digests/cshake_prov.inc",

            "providers/implementations/encode_decode/decode_der2key.inc",
            "providers/implementations/encode_decode/decode_epki2pki.inc",
            "providers/implementations/encode_decode/decode_pem2der.inc",
            "providers/implementations/encode_decode/decode_pvk2key.inc",
            "providers/implementations/encode_decode/decode_spki2typespki.inc",
            "providers/implementations/encode_decode/encode_key2any.inc",
            "providers/implementations/encode_decode/encode_key2ms.inc",

            "providers/implementations/exchange/dh_exch.inc",
            "providers/implementations/exchange/ecdh_exch.inc",
            "providers/implementations/exchange/ecx_exch.inc",

            "providers/implementations/include/prov/blake2_params.inc",

            "providers/implementations/kdfs/argon2.inc",
            "providers/implementations/kdfs/hkdf.inc",
            "providers/implementations/kdfs/hmacdrbg_kdf.inc",
            "providers/implementations/kdfs/kbkdf.inc",
            "providers/implementations/kdfs/krb5kdf.inc",
            "providers/implementations/kdfs/pbkdf1.inc",
            "providers/implementations/kdfs/pbkdf2.inc",
            "providers/implementations/kdfs/pkcs12kdf.inc",
            "providers/implementations/kdfs/pvkkdf.inc",
            "providers/implementations/kdfs/scrypt.inc",
            "providers/implementations/kdfs/sshkdf.inc",
            "providers/implementations/kdfs/sskdf.inc",
            "providers/implementations/kdfs/srtpkdf.inc",
            "providers/implementations/kdfs/snmpkdf.inc",
            "providers/implementations/kdfs/tls1_prf.inc",
            "providers/implementations/kdfs/x942kdf.inc",
            "providers/implementations/kdfs/x963kdf.inc",

            "providers/implementations/kem/ec_kem.inc",
            "providers/implementations/kem/ecx_kem.inc",
            "providers/implementations/kem/ml_kem_kem.inc",
            "providers/implementations/kem/rsa_kem.inc",

            "providers/implementations/keymgmt/dsa_kmgmt.inc",
            "providers/implementations/keymgmt/dh_kmgmt.inc",
            "providers/implementations/keymgmt/ecx_kmgmt.inc",
            "providers/implementations/keymgmt/lms_kmgmt.inc",
            "providers/implementations/keymgmt/ml_dsa_kmgmt.inc",
            "providers/implementations/keymgmt/ml_kem_kmgmt.inc",
            "providers/implementations/keymgmt/mlx_kmgmt.inc",
            "providers/implementations/keymgmt/slh_dsa_kmgmt.inc",
            "providers/implementations/keymgmt/template_kmgmt.inc",
            "providers/implementations/keymgmt/mac_legacy_kmgmt.inc",

            "providers/implementations/macs/cmac_prov.inc",
            "providers/implementations/macs/gmac_prov.inc",
            "providers/implementations/macs/hmac_prov.inc",
            "providers/implementations/macs/kmac_prov.inc",
            "providers/implementations/macs/poly1305_prov.inc",
            "providers/implementations/macs/siphash_prov.inc",

            "providers/implementations/rands/drbg_ctr.inc",
            "providers/implementations/rands/drbg_hash.inc",
            "providers/implementations/rands/drbg_hmac.inc",
            "providers/implementations/rands/fips_crng_test.inc",
            "providers/implementations/rands/seed_src.inc",
            "providers/implementations/rands/seed_src_jitter.inc",
            "providers/implementations/rands/test_rng.inc",

            "providers/implementations/signature/dsa_sig.inc",
            "providers/implementations/signature/ecdsa_sig.inc",
            "providers/implementations/signature/eddsa_sig.inc",
            "providers/implementations/signature/ml_dsa_sig.inc",
            "providers/implementations/signature/rsa_sig.inc",
            "providers/implementations/signature/slh_dsa_sig.inc",
            "providers/implementations/signature/sm2_sig.inc",

            "providers/implementations/skeymgmt/generic.inc",

            "providers/implementations/storemgmt/file_store.inc",
            "providers/implementations/storemgmt/file_store_any2obj.inc",
            "providers/implementations/storemgmt/winstore_store.inc",
            }) {
            generate2(i);
        }
        if (!win_or_mingw) {
            crypto -= "providers/implementations/storemgmt/winstore_store.c";
        }

        crypto.Private += "NO_WINDOWS_BRAINDEATH"_d;
        crypto.Private += "OPENSSL_NO_DYNAMIC_ENGINE"_d;
        crypto.Private += "OPENSSL_RAND_SEED_OS"_d;
        crypto.Public += "OPENSSL_NO_ASM"_d;
        crypto.Public += "OPENSSL_NO_ENGINE"_d;
        if (win_or_mingw)
        {
            crypto.Protected += "WIN32_LEAN_AND_MEAN"_d; // pub?
            crypto += "DSO_WIN32"_def;
            crypto.Public += "Crypt32.lib"_slib;
            crypto.Public += "Bcrypt.lib"_slib;
            crypto.Public += "ws2_32.lib"_slib, "advapi32.lib"_slib, "User32.lib"_slib;
        }
        else
        {
            crypto.Private += "HAVE_DLFCN_H"_d;
            crypto.Private += "DSO_DLFCN"_d;
            //crypto.Private += "DSO_DL"_d;
            crypto += "dl"_slib;
            if (!crypto.getBuildSettings().TargetOS.Android)
                crypto += "pthread"_slib;
        }

        if (crypto.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            crypto.Public.Definitions["OPENSSLDIR"] = "\"C:/Program Files/Common Files/SSL/\"";
            crypto.Public.Definitions["ENGINESDIR"] = "\"C:/Program Files/OpenSSL/lib/engines/\"";
            crypto.Public.Definitions["MODULESDIR"] = "\"C:/Program Files/OpenSSL/lib/modules/\"";
        }
        else
        {
            crypto.Public.Definitions["OPENSSLDIR"] = "\"/usr/local/ssl\"";
            crypto.Public.Definitions["ENGINESDIR"] = "\"/usr/local/ssl/lib/engines\"";
            crypto.Public.Definitions["MODULESDIR"] = "\"/usr/local/lib64/ossl-modules\"";
        }

        crypto -=
            // arch stuff
            "crypto/armcap.c",
            "crypto/.*s390.*"_rr,
            "crypto/sparcv9cap.c",
            "crypto/loongarchcap.c",
            "crypto/ppccap.c",
            "crypto/aes/aes_x86core.c",

            "crypto/ec/ecp_nistz256.c",
            "crypto/ec/ecp_nistz256_table.c"
            ;

        if (win_or_mingw)
        {
            crypto -= "crypto/poly1305/poly1305_ieee754.c";
            // gcc/clang
            crypto -=
                "crypto/ec/ecp_nistp224.c",
                "crypto/ec/ecp_nistp256.c",
                "crypto/ec/ecp_nistp384.c",
                "crypto/ec/ecp_nistp521.c",
                "crypto/ec/ecp_nistputil.c"
            ;
        } else {
            crypto -=
                "crypto/ec/ecp_nistp224.c",
                "crypto/ec/ecp_nistp256.c",
                "crypto/ec/ecp_nistp384.c",
                "crypto/ec/ecp_nistp521.c",
                "crypto/ec/ecp_nistputil.c"
            ;
        }

        crypto -= "crypto/poly1305/poly1305_base2_44.c";
        //crypto -= "crypto/engine/eng_devcrypto.c";

        crypto.writeFileOnce(crypto.BinaryPrivateDir / "buildinf.h", R"xxx(
#define DATE ""
#define compiler_flags ""
#define PLATFORM ""
    )xxx");
    }

    auto &ssl = openssl.addTarget<LibraryTarget>("ssl");
    {
        ssl.ExportAllSymbols = true;
        ssl += "ssl/.*\\.[hc]"_rr;
        ssl -= "ssl/record/methods/ktls_meth.c";
        ssl.Public += crypto;
    }
}
