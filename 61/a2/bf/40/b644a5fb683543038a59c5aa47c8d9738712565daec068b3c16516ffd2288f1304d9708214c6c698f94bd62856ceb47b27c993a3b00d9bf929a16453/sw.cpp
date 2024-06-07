struct PythonExecutable : ExecutableTarget {
    using ExecutableTarget::ExecutableTarget;

    void setupCommand(builder::Command &c) const override {
        /*if (getBuildSettings().TargetOS.Type != OSType::Windows)
        {
        c.setProgram("python3");
        return;
        }*/
        ExecutableTarget::setupCommand(c);
        // c.environment["PYTHONHOME"] = to_printable_string(to_path_string(SourceDir / "Lib"));
        c.environment["PYTHONPATH"] = to_printable_string(to_path_string(SourceDir / "Lib"));
        // used in sw package loading
        c.environment["SW_EXECUTABLE"] = to_printable_string(to_path_string(sw::getProgramLocation()));
    }

    void setupCommandForRun(builder::Command &c) const override {
        setupCommand(c);
    }
};

void build(Solution &s) {
    auto &python = s.addProject("python", "3.12.4");
    python += Git("https://github.com/python/cpython", "v{v}");

    auto python_lib = [](auto &&lib) -> decltype(auto) {
    {
        lib.setChecks("lib");//, true);

        lib -= "PC/.*"_rr, "Include/.*"_rr, "Python/.*"_rr, "Objects/.*"_rr, "Modules/.*"_rr;

        lib += "pyconfig.h.in", "Include/.*"_rr, "Modules/.*\\.h"_rr, "Modules/_abc.c", "Modules/_bisectmodule.c",
            "Modules/_blake2/.*\\.[hc]"_rr, "Modules/_bz2module.c", "Modules/_codecsmodule.c",
            "Modules/_collectionsmodule.c", "Modules/_contextvarsmodule.c", "Modules/_csv.c",
            "Modules/_datetimemodule.c", "Modules/_functoolsmodule.c", "Modules/_heapqmodule.c",
            "Modules/_io/.*\\.[hc]"_rr, "Modules/_json.c", "Modules/_localemodule.c", "Modules/_lsprof.c",
            "Modules/_lzmamodule.c",
            //"Modules/_math.c",
            "Modules/_opcode.c", "Modules/_operator.c",
            //"Modules/_peg_parser.c",
            "Modules/_pickle.c", "Modules/_randommodule.c",
            //"Modules/_sha3/.*\\.c"_rr, "Modules/_sha3/.*\\.h"_rr,
            //"Modules/_sha3/.*\\.inc"_rr, "Modules/_sha3/.*\\.macros"_rr,
            "Modules/_sre/.*"_rr,
            "Modules/sha2module.c",
            "Modules/sha3module.c",
            "Modules/_typingmodule.c",
            "Modules/_xxinterpchannelsmodule.c",
            "Modules/_stat.c",
            "Modules/_statisticsmodule.c", "Modules/_struct.c", "Modules/_threadmodule.c", "Modules/_tracemalloc.c",
            "Modules/_weakref.c", "Modules/_xxsubinterpretersmodule.c", "Modules/arraymodule.c",
            "Modules/atexitmodule.c", "Modules/audioop.c", "Modules/binascii.c", "Modules/cjkcodecs/_codecs_cn.c",
            "Modules/cjkcodecs/_codecs_hk.c", "Modules/cjkcodecs/_codecs_iso2022.c", "Modules/cjkcodecs/_codecs_jp.c",
            "Modules/cjkcodecs/_codecs_kr.c", "Modules/cjkcodecs/_codecs_tw.c", "Modules/cjkcodecs/multibytecodec.c",
            "Modules/cmathmodule.c", "Modules/config.c.in", "Modules/errnomodule.c", "Modules/faulthandler.c",
            "Modules/gcmodule.c", "Modules/getbuildinfo.c", "Modules/itertoolsmodule.c",
            "Modules/main.c", "Modules/mathmodule.c", "Modules/md5module.c", "Modules/mmapmodule.c",
            //"Modules/parsermodule.c",
            "Modules/posixmodule.c", "Modules/pwdmodule.c", "Modules/pyexpat.c", "Modules/rotatingtree.c",
            "Modules/sha1module.c",
            //"Modules/sha256module.c", "Modules/sha512module.c",
            "Modules/signalmodule.c",
            "Modules/symtablemodule.c", "Modules/timemodule.c", "Modules/xxsubtype.c", "Modules/zlibmodule.c"
            , "Modules/_tracemalloc.c"

            , "Objects/.*\\.h"_rr, "Objects/.*\\.inc"_rr, "Objects/abstract.c",
            //"Objects/accu.c",
            "Objects/boolobject.c",
            "Objects/bytearrayobject.c", "Objects/bytes_methods.c", "Objects/bytesobject.c", "Objects/call.c",
            "Objects/capsule.c", "Objects/cellobject.c", "Objects/classobject.c", "Objects/codeobject.c",
            "Objects/complexobject.c", "Objects/descrobject.c", "Objects/dictobject.c", "Objects/enumobject.c",
            "Objects/exceptions.c", "Objects/fileobject.c", "Objects/floatobject.c", "Objects/frameobject.c",
            "Objects/funcobject.c", "Objects/genericaliasobject.c", "Objects/genobject.c", "Objects/iterobject.c",
            "Objects/interpreteridobject.c", "Objects/listobject.c", "Objects/longobject.c", "Objects/memoryobject.c",
            "Objects/methodobject.c", "Objects/moduleobject.c", "Objects/namespaceobject.c", "Objects/object.c",
            "Objects/obmalloc.c", "Objects/odictobject.c", "Objects/picklebufobject.c", "Objects/rangeobject.c",
            "Objects/setobject.c", "Objects/sliceobject.c", "Objects/structseq.c", "Objects/tupleobject.c",
            "Objects/typeobject.c", "Objects/unicodectype.c", "Objects/unicodeobject.c", "Objects/unionobject.c",
            "Objects/weakrefobject.c"
            , "Objects/typevarobject.c"

            , "Parser/.*\\.[hc]"_rr, "Python/.*\\.h"_rr, "Python/Python-ast.c",
            "Python/_warnings.c", "Python/asdl.c", "Python/ast.c", "Python/ast_opt.c", "Python/ast_unparse.c",
            "Python/bltinmodule.c", "Python/bootstrap_hash.c", "Python/ceval.c", "Python/codecs.c", "Python/compile.c",
            "Python/context.c", "Python/dtoa.c", "Python/dynamic_annotations.c", "Python/dynload_.*"_rr,
            "Python/errors.c", "Python/fileutils.c", "Python/formatter_unicode.c", "Python/future.c",
            "Python/getargs.c", "Python/getcompiler.c", "Python/getcopyright.c", "Python/getopt.c",
            "Python/getplatform.c", "Python/getversion.c",
            //"Python/graminit.c",
            "Python/hamt.c", "Python/hashtable.c", "Python/initconfig.c", "Python/import.c", "Python/importdl.c",
            "Python/marshal.c", "Python/modsupport.c", "Python/mysnprintf.c", "Python/mystrtoul.c",
            "Python/pathconfig.c",
            //"Python/peephole.c",
            "Python/preconfig.c", "Python/pyarena.c", "Python/pyctype.c", "Python/pyfpe.c", "Python/pyhash.c",
            "Python/pylifecycle.c", "Python/pymath.c", "Python/pystate.c", "Python/pystrcmp.c", "Python/pystrhex.c",
            "Python/pystrtod.c", "Python/pythonrun.c", "Python/pytime.c", "Python/structmember.c",
            "Python/suggestions.c", "Python/symtable.c", "Python/sysmodule.c", "Python/thread.c", "Python/traceback.c"
            , "Python/ceval.c"
            , "Python/ceval_gil.c"
            , "Python/legacy_tracing.c"
            , "Python/frame.c"
            , "Python/traceback.c"
            , "Python/tracemalloc.c"
            , "Python/specialize.c"
            , "Python/instrumentation.c"
            , "Python/intrinsics.c"
            , "Python/flowgraph.c"
            , "Python/perf_trampoline.c"
            , "Python/assemble.c"
            , "Python/Python-tokenize.c"
            ;

        //lib -= "Modules/_sha3/kcp/.*"_rr;
        lib -= "Modules/_blake2/impl/.*"_rr;
        lib -= "Python/dynload_.*"_rr;

        if (lib.getBuildSettings().TargetOS.Type == OSType::Windows || lib.getBuildSettings().TargetOS.Type == OSType::Mingw) {
            lib += "Modules/_winapi.c", "PC/.*\\.h"_rr, "PC/config.c", "PC/dl_nt.c", "PC/python3dll.c",
                //"PC/getpathp.c",
                "PC/invalid_parameter_handler.c", "PC/msvcrtmodule.c", "PC/winreg.c";
            lib.Public += "PC"_id;
            lib += "WIN32"_def;

            lib.replaceInFileOnce("PC/pyconfig.h", "#ifdef MS_COREDLL", "#if 0");
        } else {
            lib += "PC/config.c", "Modules/_posixsubprocess.c", "Modules/selectmodule.c";

            lib.configureFile("pyconfig.h.in", "pyconfig.h", ConfigureFlags::EnableUndefReplacements);
        }
        lib.Public += "Include"_id;
        lib.Protected += "Include/internal"_id;

        lib.Private += "EXEC_PREFIX=\"\""_d;
        lib.Private += "PREFIX=\"\""_d;
        lib.Protected += "Py_BUILD_CORE"_d;
        lib.Private += "Py_BUILD_CORE_BUILTIN"_d;
        lib.Private += "VERSION=\"${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}\""_d;
        lib.Private += "VPATH=\"\""_d;
        lib.Private += Definition("PY3_DLLNAME=\"" + to_string(normalize_path(lib.getOutputFile().filename())) + "\"");
        lib.Public += "ABIFLAGS=\"\""_d;
        lib.Public += "ENABLE_IPV6"_d;
        lib.Public += "HAVE_DYNAMIC_LOADING"_d;
        lib.Public += "HAVE_GCC_UINT128_T=HAVE___UINT128_T"_d;
        lib.Public += "HAVE_STDARG_PROTOTYPES"_d;
        lib.Public += "HAVE_ZLIB_COPY"_d;
        //lib.Public += "PLATFORM=\"\""_d;
        lib.Public += "RETSIGTYPE=void"_d;
        lib.Public += "SOABI=\"\""_d;
        lib.Public += "STDC_HEADERS=1"_d;
        lib.Public += "USE_INLINE"_d;
        lib.Public += "WITH_DOC_STRINGS=1"_d;
        lib.Public += "WITH_PYMALLOC"_d;
        lib += "_Py_HAVE_ZLIB"_d;
        if (lib.getBuildSettings().TargetOS.Type == OSType::Windows || lib.getBuildSettings().TargetOS.Type == OSType::Mingw) {
            lib -= "Modules/pwdmodule.c";
            //lib -= "Modules/getpath.c";
            lib += "Python/dynload_win.c";

            lib.Private += "PYTHONPATH=L\"\""_d;
            lib.Private += "MS_DLL_ID=\"\""_d;
            //lib.Public += "MS_WINDOWS"_d;
            //lib.Public += "NT_THREADS"_d;

            lib.Public += "advapi32.lib"_slib;
            lib.Public += "Mincore.lib"_slib;
            lib.Public += "Shlwapi.lib"_slib;

            if (lib.getBuildSettings().TargetOS.Type == OSType::Mingw) {
                lib.patch("Modules/posixmodule.c", "#      define HAVE_FORK", "//#       define HAVE_FORK");
                lib.patch("Modules/posixmodule.c", "#    define HAVE_GETEGID", "//#     define HAVE_GETEGID");
                lib.patch("Modules/posixmodule.c", "#    define HAVE_GETEUID", "//#     define HAVE_GETEUID");
                lib.patch("Modules/posixmodule.c", "#    define HAVE_GETGID", "//#     define HAVE_GETGID");
                lib.patch("Modules/posixmodule.c", "#    define HAVE_GETUID", "//#     define HAVE_GETUID");
                lib.patch("Modules/posixmodule.c", "#    define HAVE_WAIT", "//#     define HAVE_WAIT");
                lib.patch("Modules/posixmodule.c", "#    define HAVE_TTYNAME", "//#     define HAVE_TTYNAME");
                lib.pushFrontToFileOnce("Modules/posixmodule.c", R"xxx(
#ifdef __MINGW32__
#define HAVE_DIRENT_H 1
#endif
                )xxx");
                lib.patch("Modules/posixmodule.c", "#include <stdio.h>", R"xxx(
#include  <stdio.h>
#  include "osdefs.h"
                )xxx");
                lib.Private += "MS_WINDOWS"_d;
                lib.Public += "Version.lib"_slib;
                lib.Protected.LinkOptions.push_back("-municode");
                if (lib.getBuildSettings().TargetOS.Arch == ArchType::x86_64)
                    lib.Private += "MS_WIN64"_d;
                else
                    lib.Private += "MS_WIN32"_d;
            }
        } else {
            lib += "Python/dynload_shlib.c";

            lib.Public += "PY_FORMAT_LONG_LONG=\"ll\""_d;
            lib.Public += "PY_FORMAT_SIZE_T=\"z\""_d;
            lib.Public += "WITH_THREAD"_d;
            lib.Public += "_ALL_SOURCE=1"_d;
            lib.Public += "_BSD_TYPES"_d;
            lib.Public += "_DARWIN_C_SOURCE"_d;
            lib.Public += "_FILE_OFFSET_BITS=64"_d;
            lib.Public += "_GNU_SOURCE=1"_d;
            lib.Public += "_LARGEFILE_SOURCE=1"_d;
            lib.Public += "_NETBSD_SOURCE"_d;
            lib.Public += "_POSIX_PTHREAD_SEMANTICS=1"_d;
            lib.Public += "_POSIX_THREADS"_d;
            lib.Public += "_TANDEM_SOURCE=1"_d;
            lib.Public += "__BSD_VISIBLE"_d;
            lib.Public += "__EXTENSIONS__=1"_d;

            lib.Private += "PYTHONPATH=\"\""_d;
            lib.Private += "_PYTHONFRAMEWORK=\"\""_d;
            lib.Private += "PLATLIBDIR=\"\""_d;

            lib += "pthread"_slib;
            lib += "dl"_slib;
        }
        if (lib.getBuildSettings().TargetOS.Type == OSType::Linux) {
            lib += "util"_slib;
        }
        // win does not work with this
        if (lib.getBuildSettings().TargetOS.Type != OSType::Windows) {
            lib.Public += sw::Static, "Py_NO_ENABLE_SHARED"_d;
        }

        lib.Public += "org.sw.demo.madler.zlib"_dep;
        lib.Public += "org.sw.demo.bzip2"_dep;
        lib.Public += "org.sw.demo.expat"_dep;
        lib.Public += "org.sw.demo.xz_utils.lzma"_dep;
        lib.Public -= "org.sw.demo.gnu.gettext.intl"_dep;
        if (lib.getBuildSettings().TargetOS.isApple())
            lib += "org.sw.demo.gnu.gettext.intl"_dep;

        lib -= ".*.def"_rr;

        lib.replaceInFileOnce("PC/config.c", "/* -- ADDMODULE MARKER 1 -- */", R"xxx(
            extern PyObject* PyInit_pyexpat(void);
            extern PyObject* PyInit__bz2(void);
            extern PyObject* PyInit__lzma(void);

	    extern PyObject* PyInit__locale(void);

            #ifndef _WIN32
                extern PyObject* PyInit_pwd(void);

		extern PyObject* PyInit_posix(void);
		extern PyObject* PyInit__posixsubprocess(void);
		extern PyObject* PyInit_select(void);
            #endif
)xxx");
        lib.replaceInFileOnce("PC/config.c", "/* -- ADDMODULE MARKER 2 -- */", R"xxx(
            {"pyexpat", PyInit_pyexpat},
            {"_bz2", PyInit__bz2},
            {"_lzma", PyInit__lzma},
            #ifndef _WIN32
                {"pwd", PyInit_pwd},
            #endif
)xxx");

        lib.replaceInFileOnce("PC/config.c", "{\"nt\", PyInit_nt},", R"xxx(
            #ifdef _WIN32
                { "nt", PyInit_nt},
            #else
                { "posix", PyInit_posix},
                { "_posixsubprocess", PyInit__posixsubprocess},
                { "select", PyInit_select},
                {"_locale", PyInit__locale},
            #endif
)xxx");

        lib.replaceInFileOnce("PC/config.c", "{\"_winapi\", PyInit__winapi},", R"xxx(
            #ifdef _WIN32
                { "_winapi", PyInit__winapi},
            #endif
)xxx");

        lib.replaceInFileOnce("PC/config.c", "{\"winreg\", PyInit_winreg},", R"xxx(
            #ifdef _WIN32
                { "winreg", PyInit_winreg},
            #endif
)xxx");

        lib.replaceInFileOnce("Include/internal/pycore_dtoa.h", "_PY_SHORT_FLOAT_REPR == ", "");

        lib += "Modules/_hacl/.*"_rr;
        lib += "Modules/_hacl/include"_idir;

        lib += "PREFIX=\".\""_def;
        lib += "EXEC_PREFIX=\".\""_def;
        lib += "VERSION=\".\""_def;
        lib += "VPATH=\".\""_def;
        lib += "PLATLIBDIR=\".\""_def;
    }
        return lib;
    };

    auto &bootstrap = python.addExecutable("freeze_module");
    {
        auto &t = bootstrap;
        python_lib(t);
        t += "Modules/getpath_noop.c";
        t += "Programs/_freeze_module.c";
    }

    auto &lib = python.addLibrary("lib");
    {
        python_lib(lib);
        lib += "Python/frozen.c";
        lib += "Modules/getpath.c";
        lib += "Tools/build/.*"_rr;

        auto freeze_modules1 = [&](String path, String name) {
            lib.addCommand()
                << cmd::prog(bootstrap)
                << name
                << cmd::in(path)
                << cmd::out("frozen_modules/" + name + ".h")
                ;
        };
        auto freeze_modules = [&](String name, String name2 = {}) {
            auto path = boost::replace_all_copy(name, ".", "/");
            freeze_modules1("Lib/" + path + ".py", name2.empty() ? name : name2);
        };
        freeze_modules("importlib._bootstrap");
        freeze_modules("importlib._bootstrap_external");
        freeze_modules("importlib.util");
        freeze_modules("importlib.machinery");
        freeze_modules("zipimport");
        freeze_modules("abc");
        freeze_modules("codecs");
        freeze_modules("io");
        freeze_modules("_collections_abc");
        freeze_modules("_sitebuiltins");
        freeze_modules("genericpath");
        freeze_modules("ntpath");
        freeze_modules("posixpath");
        freeze_modules("os");
        freeze_modules("site");
        freeze_modules("stat");
        freeze_modules("runpy");
        freeze_modules("__hello__");
        freeze_modules("__phello__.__init__", "__phello__");
        freeze_modules("__phello__.spam");
        freeze_modules("__phello__.ham.__init__", "__phello__.ham");
        freeze_modules("__phello__.ham.eggs");
        freeze_modules1("Tools/freeze/flag.py", "frozen_only");
        freeze_modules1("Modules/getpath.py", "getpath");
        lib.patch("Modules/getpath.c", "../Python/", "");

        {
            auto list = {
                "frozen_modules/importlib._bootstrap.h:importlib._bootstrap",
                "frozen_modules/importlib._bootstrap_external.h:importlib._bootstrap_external",
                "frozen_modules/zipimport.h:zipimport",
                "frozen_modules/abc.h:abc",
                "frozen_modules/codecs.h:codecs",
                "frozen_modules/io.h:io",
                "frozen_modules/_collections_abc.h:_collections_abc",
                "frozen_modules/_sitebuiltins.h:_sitebuiltins",
                "frozen_modules/genericpath.h:genericpath",
                "frozen_modules/ntpath.h:ntpath",
                "frozen_modules/posixpath.h:posixpath",
                "frozen_modules/os.h:os",
                "frozen_modules/site.h:site",
                "frozen_modules/stat.h:stat",
                "frozen_modules/importlib.util.h:importlib.util",
                "frozen_modules/importlib.machinery.h:importlib.machinery",
                "frozen_modules/runpy.h:runpy",
                "frozen_modules/__hello__.h:__hello__",
                "frozen_modules/__phello__.h:__phello__",
                "frozen_modules/__phello__.ham.h:__phello__.ham",
                "frozen_modules/__phello__.ham.eggs.h:__phello__.ham.eggs",
                "frozen_modules/__phello__.spam.h:__phello__.spam",
                "frozen_modules/frozen_only.h:frozen_only",
            };

            auto c = lib.addCommand()
                << cmd::wdir(lib.BinaryDir)
                << cmd::prog("org.sw.demo.python.exe-3.10"_dep)
                << cmd::in("Tools/build/deepfreeze.py")
                ;
            for (auto &&n : list) {
                c << n;
            }
            c
                << "-o" << cmd::out("deepfreeze.c")
                << cmd::end()
                ;
            for (auto &&n : list) {
                auto vec = split_string(n, ":");
                c << cmd::in(vec[0]);
            }
        }
    }

    auto &exe = python.addTarget<PythonExecutable>("exe");
    {
        exe -= "Lib/.*"_rr;
        exe -= ".*.def"_rr;
        exe += "Programs/python.c";
        exe.Public += lib;

        // https://bugs.python.org/issue45623
        exe.patch("Lib/site.py",
            "ver_nodot = sys.winver.replace('.', '')",
            "#ver_nodot = sys.winver.replace('.', '')"
        );
        exe.patch("Lib/site.py",
            "return f'{userbase}\\\\Python{ver_nodot}\\\\site-packages'",
            "return f'{userbase}/lib/python{version[0]}.{version[1]}/site-packages'"
        );

        exe.pushFrontToFileOnce("Lib/site.py", R"xxx(
# sw importer
import os
import subprocess
import sys
from importlib.util import spec_from_file_location

class SwImporter(object):
    def __init__(self):
        self.sw = os.environ['SW_EXECUTABLE']

    def run_subprocess(self, args):
        return subprocess.run(args, capture_output=True, text=True)

    def get_module_path(self, m):
        # first, try to get direct module if we have now absolute module paths
        #dir = subp([self.sw, 'path', module_name]).stdout
        p = self.run_subprocess([self.sw, 'path', 'org.sw.demo.python.pypi.' + m])
        if p.returncode != 0:
            return ''
        # need or just find __init__.py in find_spec() below?
        #if os.path.exists(p.stderr.strip() + '/src'):
            #return p.stderr.strip() + '/src'
        return p.stderr.strip()

    def find_spec(self, module_name, package_path, target=None):
        dir = self.get_module_path(module_name)
        if len(dir) == 0:
            return None
        fn = dir + '/' + module_name + '/__init__.py'
        if not os.path.exists(fn):
            fn = dir + '/src/' + module_name + '/__init__.py'
            if not os.path.exists(fn):
                return None
        return spec_from_file_location(module_name, fn)

# isn't python loads file only once and we do not need this check?
sw_importer_present = False
for obj in sys.meta_path:
    if type(obj) is SwImporter:
        sw_importer_present = True
        break
if not sw_importer_present:
    sys.meta_path.append(SwImporter())
)xxx");
    }
}

