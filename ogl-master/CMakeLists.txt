# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (MinecraftPROTO)

find_package(OpenGL REQUIRED)


if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()



# Compile external dependencies 
add_subdirectory (external)

# On Visual 2005 and above, this module can set the debug working directory
cmake_policy(SET CMP0026 OLD)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-fe2273")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP

if(INCLUDE_DISTRIB)
	add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)



include_directories(
	external/glfw-3.1.2/include/GLFW/
	external/glm-0.9.7.1/
	external/glew-1.13.0/include/
	external/enet-1.3.17/include/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_1130
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

# Tutorial 7
add_executable(tutorial07_model_loading
	tutorial07_model_loading/Scripts/main.cpp
	
	tutorial07_model_loading/Scripts/Action.h
	tutorial07_model_loading/Scripts/Block.cpp
	tutorial07_model_loading/Scripts/Block.h
	tutorial07_model_loading/Scripts/Block_Datas.h
	tutorial07_model_loading/Scripts/Block_Render.cpp
	tutorial07_model_loading/Scripts/Block_Render.h
	tutorial07_model_loading/Scripts/Block_Render_Type.h
	tutorial07_model_loading/Scripts/Block_Shape_Data.cpp
	tutorial07_model_loading/Scripts/Block_Shape_Data.h
	tutorial07_model_loading/Scripts/Block_Shapes_Type.h
	tutorial07_model_loading/Scripts/Block_Type.h
	tutorial07_model_loading/Scripts/Blocks_Global_Datas.cpp
	tutorial07_model_loading/Scripts/Blocks_Global_Datas.h
	tutorial07_model_loading/Scripts/Blocks_Global_Shapes.cpp
	tutorial07_model_loading/Scripts/Blocks_Global_Shapes.h
	tutorial07_model_loading/Scripts/Chunk.cpp
	tutorial07_model_loading/Scripts/Chunk.h
	tutorial07_model_loading/Scripts/Chunk_Data.cpp
	tutorial07_model_loading/Scripts/Chunk_Data.h
	tutorial07_model_loading/Scripts/Chunk_Data_Generator.cpp
	tutorial07_model_loading/Scripts/Chunk_Data_Generator.h
	tutorial07_model_loading/Scripts/Chunk_Render.cpp
	tutorial07_model_loading/Scripts/Chunk_Render.h
	tutorial07_model_loading/Scripts/Chunk_Render_Generator.cpp
	tutorial07_model_loading/Scripts/Chunk_Render_Generator.h
	tutorial07_model_loading/Scripts/Chunk_SideData.cpp
	tutorial07_model_loading/Scripts/Chunk_SideData.h
	tutorial07_model_loading/Scripts/Chunks_Manager.cpp
	tutorial07_model_loading/Scripts/Chunks_Manager.h
	tutorial07_model_loading/Scripts/Debug_World.cpp
	tutorial07_model_loading/Scripts/Debug_World.h
	tutorial07_model_loading/Scripts/GlobalDefine.h
	tutorial07_model_loading/Scripts/lodepng.cpp
	tutorial07_model_loading/Scripts/lodepng.h
	tutorial07_model_loading/Scripts/MainGame.cpp
	tutorial07_model_loading/Scripts/MainGame.h
	tutorial07_model_loading/Scripts/PerlinNoise.cpp
	tutorial07_model_loading/Scripts/PerlinNoise.h
	tutorial07_model_loading/Scripts/Shaders_Manager.cpp
	tutorial07_model_loading/Scripts/Shaders_Manager.h
	tutorial07_model_loading/Scripts/Side.h
	tutorial07_model_loading/Scripts/TextureLoader.cpp
	tutorial07_model_loading/Scripts/TextureLoader.h
	tutorial07_model_loading/Scripts/Thread.cpp
	tutorial07_model_loading/Scripts/Thread.h
	tutorial07_model_loading/Scripts/Thread_Manager.cpp
	tutorial07_model_loading/Scripts/Thread_Manager.h
	tutorial07_model_loading/Scripts/Thread_Structs.h
	tutorial07_model_loading/Scripts/TODO.h
	tutorial07_model_loading/Scripts/World.cpp
	tutorial07_model_loading/Scripts/World.h
	
	common/shader.cpp
	common/shader.hpp
	common/controls.cpp
	common/controls.hpp
	common/texture.cpp
	common/texture.hpp
	common/objloader.cpp
	common/objloader.hpp

	tutorial07_model_loading/TransformVertexShader.vertexshader
	tutorial07_model_loading/TextureFragmentShader.fragmentshader
)
target_link_libraries(tutorial07_model_loading
	${ALL_LIBS}
)
# Xcode and Visual working directories
set_target_properties(tutorial07_model_loading PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tutorial07_model_loading/")
create_target_launcher(tutorial07_model_loading WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tutorial07_model_loading/")

SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )
SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*shader$" )


if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

add_custom_command(
   TARGET tutorial07_model_loading POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/tutorial07_model_loading${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/tutorial07_model_loading/"
)

elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

