add_rules("mode.release", "mode.debug")
target("gta4dll")
    -- Project Settings
    set_kind("shared")
    set_languages("cxx23")
    
    -- ImGui
    add_files("../extern/vendor/imgui/*.cpp")
    add_includedirs("../extern/vendor/imgui")
    add_files("../extern/vendor/imgui/backends/imgui_impl_win32.cpp", "../extern/vendor/imgui/backends/imgui_impl_dx9.cpp")
    add_headerfiles("../extern/vendor/imgui/backends/imgui_impl_win32.h", "../extern/vendor/imgui/backends/imgui_impl_dx9.h")

    -- eyestep
    add_files("../extern/vendor/eyestep/*.cpp")
    -- Include dirs
    add_includedirs("include", "../extern/vendor","../extern/vendor/detours/include")
    add_linkdirs("../extern/vendor/detours/x86")
    add_files("src/**.cpp")
    remove_files("src/feature/rage/scr/scrThreadHook.cpp")

    add_links("d3d9", "detours.lib")