void check(Checker &c) {
    auto &s = c.addSet("lib");
    s.checkTypeAlignment("long");
    s.checkTypeAlignment("size_t");
    s.checkFunctionExists("accept4");
    s.checkFunctionExists("acosh");
    s.checkFunctionExists("alarm");
    s.checkFunctionExists("asinh");
    s.checkFunctionExists("atanh");
    s.checkFunctionExists("bind_textdomain_codeset");
    s.checkFunctionExists("chown");
    s.checkFunctionExists("chroot");
    s.checkFunctionExists("clock");
    s.checkFunctionExists("clock_getres");
    s.checkFunctionExists("clock_gettime");
    s.checkFunctionExists("confstr");
    s.checkFunctionExists("copysign");
    s.checkFunctionExists("ctermid");
    s.checkFunctionExists("ctermid_r");
    s.checkFunctionExists("dirfd");
    {
        auto &c = s.checkFunctionExists("dlopen");
        c.Parameters.Includes.push_back("dlfcn.h");
        c.Parameters.LinkOptions.push_back("-ldl");
    }
    s.checkFunctionExists("dup2");
    s.checkFunctionExists("dup3");
    s.checkFunctionExists("epoll");
    s.checkFunctionExists("epoll_create1");
    s.checkFunctionExists("erf");
    s.checkFunctionExists("erfc");
    s.checkFunctionExists("execv");
    s.checkFunctionExists("expm1");
    s.checkFunctionExists("faccessat");
    s.checkFunctionExists("fchdir");
    s.checkFunctionExists("fchmod");
    s.checkFunctionExists("fchmodat");
    s.checkFunctionExists("fchown");
    s.checkFunctionExists("fchownat");
    s.checkFunctionExists("fdopendir");
    s.checkFunctionExists("fexecve");
    s.checkFunctionExists("finite");
    s.checkFunctionExists("flock");
    s.checkFunctionExists("flockfile");
    s.checkFunctionExists("fork");
    s.checkFunctionExists("forkpty");
    s.checkFunctionExists("fpathconf");
    s.checkFunctionExists("fseek64");
    s.checkFunctionExists("fseeko");
    s.checkFunctionExists("fstatat");
    s.checkFunctionExists("fstatvfs");
    s.checkFunctionExists("fsync");
    s.checkFunctionExists("ftell64");
    s.checkFunctionExists("ftello");
    s.checkFunctionExists("ftime");
    s.checkFunctionExists("ftruncate");
    s.checkFunctionExists("funlockfile");
    s.checkFunctionExists("futimens");
    s.checkFunctionExists("futimes");
    s.checkFunctionExists("futimesat");
    s.checkFunctionExists("gai_strerror");
    s.checkFunctionExists("gamma");
    s.checkFunctionExists("getaddrinfo");
    s.checkFunctionExists("getc_unlocked");
    s.checkFunctionExists("getentropy");
    s.checkFunctionExists("getgrouplist");
    s.checkFunctionExists("getgroups");
    s.checkFunctionExists("gethostbyname");
    s.checkFunctionExists("gethostbyname_r");
    s.checkFunctionExists("getitimer");
    s.checkFunctionExists("getloadavg");
    s.checkFunctionExists("getlogin");
    s.checkFunctionExists("getnameinfo");
    s.checkFunctionExists("getpagesize");
    s.checkFunctionExists("getpeername");
    s.checkFunctionExists("getpgid");
    s.checkFunctionExists("getpgrp");
    s.checkFunctionExists("getpid");
    s.checkFunctionExists("getpriority");
    s.checkFunctionExists("getpwent");
    s.checkFunctionExists("getrandom");
    s.checkFunctionExists("getresgid");
    s.checkFunctionExists("getresuid");
    s.checkFunctionExists("getsid");
    s.checkFunctionExists("getspent");
    s.checkFunctionExists("getspnam");
    s.checkFunctionExists("gettimeofday");
    s.checkFunctionExists("getwd");
    s.checkFunctionExists("hstrerror");
    s.checkFunctionExists("htole64");
    s.checkFunctionExists("hypot");
    s.checkFunctionExists("if_nameindex");
    s.checkFunctionExists("inet_aton");
    s.checkFunctionExists("inet_pton");
    s.checkFunctionExists("initgroups");
    s.checkFunctionExists("isfinite");
    s.checkFunctionExists("isinf");
    s.checkFunctionExists("isnan");
    s.checkFunctionExists("is_term_resized");
    s.checkFunctionExists("kill");
    s.checkFunctionExists("killpg");
    s.checkFunctionExists("kqueue");
    s.checkFunctionExists("lchflags");
    s.checkFunctionExists("lchmod");
    s.checkFunctionExists("lchown");
    s.checkFunctionExists("le64toh");
    s.checkFunctionExists("lgamma");
    s.checkFunctionExists("link");
    s.checkFunctionExists("linkat");
    s.checkFunctionExists("lockf");
    s.checkFunctionExists("log1p");
    s.checkFunctionExists("log2");
    s.checkFunctionExists("lstat");
    s.checkFunctionExists("lutimes");
    s.checkFunctionExists("mbrtowc");
    s.checkFunctionExists("memmove");
    s.checkFunctionExists("memrchr");
    s.checkFunctionExists("mkdirat");
    s.checkFunctionExists("mkfifo");
    s.checkFunctionExists("mkfifoat");
    s.checkFunctionExists("mknod");
    s.checkFunctionExists("mknodat");
    s.checkFunctionExists("mktime");
    s.checkFunctionExists("mmap");
    s.checkFunctionExists("mremap");
    s.checkFunctionExists("nice");
    s.checkFunctionExists("openat");
    s.checkFunctionExists("openpty");
    s.checkFunctionExists("pathconf");
    s.checkFunctionExists("pause");
    s.checkFunctionExists("pipe2");
    s.checkFunctionExists("plock");
    s.checkFunctionExists("poll");
    s.checkFunctionExists("posix_fadvise");
    s.checkFunctionExists("posix_fallocate");
    s.checkFunctionExists("pread");
    s.checkFunctionExists("prlimit");
    s.checkFunctionExists("pthread_atfork");
    s.checkFunctionExists("pthread_init");
    s.checkFunctionExists("pthread_kill");
    s.checkFunctionExists("pthread_sigmask");
    s.checkFunctionExists("putenv");
    s.checkFunctionExists("pwrite");
    s.checkFunctionExists("readlink");
    s.checkFunctionExists("readlinkat");
    s.checkFunctionExists("readv");
    s.checkFunctionExists("realpath");
    s.checkFunctionExists("renameat");
    s.checkFunctionExists("resizeterm");
    s.checkFunctionExists("resize_term");
    s.checkFunctionExists("round");
    s.checkFunctionExists("sched_get_priority_max");
    s.checkFunctionExists("sched_rr_get_interval");
    s.checkFunctionExists("sched_setaffinity");
    s.checkFunctionExists("sched_setparam");
    s.checkFunctionExists("sched_setscheduler");
    s.checkFunctionExists("select");
    s.checkFunctionExists("sem_getvalue");
    s.checkFunctionExists("sem_open");
    s.checkFunctionExists("sem_timedwait");
    s.checkFunctionExists("sem_unlink");
    s.checkFunctionExists("sendfile");
    s.checkFunctionExists("setegid");
    s.checkFunctionExists("seteuid");
    s.checkFunctionExists("setgid");
    s.checkFunctionExists("setgroups");
    s.checkFunctionExists("sethostname");
    s.checkFunctionExists("setitimer");
    s.checkFunctionExists("setlocale");
    s.checkFunctionExists("setpgid");
    s.checkFunctionExists("setpgrp");
    s.checkFunctionExists("setpriority");
    s.checkFunctionExists("setregid");
    s.checkFunctionExists("setresgid");
    s.checkFunctionExists("setresuid");
    s.checkFunctionExists("setreuid");
    s.checkFunctionExists("setsid");
    s.checkFunctionExists("setuid");
    s.checkFunctionExists("setvbuf");
    s.checkFunctionExists("sigaction");
    s.checkFunctionExists("sigaltstack");
    s.checkFunctionExists("siginterrupt");
    s.checkFunctionExists("sigpending");
    s.checkFunctionExists("sigrelse");
    s.checkFunctionExists("sigtimedwait");
    s.checkFunctionExists("sigwait");
    s.checkFunctionExists("sigwaitinfo");
    s.checkFunctionExists("socketpair");
    s.checkFunctionExists("statvfs");
    s.checkFunctionExists("strdup");
    s.checkFunctionExists("strftime");
    s.checkFunctionExists("strlcpy");
    s.checkFunctionExists("symlink");
    s.checkFunctionExists("symlinkat");
    s.checkFunctionExists("sync");
    s.checkFunctionExists("sysconf");
    s.checkFunctionExists("tcgetpgrp");
    s.checkFunctionExists("tcsetpgrp");
    s.checkFunctionExists("tempnam");
    s.checkFunctionExists("tgamma");
    s.checkFunctionExists("timegm");
    s.checkFunctionExists("times");
    s.checkFunctionExists("tmpfile");
    s.checkFunctionExists("tmpnam");
    s.checkFunctionExists("tmpnam_r");
    s.checkFunctionExists("truncate");
    s.checkFunctionExists("tzname");
    s.checkFunctionExists("uname");
    s.checkFunctionExists("unlinkat");
    s.checkFunctionExists("unsetenv");
    s.checkFunctionExists("utimensat");
    s.checkFunctionExists("utimes");
    s.checkFunctionExists("wait3");
    s.checkFunctionExists("wait4");
    s.checkFunctionExists("waitid");
    s.checkFunctionExists("waitpid");
    s.checkFunctionExists("wcscoll");
    s.checkFunctionExists("wcsftime");
    s.checkFunctionExists("wcsxfrm");
    s.checkFunctionExists("wmemcmp");
    s.checkFunctionExists("writev");
    s.checkFunctionExists("_getpty");
    s.checkIncludeExists("alloca.h");
    s.checkIncludeExists("asm/types.h");
    s.checkIncludeExists("bluetooth/bluetooth.h");
    s.checkIncludeExists("bluetooth.h");
    s.checkIncludeExists("conio.h");
    s.checkIncludeExists("curses.h");
    s.checkIncludeExists("direct.h");
    s.checkIncludeExists("dirent.h");
    s.checkIncludeExists("dlfcn.h");
    s.checkIncludeExists("endian.h");
    s.checkIncludeExists("errno.h");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("grp.h");
    s.checkIncludeExists("ieeefp.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("io.h");
    s.checkIncludeExists("langinfo.h");
    s.checkIncludeExists("libintl.h");
    s.checkIncludeExists("libutil.h");
    s.checkIncludeExists("linux/can/bcm.h");
    s.checkIncludeExists("linux/can.h");
    s.checkIncludeExists("linux/can/raw.h");
    s.checkIncludeExists("linux/netlink.h");
    s.checkIncludeExists("linux/random.h");
    s.checkIncludeExists("linux/tipc.h");
    s.checkIncludeExists("memory.h");
    s.checkIncludeExists("ncurses.h");
    s.checkIncludeExists("ndir.h");
    s.checkIncludeExists("netpacket/packet.h");
    s.checkIncludeExists("net/if.h");
    s.checkIncludeExists("poll.h");
    s.checkIncludeExists("process.h");
    s.checkIncludeExists("pthread.h");
    s.checkIncludeExists("pty.h");
    s.checkIncludeExists("sched.h");
    s.checkIncludeExists("shadow.h");
    s.checkIncludeExists("signal.h");
    s.checkIncludeExists("spawn.h");
    s.checkIncludeExists("stdatomic.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("stropts.h");
    s.checkIncludeExists("sysexits.h");
    s.checkIncludeExists("sys/audioio.h");
    s.checkIncludeExists("sys/bsdtty.h");
    s.checkIncludeExists("sys/devpoll.h");
    s.checkIncludeExists("sys/dir.h");
    s.checkIncludeExists("sys/endian.h");
    s.checkIncludeExists("sys/epoll.h");
    s.checkIncludeExists("sys/event.h");
    s.checkIncludeExists("sys/file.h");
    s.checkIncludeExists("sys/ioctl.h");
    s.checkIncludeExists("sys/kern_control.h");
    s.checkIncludeExists("sys/loadavg.h");
    s.checkIncludeExists("sys/lock.h");
    s.checkIncludeExists("sys/mkdev.h");
    s.checkIncludeExists("sys/modem.h");
    s.checkIncludeExists("sys/ndir.h");
    s.checkIncludeExists("sys/param.h");
    s.checkIncludeExists("sys/poll.h");
    // https://www.mail-archive.com/bug-gnulib@gnu.org/msg38583.html
    s.checkIncludeExists("sys/random.h").Parameters.Includes.push_back("unistd.h");
    s.checkIncludeExists("sys/resource.h");
    s.checkIncludeExists("sys/select.h");
    s.checkIncludeExists("sys/sendfile.h");
    s.checkIncludeExists("sys/socket.h");
    s.checkIncludeExists("sys/statvfs.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/syscall.h");
    s.checkIncludeExists("sys/sysmacros.h");
    s.checkIncludeExists("sys/sys_domain.h");
    s.checkIncludeExists("sys/termio.h");
    s.checkIncludeExists("sys/times.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("sys/uio.h");
    s.checkIncludeExists("sys/un.h");
    s.checkIncludeExists("sys/utsname.h");
    s.checkIncludeExists("sys/wait.h");
    s.checkIncludeExists("sys/xattr.h");
    s.checkIncludeExists("termios.h");
    s.checkIncludeExists("term.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("util.h");
    s.checkIncludeExists("utime.h");
    s.checkIncludeExists("wchar.h");
    s.checkTypeSize("double");
    s.checkTypeSize("float");
    s.checkTypeSize("fpos_t");
    s.checkTypeSize("int");
    s.checkTypeSize("int32_t");
    s.checkTypeSize("int64_t");
    s.checkTypeSize("long");
    s.checkTypeSize("long double");
    s.checkTypeSize("long long");
    s.checkTypeSize("off_t");
    s.checkTypeSize("pid_t");
    s.checkTypeSize("pthread_t");
    s.checkTypeSize("short");
    s.checkTypeSize("size_t");
    s.checkTypeSize("ssize_t");
    s.checkTypeSize("time_t");
    s.checkTypeSize("uint32_t");
    s.checkTypeSize("uint64_t");
    s.checkTypeSize("uintptr_t");
    s.checkTypeSize("void *");
    s.checkTypeSize("wchar_t");
    s.checkTypeSize("_Bool");
    s.checkTypeSize("__uint128_t");
    {
        auto &c = s.checkSymbolExists("snprintf");
        c.Parameters.Includes.push_back("stdio.h");
    }
    s.checkSourceCompiles("GETPGRP_HAVE_ARG", R"sw_xxx(#include <unistd.h>
    int main()
    {
        getpgrp(0);
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("GETTIMEOFDAY_NO_TZ", R"sw_xxx(#include <sys/time.h>
    int main()
    {
        gettimeofday((struct timeval*)0,(struct timezone*)0);
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_ADDRINFO", R"sw_xxx(#include <netdb.h>
    int main()
    {
        struct addrinfo a;
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_ALTZONE", R"sw_xxx(#include <time.h>
    int main()
    {
        return altzone;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_BROKEN_UNSETENV", R"sw_xxx(#include <stdlib.h>
    int main()
    {
        int res = unsetenv("DUMMY");
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_BUILTIN_ATOMIC", R"sw_xxx(volatile int val = 1;
    int main() {
      __atomic_load_n(&val, __ATOMIC_SEQ_CST);
      return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_C99_BOOL", R"sw_xxx(int main()
    {
        _Bool x;
        x = (_Bool)0;
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_DEVICE_MACROS", R"sw_xxx(#if defined(MAJOR_IN_MKDEV)
    #include <sys/mkdev.h>
    #elif defined(MAJOR_IN_SYSMACROS)
    #include <sys/sysmacros.h>
    #else
    #include <sys/types.h>
    #endif
    int main()
    {
        makedev(major(0),minor(0));
        return 0;
    }
    #ifdef _WIN32
    cannot compile
    #endif
    )sw_xxx");
    s.checkSourceCompiles("HAVE_DIRENT_D_TYPE", R"sw_xxx(#include <dirent.h>

    int main() {
      struct dirent entry;
      return entry.d_type == DT_UNKNOWN;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_FDATASYNC", R"sw_xxx(#include <unistd.h>
    int main()
    {
        void *x=fdatasync;
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_GCC_ASM_FOR_MC68881", R"sw_xxx(int main()
    {  unsigned int fpcr;
      __asm__ __volatile__ ("fmove.l %%fpcr,%0" : "=g" (fpcr));
      __asm__ __volatile__ ("fmove.l %0,%%fpcr" : : "g" (fpcr));
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_GCC_ASM_FOR_X64", R"sw_xxx(int main()
    {
        __asm__ __volatile__ ("movq %rcx, %rax");
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_GCC_ASM_FOR_X87", R"sw_xxx(int main()
    {
          unsigned short cw;
      __asm__ __volatile__ ("fnstcw %0" : "=m" (cw));
      __asm__ __volatile__ ("fldcw %0" : : "m" (cw));
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_GETRANDOM_SYSCALL", R"sw_xxx(#include <unistd.h>
    #include <linux/random.h>
    #include <sys/syscall.h>

    int main() {
        char buffer[1];
        const size_t buflen = sizeof(buffer);
        const int flags = GRND_NONBLOCK;
        /* ignore the result, Python checks for ENOSYS and EAGAIN at runtime */
        (void)syscall(SYS_getrandom, buffer, buflen, flags);
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_LINUX_CAN_RAW_FD_FRAMES", R"sw_xxx(#include <linux/can/raw.h>
    int main()
    {
        int can_raw_fd_frames = CAN_RAW_FD_FRAMES;
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_MAKEDEV", R"sw_xxx(#if defined(MAJOR_IN_MKDEV)
    #include <sys/mkdev.h>
    #elif defined(MAJOR_IN_SYSMACROS)
    #include <sys/sysmacros.h>
    #else
    #include <sys/types.h>
    #endif
    int main()
    {
        makedev(0, 0);
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_OSX105_SDK", R"sw_xxx(#include <Carbon/Carbon.h>
    int main()
    {
        FSIORefNum fRef = 0;
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_PROTOTYPES", R"sw_xxx(int foo(int x) { return 0; }
    int main()
    {
        return foo(10);
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_SOCKADDR_SA_LEN", R"sw_xxx(#include <sys/types.h>
    #include <sys/socket.h>
    int main()
    {
        struct sockaddr x;
        x.sa_len = 0;
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_SOCKADDR_STORAGE", R"sw_xxx(#           include <sys/types.h>
    #           include <sys/socket.h>
    int main()
    {
        struct sockaddr_storage s;
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_STAT_TV_NSEC", R"sw_xxx(#include <sys/stat.h>
    int main()
    {
        struct stat st;
        st.st_mtim.tv_nsec = 1;
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_STAT_TV_NSEC2", R"sw_xxx(#include <sys/stat.h>
    int main()
    {
        struct stat st;
        st.st_mtimespec.tv_nsec = 1;
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_STDARG_PROTOTYPES", R"sw_xxx(#include <stdarg.h>
    int foo(int x, ...) {
    va_list va;
    va_start(va, x);
    va_arg(va, int);
    va_arg(va, char *);
    va_arg(va, double);
    return 0;
    }
    int main()
    {
        return foo(10, "", 3.14);
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_STD_ATOMIC", R"sw_xxx(#include <stdatomic.h>
    atomic_int value = ATOMIC_VAR_INIT(1);
    _Atomic void *py_atomic_address = (void*) &value;
    int main() {
      int loaded_value = atomic_load(&value);
      return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_UCS4_TCL", R"sw_xxx(#include <tcl.h>
    #if TCL_UTF_MAX != 6
    # error "NOT UCS4_TCL"
    #endif
    int main()
    {
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_WCHAR_T_SIGNED", R"sw_xxx(#include <wchar.h>
    int main()
    {
    /* Success: exit code 0 */
        exit((((wchar_t) -1) < ((wchar_t) 0)) ? 0 : 1);
    }
    )sw_xxx");
    s.checkSourceCompiles("MVWDELCH_IS_EXPRESSION", R"sw_xxx(#include <curses.h>
    int main()
    {
          int rtn;
        rtn = mvwdelch(0,0,0);
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("SETPGRP_HAVE_ARG", R"sw_xxx(#include <unistd.h>
    int main()
    {
        setpgrp(0,0);
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("SYS_SELECT_WITH_SYS_TIME", R"sw_xxx(#include <sys/types.h>
    #include <sys/select.h>
    #include <sys/time.h>
    int main()
    {
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("TIME_WITH_SYS_TIME", R"sw_xxx(#include <time.h>
    #include <sys/time.h>
    int main()
    {
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("TM_IN_SYS_TIME", R"sw_xxx(
    #include <time.h>
    int main() { struct tm t; return 0; }
    )sw_xxx");
    s.checkSourceCompiles("VA_LIST_IS_ARRAY", R"sw_xxx(#if ${HAVE_STDARG_PROTOTYPES}
    #include <stdarg.h>
    #else
    #include <varargs.h>
    #endif
    int main()
    {
        va_list list1, list2; list1 = list2;
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("WINDOW_HAS_FLAGS", R"sw_xxx(#include <curses.h>
    int main()
    {
        WINDOW *w;
        w->_flags = 0;
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_CHFLAGS", R"sw_xxx(
    #include <sys/stat.h>
    #include <unistd.h>
    int main(int argc, char *argv[])
    {
      if(chflags(argv[0], 0) != 0)
        return 1;
      return 0;
    }
    )sw_xxx");
}
