void build(Solution &s)
{
    auto &t = s.addStaticLibrary("nodejs.http_parser", "2.9.4");
    t += Git("https://github.com/nodejs/http-parser");

    t += "http_parser.h";
    t += "http_parser.c";
}
