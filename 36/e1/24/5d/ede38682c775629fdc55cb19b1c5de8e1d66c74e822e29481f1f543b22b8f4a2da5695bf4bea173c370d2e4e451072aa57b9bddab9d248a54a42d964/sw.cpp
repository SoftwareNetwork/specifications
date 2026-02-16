void build(Solution &s)
{
    auto &t = s.addTarget<SharedLibrary>("perl.packages.cpan.Text.Template", "1.61");
    t += Git("https://github.com/mschout/perl-text-template", "v{M}.{m}");
    t += "lib/.*"_rr;
}
