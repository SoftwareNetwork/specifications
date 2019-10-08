void build(Solution &s)
{
    auto &t = s.addLibrary("Wohlstand.libADLMIDI", "master");
    t += Git("https://github.com/Wohlstand/libADLMIDI");

    t.ApiName = "ADLMIDI_DECLSPEC";
    t.Public += "ADLMIDI_BUILD"_def;

    //t.writeFileOnce(t.BinaryPrivateDir / "unistd.h");
}
