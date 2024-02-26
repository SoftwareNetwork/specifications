void build(Solution &s)
{
   auto &t = s.add<PythonLibrary>("jinja", "3.1.3");
   t += Git("https://github.com/pallets/jinja");
   t += "src/.*"_rr;
}
