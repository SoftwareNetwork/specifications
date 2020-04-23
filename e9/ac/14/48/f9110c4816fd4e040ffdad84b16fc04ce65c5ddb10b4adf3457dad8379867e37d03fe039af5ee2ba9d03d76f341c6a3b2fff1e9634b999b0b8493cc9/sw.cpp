void build(Solution &s)
{
   auto &t = s.add<PythonLibrary>("mako", "1.1.2");
   t += Git("https://github.com/sqlalchemy/mako", "rel_{M}_{m}_{p}");
   t += "mako/.*"_rr;
}
