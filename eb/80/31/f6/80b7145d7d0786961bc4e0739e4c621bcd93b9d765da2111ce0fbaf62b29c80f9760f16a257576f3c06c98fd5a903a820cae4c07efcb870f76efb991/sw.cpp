void build(Solution &s)
{
    auto &p = s.addProject("littlevgl.lvgl", "6.1.1");
    p += Git("https://github.com/littlevgl/lvgl", "v{v}");

    auto add_library = [&p](const String &name)
    {
        auto &t = p.addStaticLibrary(name);
        t += FileRegex("src/lv_" + name, ".*\\.[hc]", true);
    };

    add_library("core");
    add_library("draw");
    add_library("font");
    add_library("hal");
    add_library("misc");
    add_library("objx");
    add_library("themes");

    // must be one level higher
    //t.configureFile("lv_conf_template.h", t.SourceDir.parent_path() / "lv_conf.h");
    // also replace ifdef
    //t.patch(t.SourceDir.parent_path() / "lv_conf.h", "#if 0", "#if 1");
}
