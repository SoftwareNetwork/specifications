void build(Solution &s)
{
    auto &p = s.addTarget<LibraryTarget>("duneroadrunner.SaferCPlusPlus", "master");
    p += Git("https://github.com/duneroadrunner/SaferCPlusPlus");
    p += ".*\\.h"_r;
}
