void build(Solution &s)
{
    // 5.9 branch produces huge output (50KB in -> 500KB out)
    auto &t = s.addTarget<LibraryTarget>("htacg.tidy_html5", "5.8.0");
    t += Git("https://github.com/htacg/tidy-html5", "{v}");
    t += "DISABLE_DEBUG_LOG"_def;
    // allow p tags inside spans (make <p> inline)
    t += "include/.*"_rr;
    t += "src/.*"_rr;
    t.patch("src/tags.c", "&TY_(W3CAttrsFor_P)[0],          (CM_BLOCK|CM_OPT)", "&TY_(W3CAttrsFor_P)[0],          (CM_INLINE|CM_BLOCK|CM_OPT)");
}
