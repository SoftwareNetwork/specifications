void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("FrancoisChabot.variadic_future", "master");
    t += Git("https://github.com/FrancoisChabot/variadic_future");
}
