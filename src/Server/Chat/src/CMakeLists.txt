# CMakeList.txt: Chat에 대한 CMake 프로젝트, 여기에 소스를 포함하고
# 프로젝트 특정 논리를 정의합니다.
#

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)
set(Boost_USE_STATIC_RUNTIME OFF)

set(GLOBAL_VENDOR_DIR "${CMAKE_SOURCE_DIR}/../../../vendor")
set(LOCAL_VENDOR_DIR "${CMAKE_SOURCE_DIR}/../vendor")

find_package(Boost 1.81.0 REQUIRED COMPONENTS system)
find_package(TBB 2021.7.0 REQUIRED)
find_path(TBB_INCLUDE_DIRS TBB)

# 이 프로젝트의 실행 파일에 소스를 추가합니다.
add_executable(Chat "Application.cpp" "Application.h" "EntryPoint.cpp" "Server.cpp" "Server.h" "Room.cpp" "Room.h" "Player.h" "Player.cpp" "Session.h" "Session.cpp")
target_precompile_headers(Chat PRIVATE "pch.h")

if ("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
	target_compile_definitions(Chat PRIVATE DEBUG_)
elseif("${CMAKE_BUILD_TYPE}" MATCHES "Release")
	target_compile_definitions(Chat PRIVATE RELEASE_)
endif()

target_include_directories(Chat PRIVATE "${GLOBAL_VENDOR_DIR}/include")

if (Boost_FOUND)
	target_include_directories(Chat PRIVATE ${Boost_INCLUDE_DIRS})
endif()

if (TBB_FOUND)
	target_include_directories(Chat PRIVATE ${TBB_INCLUDE_DIRS})
	target_link_libraries(Chat PRIVATE TBB::tbb)
endif()

target_sources(Chat PRIVATE "${GLOBAL_VENDOR_DIR}/include/Packet.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Chat PROPERTY CXX_STANDARD 20)
endif()

target_compile_definitions(Chat PRIVATE -D_UNICODE -DUNICODE)

# TODO: 필요한 경우 테스트를 추가하고 대상을 설치합니다.
