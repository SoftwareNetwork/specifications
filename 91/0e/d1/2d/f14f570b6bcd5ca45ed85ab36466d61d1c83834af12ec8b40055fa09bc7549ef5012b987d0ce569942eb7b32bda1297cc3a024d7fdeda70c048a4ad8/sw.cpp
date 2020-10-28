void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("jsonrpcx.json_rpc_cxx", "0.2.1");
    t += Git("https://github.com/jsonrpcx/json-rpc-cxx", "v{v}");
}
