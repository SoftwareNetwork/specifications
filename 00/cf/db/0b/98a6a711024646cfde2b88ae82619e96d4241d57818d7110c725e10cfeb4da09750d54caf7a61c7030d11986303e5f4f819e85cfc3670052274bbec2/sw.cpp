void build(Solution &s)
{
    auto &t = s.addLibrary("biojppm.debugbreak", "master");
    t += Git("https://github.com/biojppm/debugbreak");
    t += "debugbreak.h";

    auto c = t.addCommand(SW_VISIBLE_BUILTIN_FUNCTION(copy_file));
    c << cmd::in("debugbreak.h");
    c << cmd::out("c4/ext/debugbreak/debugbreak.h");
}
