void build(Solution &s)
{
    auto &sdl = s.addProject("WohlSoft.sdl", "master");
    sdl += Git("https://github.com/WohlSoft/SDL-Mixer-X");

    auto &t = sdl.addTarget<LibraryTarget>("mixer");
    {
        t.ApiName = "DECLSPEC";

        t += "include/.*"_rr;
        t += "src/.*"_rr;

        t += "MUSIC_WAV"_def;
        t += "MUSIC_MID_NATIVE"_def;

        t.Public += "include/SDL_mixer_ext"_idir;
        t += "src"_idir;
        t += "src/codecs"_idir;

        t.Public += "org.sw.demo.valve.sdl.sdl"_dep;

        if (t.getBuildSettings().TargetOS.Type != OSType::Windows)
            t += "MUSIC_CMD"_def;

        t += "MUSIC_OPUS"_def;
        t += "org.sw.demo.xiph.opusfile"_dep;

        t += "MUSIC_FLAC"_def;
        t += "org.sw.demo.xiph.flac.libflac"_dep;

        t += "MUSIC_OGG"_def;
        t += "org.sw.demo.xiph.vorbis.vorbisfile"_dep;

        t += "MUSIC_MOD_MODPLUG"_def;
        t += "org.sw.demo.konstanty.libmodplug-master"_dep;

        t += "MUSIC_MOD_MIKMOD"_def;
        t += "org.sw.demo.sezero.mikmod"_dep;

        t += "MUSIC_MP3_MPG123"_def;
        t += "off_t=int"_def;
        t += "org.sw.demo.mpg123.mpg123"_dep;

        t += "MUSIC_MP3_MAD"_def;
        t += "org.sw.demo.libmad"_dep;

        t += "MUSIC_MID_FLUIDSYNTH"_def;
        t += "org.sw.demo.FluidSynth.fluidsynth"_dep;

        t += "MUSIC_MID_OPNMIDI"_def;
        t += "org.sw.demo.Wohlstand.libOPNMIDI-master"_dep;

        t += "MUSIC_MID_ADLMIDI"_def;
        t += "org.sw.demo.Wohlstand.libADLMIDI-master"_dep;

        t += "MUSIC_MID_TIMIDITY"_def;
        t += "org.sw.demo.WohlSoft.AudioCodecs.libtimidity-master"_dep;

        // install
        if (!t.DryRun)
        {
            auto d = t.BinaryDir / "SDL2";
            if (!fs::exists(d))
            {
                fs::create_directories(d);
                for (auto &[p, sf] : t["include/.*"_rr])
                    fs::copy_file(p, d / p.filename(), fs::copy_options::update_existing);
            }
        }
    }
}
