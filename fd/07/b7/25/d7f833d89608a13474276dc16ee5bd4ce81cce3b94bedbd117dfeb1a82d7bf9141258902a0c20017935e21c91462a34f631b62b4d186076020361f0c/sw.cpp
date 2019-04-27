void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("anthonywilliams.object_ptr", "master");
    t += Git("https://github.com/anthonywilliams/object_ptr");
    t += "object_ptr.hpp";
}
