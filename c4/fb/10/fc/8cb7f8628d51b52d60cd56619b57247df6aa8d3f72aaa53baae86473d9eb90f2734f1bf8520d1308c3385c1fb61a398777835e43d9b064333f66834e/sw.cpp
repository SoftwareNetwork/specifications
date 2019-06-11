void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("qicosmos.iguana", "master");
    t += Git("https://github.com/qicosmos/iguana");
    t += "iguana/.*"_rr;
}
