-- Local Project Packages
set_allowedarchs("x86")
package("polyhook")
    set_sourcedir(path.join(os.scriptdir(), "polyhook"))
    add_deps("cmake")
    on_install("windows", function(package)
        local configs = {}
        table.insert(configs, "-DPOLYHOOK_BUILD_SHARED_LIB=OFF")
        import("package.tools.cmake").install(package, configs, {buildir="build"})
        local libs = {"PolyHook_2", "asmjit", "asmtk", "zydis"}
        for _,link in ipairs(libs) do 
            package:add("links", link)
        end
    end)
package_end()