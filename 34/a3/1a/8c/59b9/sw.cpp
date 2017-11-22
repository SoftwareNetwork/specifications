void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo.google");
    auto &p = s.addProject("tesseract", "master");
    p.Source = Git("https://github.com/tesseract-ocr/tesseract", "", "{v}");

    auto &libtesseract = p.addTarget<LibraryTarget>("libtesseract");

    libtesseract.setChecks("libtesseract");

    libtesseract.ExportAllSymbols = true;
    libtesseract +=
        "api/.*\\.cpp"_rr,
        "api/.*\\.h"_rr,
        "arch/.*\\.cpp"_rr,
        "arch/.*\\.h"_rr,
        "ccmain/.*\\.cpp"_rr,
        "ccmain/.*\\.h"_rr,
        "ccstruct/.*\\.cpp"_rr,
        "ccstruct/.*\\.h"_rr,
        "ccutil/.*\\.cpp"_rr,
        "ccutil/.*\\.h"_rr,
        "classify/.*\\.cpp"_rr,
        "classify/.*\\.h"_rr,
        "cutil/.*\\.cpp"_rr,
        "cutil/.*\\.h"_rr,
        "dict/.*\\.cpp"_rr,
        "dict/.*\\.h"_rr,
        "lstm/.*\\.cpp"_rr,
        "lstm/.*\\.h"_rr,
        "opencl/.*\\.cpp"_rr,
        "opencl/.*\\.h"_rr,
        "textord/.*\\.cpp"_rr,
        "textord/.*\\.h"_rr,
        "viewer/.*\\.cpp"_rr,
        "viewer/.*\\.h"_rr,
        "vs2010/port/.*"_rr,
        "wordrec/.*\\.cpp"_rr,
        "wordrec/.*\\.h"_rr;

    libtesseract -=
        "api/tesseractmain.cpp",
        "viewer/svpaint.cpp";

    libtesseract.Public +=
        "ccutil"_id,
        "viewer"_id,
        "ccstruct"_id,
        "dict"_id,
        "ccmain"_id,
        "api"_id,
        "lstm"_id,
        "opencl"_id,
        "vs2010/port"_id,
        "cutil"_id,
        "wordrec"_id,
        "classify"_id,
        "textord"_id,
        "arch"_id;

    libtesseract.Public += "HAVE_CONFIG_H"_d;
    libtesseract.Public += "USE_STD_NAMESPACE=1"_d;
    libtesseract.Public += "WINDLLNAME=\"tesseract\""_d;
    libtesseract.Public += "_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS=1"_d;
    libtesseract.Interface += sw::Shared, "TESS_IMPORTS"_d;
    libtesseract.Private += sw::Shared, "TESS_EXPORTS"_d;

    libtesseract.Public += "pub.cppan2.demo.danbloomberg.leptonica-1"_dep;

    libtesseract.writeFileOnce("config_auto.h", true);
    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        libtesseract += "ws2_32.lib"_l;
    }
    else
    {
        libtesseract -= "vs2010/port/.*"_rr;
    }

    //
    auto &tesseract = p.addTarget<ExecutableTarget>("tesseract");
    tesseract += "api/tesseractmain.cpp", libtesseract;

    //
    auto &tessopt = p.addTarget<StaticLibraryTarget>("tessopt");
    tessopt += "training/tessopt.*"_rr;
    tessopt.Public += libtesseract, "training"_idir;

    //
    auto &common_training = p.addTarget<StaticLibraryTarget>("common_training");
    common_training +=
        "training/commandlineflags.*"_rr,
        "training/commontraining.*"_rr;
    common_training.Public += tessopt, "training"_idir;

    //
    auto &unicharset_training = p.addTarget<StaticLibraryTarget>("unicharset_training");
    unicharset_training +=
        "training/fileio.*"_rr,
        "training/icuerrorcode.h",
        "training/lang_model_helpers.*"_rr,
        "training/lstmtester.*"_rr,
        "training/normstrngs.*"_rr,
        "training/unicharset_training_utils.*"_rr,
        "training/validat.*"_rr;
    unicharset_training.Public += common_training, "training"_idir,
        "pub.cppan2.demo.unicode.icu.i18n"_dep;

    //
    auto &ambiguous_words = p.addTarget<ExecutableTarget>("ambiguous_words");
    ambiguous_words += "training/ambiguous_words.cpp", libtesseract;

    //
    auto &classifier_tester = p.addTarget<ExecutableTarget>("classifier_tester");
    classifier_tester += "training/classifier_tester.cpp", common_training;

    //
    auto &combine_lang_model = p.addTarget<ExecutableTarget>("combine_lang_model");
    combine_lang_model += "training/combine_lang_model.cpp", unicharset_training;

    //
    auto &combine_tessdata = p.addTarget<ExecutableTarget>("combine_tessdata");
    combine_tessdata += "training/combine_tessdata.cpp", libtesseract;

    //
    auto &cntraining = p.addTarget<ExecutableTarget>("cntraining");
    cntraining += "training/cntraining.cpp", common_training;

    //
    auto &dawg2wordlist = p.addTarget<ExecutableTarget>("dawg2wordlist");
    dawg2wordlist += "training/dawg2wordlist.cpp", libtesseract;

    //
    auto &mftraining = p.addTarget<ExecutableTarget>("mftraining");
    mftraining += "training/mftraining.cpp", "training/mergenf.*"_rr, common_training;

    //
    auto &shapeclustering = p.addTarget<ExecutableTarget>("shapeclustering");
    shapeclustering += "training/shapeclustering.cpp", common_training;

    //
    auto &unicharset_extractor = p.addTarget<ExecutableTarget>("unicharset_extractor");
    unicharset_extractor += "training/unicharset_extractor.cpp", unicharset_training;

    //
    auto &wordlist2dawg = p.addTarget<ExecutableTarget>("wordlist2dawg");
    wordlist2dawg += "training/wordlist2dawg.cpp", libtesseract;

    //
    auto &lstmeval = p.addTarget<ExecutableTarget>("lstmeval");
    lstmeval += "training/lstmeval.cpp", unicharset_training;

    //
    auto &lstmtraining = p.addTarget<ExecutableTarget>("lstmtraining");
    lstmtraining += "training/lstmtraining.cpp", unicharset_training;

    //
    auto &set_unicharset_properties = p.addTarget<ExecutableTarget>("set_unicharset_properties");
    set_unicharset_properties += "training/set_unicharset_properties.cpp", unicharset_training;

    //
    auto &text2image = p.addTarget<ExecutableTarget>("text2image");

    text2image +=
        "training/boxchar.cpp",
        "training/boxchar.h",
        "training/degradeimage.cpp",
        "training/degradeimage.h",
        "training/icuerrorcode.h",
        "training/ligature_table.cpp",
        "training/ligature_table.h",
        "training/pango_font_info.cpp",
        "training/pango_font_info.h",
        "training/stringrenderer.cpp",
        "training/stringrenderer.h",
        "training/text2image.cpp",
        "training/tlog.cpp",
        "training/tlog.h",
        "training/util.h";

    text2image.Public += unicharset_training;
    text2image.Public += "pub.cppan2.demo.gnome.pango.pangocairo-1"_dep;
}

void check(Checker &c)
{
    auto &s = c.addSet("libtesseract");
    s.checkFunctionExists("getline");
    s.checkIncludeExists("dlfcn.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("limits.h");
    s.checkIncludeExists("malloc.h");
    s.checkIncludeExists("memory.h");
    s.checkIncludeExists("stdbool.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("sys/ipc.h");
    s.checkIncludeExists("sys/shm.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("sys/wait.h");
    s.checkIncludeExists("tiffio.h");
    s.checkIncludeExists("unistd.h");
    s.checkTypeSize("long long int");
    s.checkTypeSize("mbstate_t");
    s.checkTypeSize("off_t");
    s.checkTypeSize("size_t");
    s.checkTypeSize("void *");
    s.checkTypeSize("wchar_t");
    s.checkTypeSize("_Bool");
    {
        auto &c = s.checkSymbolExists("snprintf");
        c.Parameters.Includes.push_back("stdio.h");
    }
}
