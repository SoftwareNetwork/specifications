void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("sebsjames.maths", "2.0");
    t += Git("https://github.com/sebsjames/maths", "{M}.{m}");
    t += "sm/.*"_rr;
}
