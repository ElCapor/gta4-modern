add_rules("mode.release", "mode.debug")
add_requires("polyhook")
target("gta4dll")
    -- Project Settings
    set_kind("shared")
    set_languages("cxx23")
    
    add_packages("polyhook")
    
    -- ImGui
    add_files("../extern/vendor/imgui/*.cpp")
    add_includedirs("../extern/vendor/imgui")
    add_files("../extern/vendor/imgui/backends/imgui_impl_win32.cpp", "../extern/vendor/imgui/backends/imgui_impl_dx9.cpp")
    add_headerfiles("../extern/vendor/imgui/backends/imgui_impl_win32.h", "../extern/vendor/imgui/backends/imgui_impl_dx9.h")
    -- Include dirs
    add_includedirs("include", "../extern/vendor")
    add_linkdirs("../extern/vendor/detours/x86")
    add_files("src/**.cpp")

    add_links("d3d9", "detours.lib")