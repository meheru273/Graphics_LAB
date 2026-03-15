cmake_minimum_required(VERSION 3.21)
project(OpenGLProjects LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages (from MSYS2)
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# Define paths to shared resources
set(OPENGL_DIR ${CMAKE_SOURCE_DIR}/opengl)
set(SHADERS_DIR ${CMAKE_SOURCE_DIR}/shaders)

# Add include directories (global for all projects)
include_directories(
    ${OPENGL_DIR}/include
)

# Function to create a project
function(add_opengl_project PROJECT_NAME SOURCE_FILE)
    # Add executable
    add_executable(${PROJECT_NAME}
        projects/${PROJECT_NAME}/${SOURCE_FILE}
        ${OPENGL_DIR}/glad.c
    )
    
    # Add project-specific include directory
    target_include_directories(${PROJECT_NAME} PRIVATE 
        ${CMAKE_SOURCE_DIR}/projects/${PROJECT_NAME}
    )
    
    # Link libraries
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        OpenGL::GL 
        glfw 
        glm::glm
    )
    
    # Set output directory to project's build folder
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/projects/${PROJECT_NAME}
    )
    
    # Copy project-specific shaders if they exist
    if(EXISTS ${CMAKE_SOURCE_DIR}/projects/${PROJECT_NAME}/shaders)
        file(GLOB PROJECT_SHADERS ${CMAKE_SOURCE_DIR}/projects/${PROJECT_NAME}/shaders/*.vs 
                                   ${CMAKE_SOURCE_DIR}/projects/${PROJECT_NAME}/shaders/*.fs)
        foreach(SHADER ${PROJECT_SHADERS})
            get_filename_component(SHADER_NAME ${SHADER} NAME)
            configure_file(${SHADER} 
                          ${CMAKE_BINARY_DIR}/projects/${PROJECT_NAME}/shaders/${SHADER_NAME} 
                          COPYONLY)
        endforeach()
    endif()
    
    # Copy shaders from project root directory if they exist there
    file(GLOB PROJECT_ROOT_SHADERS ${CMAKE_SOURCE_DIR}/projects/${PROJECT_NAME}/*.vs 
                                    ${CMAKE_SOURCE_DIR}/projects/${PROJECT_NAME}/*.fs)
    foreach(SHADER ${PROJECT_ROOT_SHADERS})
        get_filename_component(SHADER_NAME ${SHADER} NAME)
        configure_file(${SHADER} 
                      ${CMAKE_BINARY_DIR}/projects/${PROJECT_NAME}/${SHADER_NAME} 
                      COPYONLY)
    endforeach()
    
    message(STATUS "Added project: ${PROJECT_NAME}")
endfunction()

# Add your projects here
add_opengl_project(bulldozer main.cpp)
add_opengl_project(shelf main.cpp)
add_opengl_project(aeroplane main.cpp)
# Village fair project (needs image_loader.cpp for STB_IMAGE_IMPLEMENTATION)
add_executable(village_fair
    projects/village_fair/main.cpp
    projects/village_fair/image_loader.cpp
    ${OPENGL_DIR}/glad.c
)

target_include_directories(village_fair PRIVATE
    ${CMAKE_SOURCE_DIR}/projects/village_fair
    ${CMAKE_SOURCE_DIR}/projects/village_fair/include
)

target_link_libraries(village_fair PRIVATE
    OpenGL::GL
    glfw
    glm::glm
)

set_target_properties(village_fair PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/projects/village_fair
)

# Copy shaders
file(GLOB VF_SHADERS ${CMAKE_SOURCE_DIR}/projects/village_fair/shaders/*.vs
                      ${CMAKE_SOURCE_DIR}/projects/village_fair/shaders/*.fs
                      ${CMAKE_SOURCE_DIR}/projects/village_fair/shaders/*.vert
                      ${CMAKE_SOURCE_DIR}/projects/village_fair/shaders/*.frag)
foreach(SHADER ${VF_SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    configure_file(${SHADER}
                  ${CMAKE_BINARY_DIR}/projects/village_fair/shaders/${SHADER_NAME}
                  COPYONLY)
endforeach()

# Copy point data files
file(GLOB VF_POINTS ${CMAKE_SOURCE_DIR}/projects/village_fair/points/*.txt)
foreach(POINT ${VF_POINTS})
    get_filename_component(POINT_NAME ${POINT} NAME)
    configure_file(${POINT}
                  ${CMAKE_BINARY_DIR}/projects/village_fair/points/${POINT_NAME}
                  COPYONLY)
endforeach()

# Copy textures
file(GLOB VF_TEXTURES ${CMAKE_SOURCE_DIR}/projects/village_fair/textures/*.png
                       ${CMAKE_SOURCE_DIR}/projects/village_fair/textures/*.jpg
                       ${CMAKE_SOURCE_DIR}/projects/village_fair/textures/*.jpeg
                       ${CMAKE_SOURCE_DIR}/projects/village_fair/textures/*.PNG
                       ${CMAKE_SOURCE_DIR}/projects/village_fair/textures/*.JPG)
foreach(TEX ${VF_TEXTURES})
    get_filename_component(TEX_NAME ${TEX} NAME)
    configure_file(${TEX}
                  ${CMAKE_BINARY_DIR}/projects/village_fair/textures/${TEX_NAME}
                  COPYONLY)
endforeach()

message(STATUS "Added project: village_fair")

# Manually add 'codes' project if it exists
if(EXISTS ${CMAKE_SOURCE_DIR}/projects/codes/main.cpp)
add_executable(codes
    projects/codes/main.cpp
    projects/codes/stb_image.cpp
    ${OPENGL_DIR}/glad.c
)

target_include_directories(codes PRIVATE 
    ${CMAKE_SOURCE_DIR}/projects/codes
)

target_link_libraries(codes PRIVATE 
    OpenGL::GL 
    glfw 
    glm::glm
)

set_target_properties(codes PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/projects/codes
)

# Copy shaders for codes project
file(GLOB CODES_SHADERS ${CMAKE_SOURCE_DIR}/projects/codes/*.vs 
                        ${CMAKE_SOURCE_DIR}/projects/codes/*.fs)
foreach(SHADER ${CODES_SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    configure_file(${SHADER} 
                  ${CMAKE_BINARY_DIR}/projects/codes/${SHADER_NAME} 
                  COPYONLY)
endforeach()

# Copy texture image for codes project
if(EXISTS ${CMAKE_SOURCE_DIR}/projects/codes/color.jpg)
    configure_file(${CMAKE_SOURCE_DIR}/projects/codes/color.jpg
                  ${CMAKE_BINARY_DIR}/projects/codes/color.jpg
                  COPYONLY)
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/projects/codes/birds.jpg)
    configure_file(${CMAKE_SOURCE_DIR}/projects/codes/birds.jpg
                  ${CMAKE_BINARY_DIR}/projects/codes/birds.jpg
                  COPYONLY)
endif()

message(STATUS "Added project: codes")
endif()