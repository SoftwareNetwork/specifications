void build(Build &s)
{
    auto &t = s.addLibrary("pybind.pybind11", "2.4.3");
    t += Git("https://github.com/pybind/pybind11", "v{v}");
}
