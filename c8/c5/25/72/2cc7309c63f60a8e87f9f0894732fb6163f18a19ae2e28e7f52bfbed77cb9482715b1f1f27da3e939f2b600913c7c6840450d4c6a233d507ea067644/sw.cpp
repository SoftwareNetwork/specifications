void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("htacg.tidy_html5", "5.8.0");
    t += Git("https://github.com/htacg/tidy-html5");
}
