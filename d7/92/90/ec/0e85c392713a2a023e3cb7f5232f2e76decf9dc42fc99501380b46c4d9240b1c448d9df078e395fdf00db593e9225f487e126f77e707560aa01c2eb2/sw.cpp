void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("htacg.tidy_html5", "5.9.14");
    t += Git("https://github.com/htacg/tidy-html5", "{v}-next");
    t += "DISABLE_DEBUG_LOG"_def;
}
