void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("cpp_taskflow", "2.4.0");
    t += Git("https://github.com/cpp-taskflow/cpp-taskflow", "v{v}");

    t += "taskflow/.*"_rr;
}