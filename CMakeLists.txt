cmake_minimum_required (VERSION 2.6)
project (openGL_demo)

include_directories (${PROJECT_SOURCE_DIR}/include)

add_subdirectory (${PROJECT_SOURCE_DIR}/lib)
add_subdirectory (${PROJECT_SOURCE_DIR}/module)

set (EXTRA_LIBS ${EXTRA_LIBS} ShaderLoader
							  EventProcessor
                              glad
							  SDL2
                              dl
                              m)

add_executable (openGL_demo ${PROJECT_SOURCE_DIR}/src/main.cpp)

target_link_libraries (openGL_demo ${EXTRA_LIBS})
file (REMOVE_RECURSE ${PROJECT_SOURCE_DIR}/build/shaders)
file (COPY ${PROJECT_SOURCE_DIR}/src/shaders DESTINATION .)
