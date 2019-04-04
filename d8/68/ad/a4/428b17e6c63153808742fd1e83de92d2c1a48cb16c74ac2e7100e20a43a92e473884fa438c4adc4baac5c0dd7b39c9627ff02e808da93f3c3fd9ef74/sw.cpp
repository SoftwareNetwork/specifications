void build(Solution &s)
{
    auto &p = s.addTarget<LibraryTarget>("agauniyal.rang", "3.1.0");
    p += Git("https://github.com/agauniyal/rang", "v{v}");
}
