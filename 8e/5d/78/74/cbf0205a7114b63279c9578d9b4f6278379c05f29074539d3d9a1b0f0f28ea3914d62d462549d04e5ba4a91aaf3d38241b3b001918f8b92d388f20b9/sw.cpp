void build(Solution &s)
{
    auto &p = s.addProject("Tessil");

    auto add_repo = [&p](String n, const String &v, bool branch = false)
    {
        auto url = "https://github.com/Tessil/" + n;
        boost::replace_all(n, "-", "_");
        auto &t = p.addTarget<LibraryTarget>(n, v);
        t += Git(url, branch ? "" : "v{v}", branch ? "{v}" : "");
    };

    // do not use configs like this!
    // split one config per library
    // it was used for speed
    add_repo("robin-map", "1.0.1");
    add_repo("hopscotch-map", "2.2.1");
    add_repo("ordered-map", "0.8.1");
    add_repo("hat-trie", "0.6.0");
    add_repo("sparse-map", "0.6.2");
    add_repo("array-hash", "0.7.0");
}
