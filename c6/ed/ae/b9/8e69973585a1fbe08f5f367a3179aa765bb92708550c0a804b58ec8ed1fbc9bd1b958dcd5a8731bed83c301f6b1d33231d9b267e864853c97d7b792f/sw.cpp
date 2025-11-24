void build(Solution &s)
{
   auto &t = s.add<PythonLibrary>("markupsafe", "3.0.3");
   t += Git("https://github.com/pallets/markupsafe");
   t += "src/.*"_rr;
}
