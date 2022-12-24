void build(Solution &s)
{
    // mmap added to name for serachability
    auto &t = s.addTarget<LibraryTarget>("alitrack.mman_win32_mmap", "master");
    t += Git("https://github.com/alitrack/mman-win32");
    t += "mman\\.[hc]"_r;
}
