void build(Solution &s)
{
    auto &t = s.addLibrary("juce_framework.JUCE", "7.0.8");
    t += Git("https://github.com/juce-framework/JUCE");

    t += cpp17;
    t -= "modules/.*"_rr;
    t.Public += "modules"_idir;

    if (!t.DryRun)
    for (auto &d : fs::directory_iterator(t.SourceDir / "modules"))
    {
        if (!d.is_directory())
            continue;
        if (d.path().filename().string() == "juce_audio_plugin_client")
            continue;
        t += FileRegex(d.path(), ".*", false);
        t += Definition("JUCE_MODULE_AVAILABLE_" + d.path().filename().string());
    }

    t += "JUCE_VST3HEADERS_INCLUDE_HEADERS_ONLY"_def;
    t += "modules/juce_audio_processors/format_types/VST3_SDK"_idir;

    t += "JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED"_def;
    t += "JUCE_INCLUDE_ZLIB_CODE=0"_def;
    t += "JUCE_INCLUDE_PNGLIB_CODE=0"_def;
    t += "JUCE_INCLUDE_JPEGLIB_CODE=0"_def;

    t += "JucePlugin_Build_VST=0"_def;
    t += "JucePlugin_Build_VST3"_def;
    t += "JucePlugin_Build_AU"_def;
    t += "JucePlugin_Build_AUv3"_def;
    t += "JucePlugin_Build_RTAS"_def;
    t += "JucePlugin_Build_AAX=0"_def;
    t += "JucePlugin_Build_Standalone"_def;
    t += "JucePlugin_Build_LV2=0"_def;
    t += "JucePlugin_Build_Unity=0"_def;

    t += "JucePlugin_IsSynth=1"_def;
    t += "JucePlugin_ManufacturerCode=0"_def;
    t += "JucePlugin_PluginCode=0"_def;
    t += "JucePlugin_VSTUniqueID=JucePlugin_PluginCode"_def;
    t += "JucePlugin_ProducesMidiOutput=1"_def;
    t += "JucePlugin_WantsMidiInput=1"_def;
    t += "JucePlugin_EditorRequiresKeyboardFocus=0"_def;
    t += "JucePlugin_LV2URI=1"_def;
    t += "JucePlugin_AAXIdentifier=\"\""_def;
    t += "JucePlugin_Name=\"\""_def;
    t += "JucePlugin_VersionString=\"\""_def;
    t += "JucePlugin_VersionCode=0"_def;

    t += sw::Shared, "JUCE_DLL_BUILD"_def;
    t.Interface += sw::Shared, "JUCE_DLL"_def;

    t += "advapi32.lib"_slib;
    t += "Comdlg32.lib"_slib;
    t += "Dbghelp.lib"_slib;
    t += "Shlwapi.lib"_slib;
    t += "imm32.lib"_slib;
    t += "gdi32.lib"_slib;
    t += "Shell32.lib"_slib;
    t += "ole32.lib"_slib;
    t += "OleAut32.lib"_slib;
    t += "user32.lib"_slib;
    t += "Winmm.lib"_slib;
    t += "Wininet.lib"_slib;
    t += "ws2_32.lib"_slib;
    t += "version.lib"_slib;
    t += "uuid.lib"_slib;
    t += "Strmiids.lib"_slib;

    t.CompileOptions.push_back("/bigobj");

    t += "org.sw.demo.madler.zlib"_dep;
    t += "org.sw.demo.glennrp.png"_dep;
    t += "org.sw.demo.jpeg"_dep;
    t += "org.sw.demo.find.opengl-master"_dep;
}
