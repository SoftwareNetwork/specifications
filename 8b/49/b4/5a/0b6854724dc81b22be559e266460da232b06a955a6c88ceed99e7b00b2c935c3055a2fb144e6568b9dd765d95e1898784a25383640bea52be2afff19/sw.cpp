void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("kenavolic.pipet", "master");
    t += Git("https://github.com/kenavolic/pipet");
}
