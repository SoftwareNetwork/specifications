void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("awslabs.aws_c_mqtt", "0.3.5");
    t += Git("https://github.com/awslabs/aws-c-mqtt", "v{v}");

    t += "org.sw.demo.amazon.awslabs.c_common"_dep;
}
