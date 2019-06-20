void build(Solution &s)
{
    auto &t = s.addTarget<Library>("Stiffstream.restinio", "0.4.9");
    t += Git("https://github.com/Stiffstream/restinio", "v.{v}");
    t.setRootDirectory("dev");
    t += "restinio/.*\\.hpp"_r;
}
