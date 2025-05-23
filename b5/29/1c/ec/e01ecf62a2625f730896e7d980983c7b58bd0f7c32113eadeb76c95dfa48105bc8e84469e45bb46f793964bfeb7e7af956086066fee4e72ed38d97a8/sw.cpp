void build(Solution &s)
{
    auto &cairo = s.addTarget<LibraryTarget>("cairographics.cairo", "1.18.4");
    cairo += Git("https://gitlab.freedesktop.org/cairo/cairo", "{v}");
    {
        cairo.setChecks("cairo", true);

        //if (cairo.getBuildSettings().TargetOS.Type == OSType::Windows)
            //cairo.ApiNames.insert("cairo_public");

        cairo += cpp11;

        cairo +=
            "src/cairo.*"_rr,
            "src/win32/.*"_rr
            ;

        cairo -=
            //"src/cairo-egl-context.c",
            //"src/cairo-gl.*"_rr,
            //"src/cairo-glx-context.c",
            "src/cairo-image-mask-compositor.c",
            "src/cairo-quartz-font.c",
            "src/cairo-quartz-image-surface.c",
            "src/cairo-quartz-surface.c",
            "src/cairo-tee-surface.c",
            //"src/cairo-wgl.*"_rr,
            "src/cairo-xcb.*"_rr,
            "src/cairo-xlib.*"_rr,
            "src/win32/.*"_rr
            ;

        cairo.Public +=
            "src"_id;

        cairo += "CAIRO_COMPILATION"_def;
        cairo.Public += sw::Static, "CAIRO_WIN32_STATIC_BUILD"_def;

        cairo.Public += "CAIRO_FEATURES_H=1"_d;
        cairo.Public += "CAIRO_HAS_DRM_SURFACE=0"_d;
        cairo.Public += "CAIRO_HAS_EGL_FUNCTIONS=0"_d;
        cairo.Public += "CAIRO_HAS_FC_FONT=1"_d;
        cairo.Public += "CAIRO_HAS_FT_FONT=1"_d;
        cairo.Public += "CAIRO_HAS_GALLIUM_SURFACE=0"_d;
        cairo.Public += "CAIRO_HAS_GLESV2_SURFACE=0"_d;
        cairo.Public += "CAIRO_HAS_GLX_FUNCTIONS=0"_d;
        cairo.Public += "CAIRO_HAS_GL_SURFACE=0"_d;
        cairo.Public += "CAIRO_HAS_GOBJECT_FUNCTIONS=0"_d;
        cairo.Public += "CAIRO_HAS_INTERPRETER=1"_d;
        cairo.Public += "CAIRO_HAS_OS2_SURFACE=0"_d;
        cairo.Public += "CAIRO_HAS_PDF_SURFACE=1"_d;
        cairo.Public += "CAIRO_HAS_PNG_FUNCTIONS=1"_d;
        cairo.Public += "CAIRO_HAS_PS_SURFACE=1"_d;
        cairo.Public += "CAIRO_HAS_SCRIPT_SURFACE=1"_d;
        cairo.Public += "CAIRO_HAS_SKIA_SURFACE=0"_d;
        cairo.Public += "CAIRO_HAS_SVG_SURFACE=1"_d;
        cairo.Public += "CAIRO_HAS_SYMBOL_LOOKUP=0"_d;
        cairo.Public += "CAIRO_HAS_TEE_SURFACE=0"_d;
        cairo.Public += "CAIRO_HAS_TEST_SURFACES=0"_d;
        cairo.Public += "CAIRO_HAS_TRACE=0"_d;
        cairo.Public += "CAIRO_HAS_WGL_FUNCTIONS=0"_d;
        cairo.Public += "CAIRO_HAS_XCB_SHM_FUNCTIONS=0"_d;
        cairo.Public += "CAIRO_HAS_XCB_SURFACE=0"_d;
        cairo.Public += "CAIRO_HAS_XLIB_SURFACE=0"_d;
        cairo.Public += "CAIRO_HAS_XLIB_XCB_FUNCTIONS=0"_d;
        cairo.Public += "CAIRO_HAS_XLIB_XRENDER_SURFACE=0"_d;
        cairo.Public += "CAIRO_HAS_XML_SURFACE=0"_d;

        // without this clang-cl does not work - requires some builtin
        cairo += "HAVE_UINT128_T=0"_d;

        if (cairo.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            cairo.Public += "CAIRO_HAS_WIN32_FONT=1"_d;
            cairo.Public += "CAIRO_HAS_WIN32_SURFACE=1"_d;
            cairo.Public += "CAIRO_HAS_DWRITE_FONT=1"_d;
            cairo.Public += "HAVE_DWRITE_3_H=1"_d;
            cairo.Public += "HAVE_D2D1_3_H=1"_d;
        }
        else
        {
            cairo.Public += "CAIRO_HAS_PTHREAD"_d;
        }

        if (cairo.getBuildSettings().TargetOS.Type == OSType::Macos)
        {
#if SW_MODULE_ABI_VERSION > 28
            cairo.add("src/cairo-quartz-font.c", ".m");
            cairo.add("src/cairo-quartz-image-surface.c", ".m");
            cairo.add("src/cairo-quartz-surface.c", ".m");
#endif

            cairo.Public += "CAIRO_HAS_QUARTZ_SURFACE"_def;
            cairo.Public += "CAIRO_HAS_QUARTZ_IMAGE_SURFACE"_def;
            cairo.Public += "CAIRO_HAS_QUARTZ_FONT"_def;

            cairo += "CoreFoundation"_framework;
            cairo += "CoreGraphics"_framework;
            cairo += "CoreText"_framework;
            cairo += "ImageIO"_framework;
        }
        else
        {
            cairo.Public += "CAIRO_HAS_QUARTZ_FONT=0"_d;
            cairo.Public += "CAIRO_HAS_QUARTZ_IMAGE_SURFACE=0"_d;
            cairo.Public += "CAIRO_HAS_QUARTZ_SURFACE=0"_d;
        }

        //cairo.Public += "HAVE_CXX11_ATOMIC_PRIMITIVES=1"_d;

        cairo.Public += "org.sw.demo.freedesktop.fontconfig.fontconfig"_dep;
        cairo.Public += "org.sw.demo.freedesktop.pixman"_dep;
        cairo.Public += "org.sw.demo.freetype"_dep;
        cairo.Public += "org.sw.demo.madler.zlib"_dep;
        cairo.Public += "org.sw.demo.glennrp.png"_dep;

        cairo.writeFileOnce("cairo-features.h");
        cairo.deleteInFileOnce("src/cairo-compiler-private.h", "#define access _access");
        cairo.deleteInFileOnce("src/cairo-compiler-private.h", "#define vsnprintf _vsnprintf");
        cairo.writeFileOnce(cairo.BinaryPrivateDir / "config.h");
        cairo.pushFrontToFileOnce("src/cairo-ft-font.c", "#include <wchar.h>");

        if (cairo.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            cairo += "src/win32/.*"_rr;
            cairo.Public +=
                "Gdi32.lib"_slib,
                "User32.lib"_slib,
                "Msimg32.lib"_slib,
                "advapi32.lib"_slib,
                "ole32.lib"_slib,
                "uuid.lib"_slib
                ;
        }
        else
        {
            //cairo.ExportAllSymbols = true;
            cairo += "pthread"_slib;
        }

        // special library to init gobject/glib on windows with static linking
        auto &wi = cairo.addStaticLibrary("wininit");
        {
            wi.AutoDetectOptions = false;
            wi.WholeArchive = true;
            wi.writeFileOnce("cairo_init.cpp", R"(
extern "C" void _cairo_mutex_initialize (void);
extern "C" void _cairo_mutex_finalize (void);
struct SW_CAIRO_INITIALIZER { SW_CAIRO_INITIALIZER() { _cairo_mutex_initialize(); } ~SW_CAIRO_INITIALIZER() { _cairo_mutex_finalize(); } };
static SW_CAIRO_INITIALIZER ___________SW_CAIRO_INITIALIZER;
)");
            wi += "cairo_init.cpp";
        }
        cairo -= wi;
        if (cairo.getBuildSettings().TargetOS.Type == OSType::Windows
            && cairo.getBuildSettings().Native.LibrariesType == LibraryType::Static
            && (cairo.getCompilerType() == CompilerType::MSVC || cairo.getCompilerType() == CompilerType::ClangCl)
            )
        {
            cairo += wi;
        }
    }

    auto &cg = cairo.addTarget<LibraryTarget>("gobject");
    {
        if (cg.getBuildSettings().TargetOS.Type != OSType::Windows)
            cg.ExportAllSymbols = true;
        cg += "util/cairo-gobject/.*\\.[hc]"_rr;
        cg.Public += "CAIRO_HAS_GOBJECT_FUNCTIONS"_def;
        cg.Public += "util/cairo-gobject"_id;
        cg.Public += cairo;
        cg.Public += "org.sw.demo.gnome.glib.gobject"_dep;
        cg.writeFileOnce(cg.BinaryPrivateDir / "config.h");
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("cairo");
    s.checkFunctionExists("accept");
    s.checkFunctionExists("ctime_r");
    s.checkFunctionExists("dlsym");
    s.checkFunctionExists("XRenderCreateConicalGradient");
    s.checkFunctionExists("XRenderCreateLinearGradient");
    s.checkFunctionExists("XRenderCreateRadialGradient");
    s.checkFunctionExists("XRenderCreateSolidFill");
    s.checkIncludeExists("byteswap.h");
    s.checkIncludeExists("dlfcn.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("io.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("sys/int_types.h");
    s.checkIncludeExists("sys/ioctl.h");
    s.checkIncludeExists("sys/ipc.h");
    s.checkIncludeExists("sys/shm.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("wchar.h");
    s.checkIncludeExists("X11/extensions/shmproto.h");
    s.checkIncludeExists("X11/extensions/shmstr.h");
    s.checkIncludeExists("X11/extensions/XShm.h");
    s.checkTypeSize("int");
    s.checkTypeSize("long");
    s.checkTypeSize("long long");
    s.checkTypeSize("size_t");
    //s.checkTypeSize("uint128_t");
    //s.checkTypeSize("__uint128_t");
    s.checkTypeSize("uint64_t");
    s.checkTypeSize("void *");
    s.checkLibraryFunctionExists("bfd", "bfd_openr");
    s.checkLibraryFunctionExists("csi", "csi_stream_attachresource");
    s.checkLibraryFunctionExists("dl", "dlsym");
    s.checkLibraryFunctionExists("be", "main");
    s.checkLibraryFunctionExists("rt", "shm_open");
}
