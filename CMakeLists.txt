cmake_minimum_required(VERSION 3.14)
project(OrganMarketplace)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCES main.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

if (EMSCRIPTEN)
    # WebAssembly-specific configuration
    message(STATUS "Configuring for WebAssembly/Emscripten")

    # Force the output to be an HTML file
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")

		# Fetch SDL3 and SDL3_image directly from source
    include(FetchContent)
		FetchContent_Declare(
			SDL3
			GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
			GIT_TAG main
		)
		set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    set(SDL_TESTS OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(SDL3)

    FetchContent_Declare(
        SDL3_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG main
    )
    # Turn off things we don't need to speed up compilation
    set(SDLIMAGE_SAMPLES OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_TESTS OFF CACHE BOOL "" FORCE)
		set(SDLIMAGE_BACKEND_STB ON CACHE BOOL "" FORCE)
		set(SDLIMAGE_PNG ON CACHE BOOL "" FORCE) # Disables external libpng dependency
    FetchContent_MakeAvailable(SDL3_image)

		# target_compile_options(${PROJECT_NAME} PRIVATE "-sUSE_SDL=3")
    target_link_options(${PROJECT_NAME} PRIVATE
        "-sMAX_WEBGL_VERSION=2"
        "-sMIN_WEBGL_VERSION=1"
        "-sDEFAULT_TO_CXX"
        "--preload-file" "${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets" # Uncomment this later when you add sprite folders
        "--shell-file" "${CMAKE_CURRENT_SOURCE_DIR}/shell.html"
    )

		target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)
else()
    message(STATUS "Configuring for Native Desktop")

    find_package(SDL3 REQUIRED)
    find_package(SDL3_image REQUIRED)

    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL3_INCLUDE_DIRS} ${SDL3_IMAGE_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} SDL3::SDL3 SDL3_image::SDL3_image)

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        ${CMAKE_CURRENT_BINARY_DIR}/assets
    )
endif()
