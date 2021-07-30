void build(Solution &s)
{
    auto &p = s.addTarget<LibraryTarget>("agauniyal.rang", "3.2");
    p += Git("https://github.com/agauniyal/rang", "v{M}.{m}");
}
