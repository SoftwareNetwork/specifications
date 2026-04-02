void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("htacg.tidy_html5", "5.9.14");
    t += Git("https://github.com/htacg/tidy-html5", "{v}-next");
    t += "DISABLE_DEBUG_LOG"_def;
    // allow p tags inside spans (make <p> inline)
    t.patch("src/tags.c", "&TY_(W3CAttrsFor_P)[0],          (CM_BLOCK|CM_OPT)", "&TY_(W3CAttrsFor_P)[0],          (CM_INLINE|CM_BLOCK|CM_OPT)");
}
