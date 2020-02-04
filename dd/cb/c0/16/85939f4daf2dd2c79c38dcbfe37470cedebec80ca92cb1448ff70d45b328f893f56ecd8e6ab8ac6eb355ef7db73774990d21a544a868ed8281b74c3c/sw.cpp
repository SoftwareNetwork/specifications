// this is a stub
void build(Solution &s)
{
    auto &t = s.addLibrary("axboe.liburing", "0.3.0");
    t += Git("https://github.com/axboe/liburing", "liburing-{M}.{m}");
}
