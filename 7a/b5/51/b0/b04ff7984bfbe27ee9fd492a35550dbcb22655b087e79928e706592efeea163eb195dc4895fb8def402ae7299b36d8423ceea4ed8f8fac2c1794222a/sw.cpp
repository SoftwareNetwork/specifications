void build(Solution &s)
{
    auto &t = s.addStaticLibrary("littlevgl.lvgl", "6.1.1");
    t += Git("https://github.com/littlevgl/lvgl", "v{v}");

    t += ".*\\.h"_rr;
    t += "src/.*\\.[hc]"_rr;

    // must be one level higher
    //t.configureFile("lv_conf_template.h", t.SourceDir.parent_path() / "lv_conf.h");
    // also replace ifdef
    //t.patch(t.SourceDir.parent_path() / "lv_conf.h", "#if 0", "#if 1");
}
