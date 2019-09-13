project "Toolkit"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    -- Sets RuntimLibrary to MultiThreaded (non DLL version for static linking)
    staticruntime "on"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    files
    {
        "Logging.h",
        "Logging.cpp",
        "CerealGLM.h",
        "EnumToString.h",
        "Sys.h",
        "Sys.cpp"
    }

    includedirs {
        "%{wks.location}\\external\\spdlog"
    }

    filter "system:windows"
        systemversion "latest"

        defines {
            "WINDOWS"
        }

        
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
        
