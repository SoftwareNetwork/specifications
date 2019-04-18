void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("recatek.fixed_storage_arrays", "master");
    t += Git("https://github.com/recatek/fixed_storage_arrays");
}
