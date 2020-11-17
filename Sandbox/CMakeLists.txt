cmake_minimum_required (VERSION 3.13)

add_executable(PixelSandbox)
set_property(TARGET PixelSandbox PROPERTY CXX_STANDARD 17)

target_sources(PixelSandbox

	PRIVATE

		"main.cpp"

)

target_include_directories(PixelSandbox

	PRIVATE

		"../Pixels/include/"
		"../Dependencies/imgui/"

)

target_link_libraries(PixelSandbox

	PRIVATE

		pixels
)