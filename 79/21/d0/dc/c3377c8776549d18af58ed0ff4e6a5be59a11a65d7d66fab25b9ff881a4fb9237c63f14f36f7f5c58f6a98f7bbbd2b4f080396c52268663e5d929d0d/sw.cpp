void build(Solution &s)
{
    auto add_project = [&s](const String &name)
    {
        auto &t = s.addTarget<StaticLibrary>("mattreecebentley.plf." + name, "master");
        t += Git("https://github.com/mattreecebentley/plf_" + name);
        t += ".*\\.h"_rr;
    };

    add_project("stack");
    add_project("list");
    add_project("colony");
    add_project("nanotimer");
}
