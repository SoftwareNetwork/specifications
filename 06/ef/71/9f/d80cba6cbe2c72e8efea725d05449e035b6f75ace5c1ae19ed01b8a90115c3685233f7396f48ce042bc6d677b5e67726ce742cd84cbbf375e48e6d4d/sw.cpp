void build(Solution &s)
{
    auto &tgbot = s.addLibrary("tgbot", "1.1.5.7.2");
    tgbot += Git("https://github.com/egorpugin/tgbot");

    auto &apitool = tgbot.addExecutable("apitool");
    {
        auto &t = apitool;
        t.PackageDefinitions = true;
        t += cpp23;
        t += "tools/.*"_rr;
        t += "pub.egorpugin.primitives.emitter"_dep;
        t += "pub.egorpugin.primitives.xml"_dep;
        t += "pub.egorpugin.primitives.http"_dep;
        t += "pub.egorpugin.primitives.sw.main"_dep;
        t += "org.sw.demo.nlohmann.json"_dep;
        t += "org.sw.demo.imageworks.pystring"_dep;
    }

    //
    {
        tgbot += cpp23;
        tgbot += "include/.*"_rr;
        if (tgbot.getCompilerType() == CompilerType::MSVC)
            tgbot.Public.CompileOptions.push_back("/Zc:__cplusplus");
        tgbot.Public += "org.sw.demo.nlohmann.json"_dep;
        tgbot.Public += "org.sw.demo.boostorg.pfr-develop"_dep;
        {
            auto c = tgbot.addCommand();
            c << cmd::prog(apitool)
                << cmd::wdir(tgbot.BinaryDir)
                << cmd::in("TelegramBotAPI.html")
                << cmd::end()
                << cmd::out("types.inl.h")
                << cmd::out("methods.inl.h")
                ;
            // to make dependency? (in VS IDE only?)
            // causes cyclic deps for some reason
            //tgbot.Public += "types.inl.h";
            //tgbot.Public += "methods.inl.h";
        }
    }

    auto &report_detection_bot = tgbot.addTarget<ExecutableTarget>("examples.report_detection_bot");
    {
        auto &bot = report_detection_bot;
        bot.PackageDefinitions = true;
        bot += cpp23;
        bot += "examples/report_detection_bot/src/.*"_rr;
        bot +=
            "pub.egorpugin.primitives.http"_dep,
            "pub.egorpugin.primitives.sw.main"_dep,
            tgbot
            ;
    }
}
