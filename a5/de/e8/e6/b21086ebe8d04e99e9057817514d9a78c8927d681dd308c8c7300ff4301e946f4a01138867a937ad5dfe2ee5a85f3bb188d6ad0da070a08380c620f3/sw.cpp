void build(Solution &s)
{
    auto &p = s.addProject("gnome.gtk", "3.94.0");
    p += RemoteFile("https://ftp.gnome.org/pub/GNOME/sources/gtk+/{M}.{m}/gtk+-{v}.tar.xz");

    auto &gdk = p.addTarget<StaticLibrary>("gdk");
    {
        gdk -= "gdk/.*"_rr;
        gdk.Public += "gdk"_id;
        gdk.Public += "."_id;
        gdk.Public += IncludeDirectory(gdk.BinaryDir / "gdk");

        gdk += "gdk/.*\\.[hc]"_r;

        gdk += "GDK_COMPILATION"_def;

        gdk.Public += "HAVE_DECL_ISNAN"_def;
        gdk.Public += "HAVE_DECL_ISINF"_def;

        if (gdk.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            gdk.Variables["GDK_WINDOWING_WIN32"] = 1;
        }
        else
        {
            gdk.Variables["GDK_WINDOWING_X11"] = 1;
            //gdk.Variables["GDK_WINDOWING_BROADWAY"] = 0;
            //gdk.Variables["GDK_WINDOWING_WAYLAND"] = 0;
            //gdk.Variables["GDK_WINDOWING_MIR"] = 0;
        }

        //gdk.Variables["GDK_RENDERING_VULKAN"] = 0;
        //gdk -= "gdk/.*vulkan.*\\.[hc]"_r;
        //gdk.Public += "org.sw.demo.khronos.vulkan"_dep;

        gdk.configureFile("gdk/gdkconfig.h.meson", "gdk/gdkconfig.h");

        gdk.writeFileOnce(gdk.BinaryPrivateDir / "config.h");

        gdk.Variables["GTK_MAJOR_VERSION"] = gdk.Variables["PACKAGE_VERSION_MAJOR"];
        gdk.Variables["GTK_MINOR_VERSION"] = gdk.Variables["PACKAGE_VERSION_MINOR"];
        gdk.Variables["GTK_MICRO_VERSION"] = gdk.Variables["PACKAGE_VERSION_PATCH"];
        gdk.configureFile("gdk/gdkversionmacros.h.in", "gdk/gdkversionmacros.h");

        gdk.Public += "org.sw.demo.cairographics.cairo.gobject"_dep;
        gdk.Public += "org.sw.demo.gnome.glib.gio"_dep;
        gdk.Public += "org.sw.demo.gnome.gtk.gdk.pixbuf"_dep;
        gdk.Public += "org.sw.demo.gnome.pango.pangocairo"_dep;
        gdk.Public += "org.sw.demo.anholt.libepoxy"_dep;
        gdk.Public += "org.sw.demo.ebassi.graphene"_dep;

        for (auto ext : { "h", "c" })
        {
            // glib.mkenums
            auto c = gdk.addCommand();
            c << cmd::prog("org.sw.demo.python.exe-3"_dep)
                << gdk.getFile("org.sw.demo.gnome.glib.gobject-2"_dep, "gobject/glib-mkenums.in")
                << "--template"
                << cmd::in("gdk/gdkenumtypes."s + ext + ".template")
                << "--output"
                << cmd::out("gdk/gdkenumtypes."s + ext)
                ;
            for (auto f : {
                "gdk-autocleanup.h",
                "gdk.h",
                "gdkapplaunchcontext.h",
                "gdkcairo.h",
                "gdkcairocontext.h",
                "gdkclipboard.h",
                "gdkcontentdeserializer.h",
                "gdkcontentformats.h",
                "gdkcontentprovider.h",
                "gdkcontentproviderimpl.h",
                "gdkcontentserializer.h",
                "gdkcursor.h",
                "gdkdevice.h",
                "gdkdevicepad.h",
                "gdkdevicetool.h",
                "gdkdisplay.h",
                "gdkdisplaymanager.h",
                "gdkdnd.h",
                "gdkdrawcontext.h",
                "gdkdrop.h",
                "gdkevents.h",
                "gdkframeclock.h",
                "gdkframetimings.h",
                "gdkglcontext.h",
                "gdkgltexture.h",
                "gdkkeys.h",
                "gdkkeysyms.h",
                "gdkmemorytexture.h",
                "gdkmonitor.h",
                "gdkpaintable.h",
                "gdkpango.h",
                "gdkpixbuf.h",
                "gdkproperty.h",
                "gdkrectangle.h",
                "gdkrgba.h",
                "gdkseat.h",
                "gdksnapshot.h",
                "gdktexture.h",
                "gdktypes.h",
                "gdkvulkancontext.h",
                "gdksurface.h",
                })
            {
                c << cmd::in("gdk/"s + f);
            }
        }

        for (auto ext : { "h"s, "c"s })
        {
            // glib.genmarshal
            auto c = gdk.addCommand();
            c << cmd::prog("org.sw.demo.python.exe-3"_dep)
                << gdk.getFile("org.sw.demo.gnome.glib.gobject-2"_dep, "gobject/glib-genmarshal.in")
                << "--prefix" << "_gdk_marshal"
                << "--output" << cmd::out("gdkmarshalers." + ext)
                << (ext == "h" ? "--header" : "--body")
                << cmd::in("gdk/gdkmarshalers.list")
                ;
        }

        {
            // resources
            auto c = gdk.addCommand();
            c << cmd::prog("org.sw.demo.python.exe-3"_dep)
                << cmd::in("gdk/gen-gdk-gresources-xml.py")
                << gdk.SourceDir / "gdk"
                << cmd::out("gdk.gresources.xml")
                ;
        }

        for (auto ext : { "h"s, "c"s })
        {
            auto c = gdk.addCommand();
            c << cmd::prog("org.sw.demo.gnome.glib.compile_resources"_dep)
                << cmd::wdir(gdk.SourceDir / "gdk")
                << cmd::in("gdk.gresources.xml")
                << "--c-name" << "_gdk"
                << "--manual-register"
                << "--generate"
                << "--internal"
                << "--target"
                << cmd::out("gdkresources." + ext)
                ;
        }
    }

    auto &w32 = gdk.addTarget<StaticLibrary>("win32");
    {
        w32 += "GDK_COMPILATION"_def;
        w32 += "INSIDE_GDK_WIN32"_def;
        w32 += "_USE_MATH_DEFINES"_def;
        w32 += "gdk/win32/.*"_rr;
        w32 += "gdk/win32"_id;
        w32 += "Winmm.lib"_slib;
        w32.Public += gdk;
        w32.writeFileOnce(w32.BinaryPrivateDir / "config.h");
    }

    auto &gsk = p.addTarget<StaticLibrary>("gsk");
    {
        gsk -= "gsk/.*"_rr;
        gsk.Public += "gsk"_id;
        gsk.Public += IncludeDirectory(gsk.BinaryDir / "gsk");

        gsk += "gsk/.*\\.[hc]"_r;
        gsk += "gsk/gl/.*\\.[hc]"_r;

        gsk += "GSK_COMPILATION"_def;

        gsk.writeFileOnce(gsk.BinaryPrivateDir / "config.h");

        gsk.Public += gdk;

        for (auto ext : {"h", "c"})
        {
            // glib.mkenums
            auto c = gsk.addCommand();
            c << cmd::prog("org.sw.demo.python.exe-3"_dep)
                << gsk.getFile("org.sw.demo.gnome.glib.gobject-2"_dep, "gobject/glib-mkenums.in") << "--template"
                << cmd::in("gsk/gskenumtypes."s + ext + ".template") << "--output"
                << cmd::out("gsk/gskenumtypes."s + ext);
            for (auto f : {
                "gskenums.h", "gskrenderer.h", "gskrendernode.h", "gskroundedrect.h", "gsktypes.h",
                "gsk-autocleanup.h"
                })
            {
                c << cmd::in("gsk/"s + f);
            }
        }

        {
            // resources
            auto c = gsk.addCommand();
            c << cmd::prog("org.sw.demo.python.exe-3"_dep)
                << cmd::in("gsk/gen-gsk-gresources-xml.py")
                << cmd::out("gsk.gresources.xml")
                ;
            // not working
            //for (auto &[p, sf] : gsk["gsk/resources/.*\\.(glsl|spv|frag|vert)"_rr])
                //c << cmd::in(p);
            c.c->use_response_files = false;
        }

        for (auto ext : { "h"s, "c"s })
        {
            auto c = gsk.addCommand();
            c << cmd::prog("org.sw.demo.gnome.glib.compile_resources"_dep)
                << cmd::wdir(gsk.SourceDir / "gsk")
                << cmd::in("gsk.gresources.xml")
                << "--c-name" << "_gsk"
                << "--manual-register"
                << "--generate"
                << "--internal"
                << "--target"
                << cmd::out("gskresources." + ext)
                ;
        }
    }

    auto &gtk = p.addTarget<Library>("gtk");
    {
        gtk.ExportAllSymbols = true;

        gtk -= "gtk/.*"_rr;
        gtk.Public += "gtk"_id;
        gtk.Public += IncludeDirectory(gtk.BinaryDir / "gtk");

        gtk += "gtk/.*\\.[hc]"_r;

        gtk -= "gtk/.*quartz.*"_rr;
        gtk -= "gtk/.*wayland.*"_rr;
        gtk -= "gtk/.*x11.*"_rr;
        gtk -= "gtk/.*unix.*"_rr;
        gtk -= "gtk/.*broadway.*"_rr;
        gtk -= "gtk/.*-portal.*"_rr;

        // tools
        gtk -= "gtk/gtk-launch.c";
        gtk -= "gtk/gtk-builder-tool.c";
        gtk -= "gtk/encodesymbolic.c";
        gtk -= "gtk/gen-paper-names.c";
        gtk -= "gtk/updateiconcache.c";
        gtk -= "gtk/gtk-query-settings.c";

        gtk -= "gtk/paper_names.c";
        gtk -= "gtk/language-names.c";
        gtk -= "gtk/gtkimcontextxim.c"; // x11

        gtk += "GTK_COMPILATION"_def;
        gtk += "GTK_PRINT_BACKEND_ENABLE_UNSUPPORTED"_def;
        gtk += "GTK_PRINT_BACKENDS=\"file,lpr\""_def;

        gtk += "GTK_HOST=\"\""_def;
        gtk += "GTK_BINARY_VERSION=\"\""_def;
        gtk += "GTK_VERSION=\"\""_def;

        gtk += "_USE_MATH_DEFINES"_def;

        gtk += "GTK_LIBDIR=\"\""_def;
        gtk += "GTK_LOCALEDIR=\"\""_def;

        gtk.writeFileOnce(gtk.BinaryPrivateDir / "config.h", R"(
#ifdef _WIN32

#if !S_IRUSR && S_IREAD
# define S_IRUSR S_IREAD
#endif
#if !S_IRUSR
# define S_IRUSR 00400
#endif
#if !S_IWUSR && S_IWRITE
# define S_IWUSR S_IWRITE
#endif
#if !S_IWUSR
# define S_IWUSR 00200
#endif

#ifndef O_CLOEXEC
#define O_CLOEXEC 0
#endif

#define fchmod(x,y)

#endif
)");
        if (gtk.getBuildSettings().TargetOS.Type == OSType::Windows)
            gtk += "fchmod="_def;

        gtk.Public += gsk;
        gtk.Public += "org.sw.demo.gnome.atk"_dep;

        gtk.Variables["GTK_MAJOR_VERSION"] = gtk.Variables["PACKAGE_VERSION_MAJOR"];
        gtk.Variables["GTK_MINOR_VERSION"] = gtk.Variables["PACKAGE_VERSION_MINOR"];
        gtk.Variables["GTK_MICRO_VERSION"] = gtk.Variables["PACKAGE_VERSION_PATCH"];
        gtk.Variables["GTK_BINARY_AGE"] = 2;
        gtk.Variables["GTK_INTERFACE_AGE"] = 2;
        gtk.configureFile("gtk/gtkversion.h.in", "gtk/gtkversion.h");

        // for mkenums
        gtk -=
            "gtk/gtkcolorscaleprivate.h",
            "gtk/gtkcssimageradialprivate.h",
            "gtk/gtkcssrepeatvalueprivate.h",
            "gtk/gtkfilechooserutils.h",
            "gtk/gtkfontchooserutils.h",
            "gtk/gtkmenuprivate.h",
            "gtk/gtkmenushellprivate.h",
            "gtk/gtkmenutrackeritem.h",
            "gtk/gtkplacessidebarprivate.h",
            "gtk/gtkprintbackend.h",
            "gtk/gtkprinter.h",
            "gtk/gtkprinteroption.h",
            "gtk/gtkprintoperation-private.h",
            "gtk/gtkprogresstrackerprivate.h",
            "gtk/gtkplacessidebarprivate.h",
            "gtk/gtkrbtreeprivate.h",
            "gtk/gtksettingsprivate.h",
            "gtk/gtktreeprivate.h",
            "gtk/xembed.h"
            ;

        for (auto ext : {"h", "c"})
        {
            // glib.mkenums
            auto c = gtk.addCommand();
            c << cmd::prog("org.sw.demo.python.exe-3"_dep)
                << gtk.getFile("org.sw.demo.gnome.glib.gobject-2"_dep, "gobject/glib-mkenums.in") << "--template"
                << cmd::in("gtk/gtktypebuiltins."s + ext + ".template") << "--output"
                << cmd::out("gtk/gtktypebuiltins."s + ext);
            for (auto &[p, f] : gtk[".*\\.h"_rr])
            {
                if (!f->skip)
                    c << cmd::in(p);
            }
            c.c->first_response_file_argument = 1;
        }

        for (auto ext : {"h", "c"})
        {
            // glib.mkenums
            auto c = gtk.addCommand();
            c << cmd::prog("org.sw.demo.python.exe-3"_dep)
                << gtk.getFile("org.sw.demo.gnome.glib.gobject-2"_dep, "gobject/glib-mkenums.in") << "--template"
                << cmd::in("gtk/gtkprivatetypebuiltins."s + ext + ".template") << "--output"
                << cmd::out("gtk/gtkprivatetypebuiltins."s + ext);
            for (auto &f : {"gtkcsstypesprivate.h", "gtktexthandleprivate.h", "gtkeventcontrollerlegacyprivate.h"})
            {
                c << cmd::in("gtk/"s + f);
            }
        }

        for (auto ext : {"h"s, "c"s})
        {
            // glib.genmarshal
            auto c = gtk.addCommand();
            c << cmd::prog("org.sw.demo.python.exe-3"_dep)
                << gtk.getFile("org.sw.demo.gnome.glib.gobject-2"_dep, "gobject/glib-genmarshal.in")
                << "--prefix" << "_gtk_marshal"
                << "--valist-marshallers"
                << "--output" << cmd::out("gtkmarshalers." + ext) << (ext == "h" ? "--header" : "--body")
                << cmd::in("gtk/gtkmarshalers.list");
        }

        {
            auto c = gtk.addCommand();
            c << cmd::prog("org.sw.demo.python.exe-3"_dep)
                << gtk.getFile("org.sw.demo.gnome.glib.gio"_dep, "gio/gdbus-2.0/codegen/gdbus-codegen.in")
                << "--interface-prefix" << "org.Gtk."
                << "--output-directory" << gtk.BinaryDir
                << "--generate-c-code" << "gtkdbusgenerated"
                << "--c-namespace" << "_Gtk"
                << cmd::in("gtk/gtkdbusinterfaces.xml")
                << cmd::end()
                << cmd::out("gtkdbusgenerated.h")
                << cmd::out("gtkdbusgenerated.c");
        }

        gtk -= "gtk/gtkapplicationprivate.h";
        gtk -= "gtk/gtkimcontextxim.h";

        {
            gtk.patch("gtk/gentypefuncs.py", "in_files = sys.argv[2:]", R"(in_files = sys.argv[2:]
if in_files[0][0] == '@':
    in_files = [line.strip(" \n\r\"") for line in open(in_files[0][1:])]
)");

            auto c = gtk.addCommand();
            c << cmd::prog("org.sw.demo.python.exe-3"_dep)
                << cmd::in("gtk/gentypefuncs.py")
                << cmd::out("gtktypefuncs.inc");
            /*for (auto &[p, f] : gdk[".*\\.h"_rr])
            {
            if (!f->skip)
            c << cmd::in(p);
            }*/
            for (auto &[p, f] : gtk[".*\\.h"_rr])
            {
                if (!f->skip)
                    c << cmd::in(p);
            }
            c.c->first_response_file_argument = 2;
        }

        gtk -= w32;
        if (gtk.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            gtk += w32;

            gtk += "gdi32.lib"_slib;
            gtk += "Imm32.lib"_slib;
            gtk += "Winspool.lib"_slib;
            gtk += "Comdlg32.lib"_slib;
            gtk += "Comctl32.lib"_slib;
            gtk += "Dwmapi.lib"_slib;
            gtk += "Setupapi.lib"_slib;
            gtk += "uuid.lib"_slib;
        }

        gtk += "gtk/a11y/.*\\.[hc]"_r;
        gtk += "gtk/inspector/.*\\.[hc]"_r;

        {
            auto c = gtk.addCommand();
            c << cmd::prog("org.sw.demo.python.exe-3"_dep)
                << cmd::in("gtk/gen-gtk-gresources-xml.py")
                << (gtk.SourceDir / "gtk")
                << cmd::std_out("gtk.gresources.xml")
                ;
        }

        {
            auto c = gtk.addCommand();
            c << cmd::prog("org.sw.demo.gnome.glib.compile_resources"_dep)
                << cmd::in("gtk.gresources.xml")
                << "--c-name" << "_gtk"
                << "--sourcedir" << (gtk.SourceDir / "gtk")
                << "--manual-register"
                << "--internal"
                << "--generate"
                << "--target"
                << cmd::out("gtkresources.c")
                ;
        }

        {
            // resources
            auto c = gtk.addCommand();
            c << cmd::prog("org.sw.demo.python.exe-3"_dep)
                << cmd::in("gtk/gen-gtk-gresources-xml.py")
                << gtk.SourceDir / "gtk"
                << cmd::out("gtk.gresources.xml")
                ;
        }

        for (auto ext : { "h"s, "c"s })
        {
            auto c = gtk.addCommand();
            c << cmd::prog("org.sw.demo.gnome.glib.compile_resources"_dep)
                << cmd::wdir(gtk.SourceDir / "gtk")
                << cmd::in("gtk.gresources.xml")
                << "--c-name" << "_gtk"
                << "--manual-register"
                << "--generate"
                << "--internal"
                << "--target"
                << cmd::out("gtkresources." + ext)
                ;
        }
    }
}
