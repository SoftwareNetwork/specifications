void build(Solution &s)
{
    auto &tiny_dnn = s.addTarget<LibraryTarget>("tiny_dnn", "master");
    tiny_dnn += Git("https://github.com/tiny-dnn/tiny-dnn");
    tiny_dnn += "tiny_dnn/.*"_rr;
}
