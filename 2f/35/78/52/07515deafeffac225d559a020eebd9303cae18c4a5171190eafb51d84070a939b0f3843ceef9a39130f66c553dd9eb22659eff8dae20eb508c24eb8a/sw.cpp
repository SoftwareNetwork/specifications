void build(Solution &s)
{
   auto &t = s.add<PythonLibrary>("python.pypi.jinja2", "3.1.3");
   t += Git("https://github.com/pallets/jinja");
   t += "src/.*"_rr;
   t += "org.sw.demo.python.pypi.markupsafe"_dep;
}
