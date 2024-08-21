void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("ned14.outcome", "2.2.10");
    t += Git("https://github.com/ned14/outcome", "v{v}");

    t.Public += "OUTCOME_DISABLE_ABI_PERMUTATION"_def;
    t.Public += "org.sw.demo.ned14.quickcpplib-master"_dep;
}
