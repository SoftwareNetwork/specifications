// TODO: https://gitlab.gnome.org/GNOME/gtkmm

void build(Solution &s)
{
    auto &p = s.addProject("gnome.glib", "2.59.1");
    p += RemoteFile("http://ftp.gnome.org/pub/GNOME/sources/glibmm/{M}.{m}/glibmm-{v}.tar.xz");

    auto &glibmm = p.addTarget<Library>("glibmm");

    glibmm += "glib/.*"_rr;
    glibmm.Public += "glib"_id;

    glibmm += "GLIBMM_BUILD"_def;
    glibmm.Public += sw::Static, "GLIBMM_STATIC_LIB"_def;

    glibmm.configureFile("glib/glibmmconfig.h.in", "glibmmconfig.h");

    glibmm.Public += "org.sw.demo.gnome.glib.gobject"_dep;

    for (auto &[p, _] : glibmm["glib/.*\\.[hc]c?g"_rr])
    {
        auto c = glibmm.addCommand();
        // switch perl lib from exe to lib
        c << cmd::prog("org.sw.demo.perl.perl"_dep)
            << cmd::in("tools/gmmproc.in")
            << cmd::in(p)
            << cmd::end()
            << cmd::out("1.out")
            ;
        break;

        /*auto fn = p.filename().u8string();
        auto out = p.parent_path().parent_path() / "glibmm" / fn.substr(0, fn.size() - 1);
        if (!fs::exists(out))
            fs::copy_file(p, out);
        glibmm += out;*/
    }
}
