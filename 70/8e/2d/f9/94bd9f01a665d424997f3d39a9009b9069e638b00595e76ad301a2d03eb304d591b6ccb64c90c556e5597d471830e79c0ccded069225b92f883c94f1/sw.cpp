void build(Build &s)
{
    auto &t = s.addLibrary("pybind.pybind11", "2.11.0");
    t += Git("https://github.com/pybind/pybind11", "v{v}");
}
