void build(Solution &s)
{
    auto &t = s.addStaticLibrary("gnome.gvdb", "main");
    t += Git("https://gitlab.gnome.org/GNOME/gvdb");
    t += "gvdb/.*"_r;
    // this does not create gobject/gobject-visibility.h during configure (it is created during build)
    //(t + "org.sw.demo.gnome.glib.gobject"_dep)->IncludeDirectoriesOnly = true;
    (t + "org.sw.demo.gnome.glib.gio"_dep)->IncludeDirectoriesOnly = true;
}
