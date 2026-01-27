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
    
    message(STATUS "Added project: ${PROJECT_NAME}")
endfunction()

# Add your projects here
add_opengl_project(bulldozer main.cpp)
add_opengl_project(shelf main.cpp)
add_opengl_project(aeroplane main.cpp)

# Add more projects as needed:
# add_opengl_project(myproject main.cpp)