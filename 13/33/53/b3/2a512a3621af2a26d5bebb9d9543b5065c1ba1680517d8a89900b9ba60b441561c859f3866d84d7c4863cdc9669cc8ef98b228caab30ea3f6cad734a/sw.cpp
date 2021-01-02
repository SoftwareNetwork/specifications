void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("cpp_taskflow", "3.0.0");
    t += Git("https://github.com/cpp-taskflow/cpp-taskflow", "v{v}");

    t += "taskflow/.*"_rr;
}
