void build(Solution &s)
{
    auto &p = s.addLibrary("CaseyCarter.cmcstl2", "master");
    p += Git("https://github.com/CaseyCarter/cmcstl2", "", "{v}");
}
