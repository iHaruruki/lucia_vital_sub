cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)

project(lucia_vital_sub)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(YARP REQUIRED)
find_package(OpenCV REQUIRED)

# find dependencies
find_package(ament_cmake REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

include_directories(lucia_vital_sub
  ${YARP_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
)

add_executable(lucia_vital_sub src/lucia_vital_sub.cpp src/ARM_CONTROLL.cpp include/lucia_vital_sub/ARM_CONTROLL.h)
ament_target_dependencies(lucia_vital_sub
  rclcpp
  std_msgs
)

target_link_libraries(lucia_vital_sub
  ${YARP_LIBRARIES}
  ${OpenCV_LIBRARIES}
)

install(TARGETS
  lucia_vital_sub
  DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
