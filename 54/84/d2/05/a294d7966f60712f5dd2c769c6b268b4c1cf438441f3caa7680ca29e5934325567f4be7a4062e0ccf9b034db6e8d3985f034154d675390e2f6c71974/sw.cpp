void build(Solution &s)
{
    auto &p = s.addProject("PointCloudLibrary.pcl", "master");
    p += Git("https://github.com/PointCloudLibrary/pcl"/*, "pcl-{v}"*/);

    auto &common = p.addLibrary("common");
    {
        common.setRootDirectory("common");
        if (common.getBuildSettings().TargetOS.is(OSType::Windows))
        {
            common.Public += "WIN32"_def;
            common.Public += "NOMINMAX"_def;
        }
        common.Protected += sw::Shared, "PCLAPI_EXPORTS"_def;
        common.Public += "org.sw.demo.eigen"_dep;
        common.Public += "org.sw.demo.boost.integer"_dep;
        common.Public += "org.sw.demo.boost.smart_ptr"_dep;
        common.Public += "org.sw.demo.boost.mpl"_dep;
        common.Public += "org.sw.demo.boost.optional"_dep;
        common.Public += "org.sw.demo.boost.assert"_dep;
        common.Public += "org.sw.demo.boost.algorithm"_dep;
        common.Public += "org.sw.demo.boost.foreach"_dep;
        common.Public += "org.sw.demo.boost.signals2"_dep;
        common.writeFileOnce("pcl/pcl_config.h", R"()");
    }

    auto &geometry = p.addLibrary("geometry");
    {
        geometry.setRootDirectory("geometry");
        geometry.Public += common;
    }

    auto &ml = p.addLibrary("ml");
    {
        ml.setRootDirectory("ml");
        ml.Public += common;
    }

    auto &octree = p.addLibrary("octree");
    {
        octree.setRootDirectory("octree");
        octree.Public += common;
        octree.Public += "org.sw.demo.boost.graph"_dep;
    }

    auto &kdtree = p.addLibrary("kdtree");
    {
        kdtree.setRootDirectory("kdtree");
        kdtree.Public += common;
        kdtree.Public += "org.sw.demo.mariusmuja.flann-master"_dep;
    }

    auto &search = p.addLibrary("search");
    {
        search.setRootDirectory("search");
        search.Public += kdtree, octree;
    }

    auto &sample_consensus = p.addLibrary("sample_consensus");
    {
        sample_consensus.setRootDirectory("sample_consensus");
        sample_consensus.Public += search;
    }

    auto &filters = p.addLibrary("filters");
    {
        filters.setRootDirectory("filters");
        filters.Public += sample_consensus;
    }

    auto &_2d = p.addStaticLibrary("2d");
    {
        _2d.setRootDirectory("2d");
        _2d -= "src/examples.cpp";
        _2d.Public += filters;
    }

    auto &features = p.addLibrary("features");
    {
        features.setRootDirectory("features");
        features.Public += _2d;
        if (features.getCompilerType() == CompilerType::MSVC)
            features.CompileOptions.push_back("-bigobj");
    }

    auto &keypoints = p.addLibrary("keypoints");
    {
        keypoints.setRootDirectory("keypoints");
        keypoints.Public += features;
    }

    auto &registration = p.addLibrary("registration");
    {
        registration.setRootDirectory("keypoints");
        registration.Public += features;
    }

    return;

    auto &tracking = p.addLibrary("tracking");
    {
        tracking.setRootDirectory("keypoints");
        tracking.Public += filters;
    }

    auto &surface = p.addLibrary("surface");
    {
        surface.setRootDirectory("surface");
        surface -= "src/3rdparty/.*"_rr;
        surface.Public += search;
    }

    return;

    auto &io = p.addLibrary("io");
    {
        io.setRootDirectory("io");
        io.Public += octree;
        io.Public += "org.sw.demo.boost.asio"_dep;
        io.Public += "org.sw.demo.boost.interprocess"_dep;
        io.Public += "org.sw.demo.glennrp.png"_dep;
        io.Public += "org.sw.demo.libusb"_dep;
    }
}
