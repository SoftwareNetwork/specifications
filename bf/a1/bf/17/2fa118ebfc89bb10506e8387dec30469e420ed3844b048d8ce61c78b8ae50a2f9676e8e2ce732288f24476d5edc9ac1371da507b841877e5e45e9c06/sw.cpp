void build(Solution &s)
{
    auto &t = s.addLibrary("dmlc.xgboost", "0.90.0");
    t += Git("https://github.com/dmlc/xgboost", "v{M}.{m}");

    t += "org.sw.demo.dmlc.rabit-master"_dep;
}
