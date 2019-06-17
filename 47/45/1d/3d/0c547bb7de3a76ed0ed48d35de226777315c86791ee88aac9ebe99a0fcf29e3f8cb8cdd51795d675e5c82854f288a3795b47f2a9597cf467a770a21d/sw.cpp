void build(Solution &s)
{
    auto &nasm = s.addTarget<ExecutableTarget>("nasm", "2.14.2");
    nasm += RemoteFile("https://www.nasm.us/pub/nasm/releasebuilds/{M}.{m}.{p:02}/nasm-{M}.{m}.{p:02}.tar.xz");

    nasm.setChecks("nasm");

    nasm +=
        ".*\\.h"_rr,
        "asm/.*\\.c"_rr,
        "output/.*\\.c"_rr,
        "nasmlib/.*\\.c"_rr,
        "common/.*\\.c"_rr,
        "macros/.*\\.c"_rr,
        "stdlib/.*\\.c"_rr,
        "x86/.*\\.c"_rr
        ;
    nasm +=
        "."_idir,
        "include"_idir,
        "x86"_idir,
        "asm"_idir,
        "output"_idir
        ;

    nasm.Public +=
        "include"_id;

}

void check(Checker &c)
{
    auto &s = c.addSet("nasm");
    s.checkFunctionExists("canonicalize_file_name");
    s.checkFunctionExists("getgid");
    s.checkFunctionExists("getuid");
    s.checkFunctionExists("pathconf");
    s.checkFunctionExists("realpath");
    s.checkFunctionExists("strcasecmp");
    s.checkFunctionExists("strcspn");
    s.checkFunctionExists("stricmp");
    s.checkFunctionExists("strlcpy");
    s.checkFunctionExists("strncasecmp");
    s.checkFunctionExists("strnicmp");
    s.checkFunctionExists("strsep");
    s.checkFunctionExists("strspn");
    s.checkFunctionExists("vsnprintf");
    s.checkFunctionExists("_fullpath");
    s.checkFunctionExists("_snprintf");
    s.checkFunctionExists("_vsnprintf");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("limits.h");
    s.checkIncludeExists("memory.h");
    s.checkIncludeExists("stdbool.h");
    s.checkIncludeExists("stddef.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("sys/param.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("unistd.h");
    s.checkTypeSize("size_t");
    s.checkTypeSize("void *");
    {
        auto &c = s.checkSymbolExists("snprintf");
        c.Parameters.Includes.push_back("stdio.h");
    }
    s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <float.h>
    int main() {return 0;}
    )sw_xxx");
    s.checkDeclarationExists("decl");
    s.checkDeclarationExists("decl");
    s.checkDeclarationExists("decl");
    s.checkDeclarationExists("decl");
    s.checkDeclarationExists("decl");
    s.checkDeclarationExists("decl");
}
