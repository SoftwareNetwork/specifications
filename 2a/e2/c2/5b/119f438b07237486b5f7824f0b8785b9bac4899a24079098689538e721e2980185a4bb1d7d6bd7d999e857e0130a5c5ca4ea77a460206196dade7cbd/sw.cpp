void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("gnu.shishi", "1.0.2");
    t += RemoteFile("https://ftp.gnu.org/gnu/shishi/shishi-{M}.{m}.{p}.tar.gz");

    t.PackageDefinitions = true;

    t +=
        "lib/[^/]*\\.c"_rr,
        "lib/[^/]*\\.h"_rr;

    t += "HAVE_CONFIG_H"_def;
    t += "_GL_ATTRIBUTE_CONST="_def;
    t += "restrict=__restrict"_def;

    t += "gl"_idir;
    t.Public += "lib"_idir;

    t += "org.sw.demo.gnu.gettext.intl"_dep;

    t.writeFileOnce(t.BinaryPrivateDir / "shishi-int.h", "#include <stdint.h>");
    t.writeFileOnce(t.BinaryPrivateDir / "sys/socket.h", "");
    t.writeFileOnce(t.BinaryPrivateDir / "sys/select.h", "");
    t.writeFileOnce(t.BinaryPrivateDir / "sys/time.h", "");
    t.writeFileOnce(t.BinaryPrivateDir / "unistd.h", "");
    t.writeFileOnce(t.BinaryPrivateDir / "netdb.h", "");
    t.writeFileOnce(t.BinaryPrivateDir / "netinet/in.h", "");
    t.writeFileOnce(t.BinaryPrivateDir / "arpa/inet.h", "");

    t.writeFileOnce(t.BinaryPrivateDir / "config.h", R"xxx(
#pragma once

/* The __pure__ attribute was added in gcc 2.96.  */
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 96)
# define _GL_ATTRIBUTE_PURE __attribute__ ((__pure__))
#else
# define _GL_ATTRIBUTE_PURE /* empty */
#endif

#define _GL_ATTRIBUTE_MALLOC

#define _GL_INLINE inline
#define _GL_EXTERN_INLINE inline

# define _GL_INLINE_HEADER_BEGIN
# define _GL_INLINE_HEADER_END

#define _GL_ASYNC_SAFE

/* The _Noreturn keyword of C11.  */
#if ! (defined _Noreturn \
        || (defined __STDC_VERSION__ && 201112 <= __STDC_VERSION__))
# if (3 <= __GNUC__ || (__GNUC__ == 2 && 8 <= __GNUC_MINOR__) \
        || 0x5110 <= __SUNPRO_C)
#  define _Noreturn __attribute__ ((__noreturn__))
# elif defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn
# endif
#endif

#define _GL_UNUSED
#define __getopt_argv_const const
#define _GL_ATTRIBUTE_CONST

#define _GL_EXTERN_C extern

#define _GL_FUNCDECL_SYS(func,rettype,parameters_and_attributes) \
    _GL_EXTERN_C rettype func parameters_and_attributes

# define _GL_CXXALIAS_SYS(func,rettype,parameters) \
    _GL_EXTERN_C int _gl_cxxalias_dummy

    )xxx"
    );
}
