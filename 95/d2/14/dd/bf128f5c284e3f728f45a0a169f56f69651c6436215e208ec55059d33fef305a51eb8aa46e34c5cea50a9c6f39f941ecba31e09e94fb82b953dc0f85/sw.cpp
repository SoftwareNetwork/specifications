void build(Solution &s)
{
   auto &t = s.add<PythonLibrary>("packaging", "26.0.0");
   t += Git("https://github.com/pypa/packaging", "{M}.{m}");
   t += "src/.*"_rr;
}
