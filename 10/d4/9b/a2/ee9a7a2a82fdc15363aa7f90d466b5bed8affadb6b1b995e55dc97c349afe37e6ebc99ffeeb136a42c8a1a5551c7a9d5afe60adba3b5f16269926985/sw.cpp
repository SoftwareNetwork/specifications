void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("anthonywilliams.strong_typedef", "master");
    t += Git("https://github.com/anthonywilliams/strong_typedef");
    t += "strong_typedef.hpp";
}
