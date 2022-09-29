cmake_minimum_required(VERSION 3.5)
project(playgame)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(BUILD_STATIC FALSE CACHE STRING "Set this to link external libraries statically")

# find dependencies
find_package(ament_cmake_auto REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(example_interfaces REQUIRED)

# ament_auto_find_build_dependencies()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/;${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}")
message(">>>camke_module_path = ${CMAKE_MODULE_PATH}")
# Set static if BUILD_STATIC is set
if (BUILD_STATIC)
    set(SFML_STATIC_LIBRARIES TRUE)
    # Link libgcc and libstc++ statically as well
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")
    endif()
endif()

find_package(SFML 2 COMPONENTS audio graphics window system REQUIRED)
message("\n>>>${SFML_DIR}")

message("\n>>>sfml_include_path= ${SFML_INCLUDE_DIR}")


include_directories(${SFML_INCLUDE_DIR})
include_directories("${PROJECT_SOURCE_DIR}/include/*.h")

# Add sources
file(GLOB SOURCES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # uncomment the line when a copyright and license is not present in all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # uncomment the line when this package is not in a git repo
  #set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_auto_add_executable(PLay ${SOURCES})

target_link_libraries(PLay ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})

ament_auto_package()
