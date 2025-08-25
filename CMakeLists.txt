# minimum version

cmake_minimum_required(VERSION 3.12)

# project name

project(MatrixCalulator)

# compiling requirements

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQIRED True)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "-g -w")
#add_compile_options(-fsanitize=address)

# find openGL

find_package(OpenGL REQUIRED)

# generate glfw library

add_subdirectory(../glfw ./bin)

# add source files

set(SOURCES src/main.cpp
			src/controller/controller.cpp
			src/view/view.cpp
			src/model/model.cpp)

# add imgui source files

set(IMGUI_SOURCES ../imgui/imgui.cpp
				  ../imgui/imgui_draw.cpp
				  ../imgui/imgui_tables.cpp
				  ../imgui/imgui_widgets.cpp
				  ../imgui/backends/imgui_impl_opengl3.cpp
				  ../imgui/backends/imgui_impl_glfw.cpp)

# create executable

add_executable(MatrCalc ${SOURCES}
						${IMGUI_SOURCES})

# add paths to header files

target_include_directories(MatrCalc PRIVATE ./header/controller
									 		./header/model
									 		./header/view
										 	../imgui
									 		../imgui/backends)

# link with glfw and opengl

target_link_libraries(MatrCalc glfw
							   ${OPENGL_LIBRARIES})
