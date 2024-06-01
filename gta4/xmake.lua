add_rules("mode.release", "mode.debug")
add_requires("asmjit", "capstone", "asmtk")
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
    remove_files(("src/standalone.cpp"))

    add_links("d3d9", "detours.lib", "user32", "dxguid", "dinput8")
    add_packages("asmjit", "capstone", "asmtk")
--[[
target("gta4exe")
    set_kind("binary")
    set_languages("cxx23")

    add_files("src/standalone.cpp", "src/console.cpp")
    add_files("src/utils/*.cpp")
    add_includedirs("include")
    add_headerfiles("include/console.hpp")

    add_packages("asmjit","capstone","asmtk")
    add_links("user32")
--]]