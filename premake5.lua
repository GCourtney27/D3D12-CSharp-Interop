
workspace ("DX12OnCSharp")
	architecture ("x86_64")

	configurations
	{
		"Debug",
		"Release",
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project("CPPBackend")
    location("CPPBackend")
    kind("SharedLib")
    language("C++")
	cppdialect("C++17")
    targetname("CPPLib")

	targetdir ("Bin/" .. outputdir .. "/%{prj.name}")
	objdir ("Bin-Int/" .. outputdir .. "/%{prj.name}")

	pchheader ("pch.h")
    pchsource ("%{prj.name}/Source/pch.cpp")
    
    files
    {
        "%{prj.name}/Source/**.h",
        "%{prj.name}/Source/**.cpp"
    }

    includedirs
    {
        "Source/"
    }

    defines
    {
        "LIBRARY_EXPORTS",
    }

	filter {"system:windows"}
        systemversion "latest"
		flags
		{
			"MultiProcessorCompile"
		}

        -- postbuildcommands
		-- {
		-- 	("{COPY} ../bin/" .. outputdir .. "/%{prj.name}/%{prj.targetname}.dll ../bin/"..outputdir.."/CSharpFrontend"),
		-- 	("{COPY} ../bin/" .. outputdir .. "/%{prj.name}/%{prj.targetname}.lib ../bin/"..outputdir.."/CSharpFrontend"),
        -- }

project("NativeCodeWrapper")
    location("NativeCodeWrapper")
    kind("SharedLib")
    language("C++")
	cppdialect("C++17")
    clr("On")
    
	targetdir ("Bin/" .. outputdir .. "/%{prj.name}")
    objdir ("Bin-Int/" .. outputdir .. "/%{prj.name}")
    
    files
    {
        "%{prj.name}/Source/**.h",
        "%{prj.name}/Source/**.cpp"
    }

    includedirs
    {
        "Source/",
        "CPPBackend/Source/"
    }

    libdirs
    {
        "Bin/" .. outputdir .. "/CPPBackend"
    }



project("CSharpFrontend")
    location("CSharpFrontend")
    kind("WindowedApp")
    language("C#")
    targetname("Application")

	targetdir ("Bin/" .. outputdir .. "/%{prj.name}")
	objdir ("Bin-Int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/**.cs",
        "%{prj.name}/**.xaml",
        "%{prj.name}/**.resx",
        "%{prj.name}/**.settings",
        "%{prj.name}/**.config",
    }

    includedirs
    {
        "%{prj.name}/Source/",
        "%{prj.name}/Properties/",
    }

    links
    {
		"Microsoft.CSharp",
		"PresentationCore",
		"PresentationFramework",
		"System",
		"System.Core",
		"System.Data",
		"System.Data.DataSetExtensions",
		"System.Net.Http",
		"System.Xaml",
		"System.Xml",
		"System.Xml.Linq",
        "WindowsBase",
        
        "NativeCodeWrapper",
    }