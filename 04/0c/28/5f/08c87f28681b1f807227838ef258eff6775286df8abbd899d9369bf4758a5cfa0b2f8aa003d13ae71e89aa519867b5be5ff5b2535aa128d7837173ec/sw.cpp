void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("sebsjames.mathplot", "master");
    t += Git("https://github.com/sebsjames/mathplot");
    t += "include/.*"_rr;
    t += "mplot/.*"_rr;
    //t += "shaders/.*"_rr;
}
