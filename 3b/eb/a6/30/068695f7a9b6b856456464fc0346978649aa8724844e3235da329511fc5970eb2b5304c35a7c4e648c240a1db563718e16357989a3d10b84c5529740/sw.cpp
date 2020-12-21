void build(Solution &s)
{
    auto &tba = s.addStaticLibrary("telegram.telegram_bot_api", "master");
    tba += Git("https://github.com/tdlib/telegram-bot-api");

    tba += "telegram-bot-api/.*"_rr;
    tba += "org.sw.demo.telegram.td.core-1.7"_dep;
}
