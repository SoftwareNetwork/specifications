void build(Solution &s)
{
   auto &t = s.add<PythonLibrary>("markupsafe", "2.1.5");
   t += Git("https://github.com/pallets/markupsafe");
   t += "src/.*"_rr;
}
