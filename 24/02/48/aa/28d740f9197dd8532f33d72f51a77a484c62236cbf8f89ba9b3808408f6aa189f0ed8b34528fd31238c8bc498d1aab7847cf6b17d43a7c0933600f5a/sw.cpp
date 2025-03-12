void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("Thalhammer.jwt_cpp", "0.7.1");
    t += Git("https://github.com/Thalhammer/jwt-cpp");
}
