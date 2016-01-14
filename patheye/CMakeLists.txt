cmake_minimum_required (VERSION 2.8)
project (patheye)

list (APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

include (FindPkgConfig)
include (FindVala)
include (UseVala)

find_package (Vala REQUIRED)

pkg_check_modules (GTK REQUIRED gtk+-3.0)

include_directories (${GTK_INCLUDE_DIRS} cbits/)

file (GLOB_RECURSE patheye_cbits cbits/*.cpp)

vala_precompile (
	VALA_C
		src/main.vala
		src/Color.vala
		src/Gate.vala
		src/GateNetwork.vala
		src/GateNetworkParser.vala
		src/Map.vala
		src/MapParser.vala
		src/Path.vala
		src/PathParser.vala
		src/PointF.vala
		src/Segment.vala
		src/UI/AboutDialog.vala
		src/UI/Application.vala
		src/UI/Board.vala
		src/UI/Commands.vala
		src/UI/ControlBar.vala
		src/UI/PathScale.vala
		src/UI/Window.vala
	PACKAGES
		gtk+-3.0
		posix
	OPTIONS
		--thread
		-g
)

add_library (patheye_cbits STATIC
	${patheye_cbits}
)

add_executable (patheye
	${VALA_C}
)

target_compile_options (patheye PRIVATE -w)
target_compile_options (patheye_cbits PRIVATE -std=c++11)

target_link_libraries (patheye patheye_cbits)
target_link_libraries (patheye ${GTK_LIBRARIES})

install (TARGETS patheye RUNTIME DESTINATION bin)

#file (GLOB_RECURSE include_files "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
#install (DIRECTORY include/ DESTINATION include/patheye)
