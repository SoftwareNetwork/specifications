void build(Solution &s)
{
    auto &t = s.addLibrary("kokkos.mdspan", "stable");
    t += Git("https://github.com/kokkos/mdspan", "", "stable");
}
