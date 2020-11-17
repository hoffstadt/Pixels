cmake_minimum_required (VERSION 3.13)

add_library(pixels STATIC)
set_property(TARGET pixels PROPERTY CXX_STANDARD 17)

target_sources(pixels

	PRIVATE

		"src/Graphics.cpp"
		"src/Window.cpp"

		# imgui
		"../Dependencies/imgui/imgui.cpp"
		"../Dependencies/imgui/imgui_demo.cpp"
		"../Dependencies/imgui/imgui_draw.cpp"
		"../Dependencies/imgui/imgui_widgets.cpp"
		"$<$<PLATFORM_ID:Windows>:../Dependencies/imgui/examples/imgui_impl_win32.cpp>"
		"$<$<PLATFORM_ID:Windows>:../Dependencies/imgui/examples/imgui_impl_dx11.cpp>"

)

target_include_directories(pixels

	PRIVATE

		"include/"

		"../Dependencies/imgui/"
		"../Dependencies/imgui/examples/"
)

target_link_libraries(pixels

	PUBLIC

		$<$<PLATFORM_ID:Windows>:d3d11>
)
