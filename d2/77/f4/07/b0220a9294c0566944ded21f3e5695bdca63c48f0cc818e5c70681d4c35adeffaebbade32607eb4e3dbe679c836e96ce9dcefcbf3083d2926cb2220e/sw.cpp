void build(Build &s)
{
    auto &t = s.addLibrary("pybind.pybind11", "2.13.5");
    t += Git("https://github.com/pybind/pybind11", "v{v}");
}
