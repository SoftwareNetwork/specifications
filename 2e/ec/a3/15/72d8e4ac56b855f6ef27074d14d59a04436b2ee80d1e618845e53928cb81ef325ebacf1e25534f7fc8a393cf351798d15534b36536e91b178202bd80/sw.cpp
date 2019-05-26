void build(Solution &s)
{
    auto &lzma_sdk = s.addTarget<LibraryTarget>("_7zip.lzma_sdk", "16.4.0");
    lzma_sdk += RemoteFile("https://www.7-zip.org/a/lzma{M}0{m}.7z");

    lzma_sdk.setRootDirectory("C");
    lzma_sdk +=
        ".*\\.h"_rr,
        "Alloc.c",
        "LzFind.c",
        "LzFindMt.c",
        "LzmaDec.c",
        "LzmaEnc.c",
        "LzmaLib.c",
        "Threads.c",
        "Util/LzmaLib/LzmaLib.def";
}
