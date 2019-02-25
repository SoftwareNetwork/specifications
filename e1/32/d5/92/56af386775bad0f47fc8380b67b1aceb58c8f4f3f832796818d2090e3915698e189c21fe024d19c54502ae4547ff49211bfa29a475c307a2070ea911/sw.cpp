void build(Solution &s)
{
    auto &p = s.addProject("Tessil");

    auto add_repo = [&p](String n, const String &v, bool branch = false)
    {
        auto url = "https://github.com/Tessil/" + n;
        boost::replace_all(n, "-", "_");
        auto &t = p.addTarget<LibraryTarget>(n, v);
        t += Git(url, branch ? "" : "v{v}", "{v}");
    };

    add_repo("ordered-map", "0.8.1");
}
