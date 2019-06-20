void build(Solution &s)
{
    auto &t = s.addTarget<Library>("Stiffstream.restinio", "0.5.1");
    t += Git("https://github.com/Stiffstream/restinio", "v.{v}");
    t.setRootDirectory("dev");
    t += "restinio/.*\\.hpp"_r;
}
