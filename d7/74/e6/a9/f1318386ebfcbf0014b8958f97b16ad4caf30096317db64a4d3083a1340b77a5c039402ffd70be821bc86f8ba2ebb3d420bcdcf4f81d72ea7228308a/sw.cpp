void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("code_disaster.libsdlang", "master");
    t += Git("https://github.com/code-disaster/libsdlang");

    t += "sdlang.h";
    t += "sdlang.c";
    t += "sdlang.inl";
}
