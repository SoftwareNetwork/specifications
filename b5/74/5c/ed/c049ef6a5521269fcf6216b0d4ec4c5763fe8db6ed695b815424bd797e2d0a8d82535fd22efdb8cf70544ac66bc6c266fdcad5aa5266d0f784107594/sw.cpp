void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("taskflow", "3.7.0");
    t += Git("https://github.com/taskflow/taskflow", "v{v}");

    t += "taskflow/.*"_rr;
}
