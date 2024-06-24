void build(Solution &s)
{
    auto &p = s.addProject("libproxy", "0.5.7");
    p += Git("https://github.com/libproxy/libproxy");


    auto &backend = p.addTarget<StaticLibraryTarget>("backend");
    {
        auto &t = backend;
        t.setRootDirectory("src/backend");

        t += ".*\\.[hc]"_r;
        t -= "plugins/.*\\.[hc]"_rr;

        t += "HAVE_CONFIG_GNOME"_def;
        t += "plugins/config-gnome/.*\\.[hc]"_rr;
        t.Public += "org.sw.demo.gnome.glib.gio"_dep;

        t += "HAVE_CURL"_def;
        t.Public += "org.sw.demo.badger.curl.libcurl"_dep;

        t.writeFileOnce("px-backend-config.h");

        //t += "HAVE_CONFIG_WINDOWS"_def;
    /*
    backend_config_h = configuration_data()
backend_config_h.set('HAVE_CONFIG_ENV', get_option('config-env'))
backend_config_h.set('HAVE_CONFIG_GNOME', get_option('config-gnome'))
backend_config_h.set('HAVE_CONFIG_KDE', get_option('config-kde'))
backend_config_h.set('HAVE_CONFIG_OSX', get_option('config-osx') and with_platform_darwin)
backend_config_h.set('HAVE_CONFIG_SYSCONFIG', get_option('config-sysconfig'))
backend_config_h.set('HAVE_PACRUNNER_DUKTAPE', get_option('pacrunner-duktape'))
*/
    }

    auto &t = p.addTarget<StaticLibraryTarget>("libproxy");
    t.setRootDirectory("src/libproxy");
    t.Public += backend;
}
