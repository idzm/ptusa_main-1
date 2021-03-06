cmake_minimum_required(VERSION 3.13)

#set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(LINUX)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()

project(ptusa)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

################# create targets ######################################################

add_definitions(-DPAC_PC)
add_definitions(-DDEBUG_NO_IO_MODULES)

if(LINUX)
    add_definitions(-DLINUX_OS)
elseif(WIN32)
    add_definitions(-DWIN_OS)
    add_definitions(-DSIMPLE_LOG)
endif()

add_definitions(-DTEST_SPEED)

if(MSVC)
    add_compile_options("/utf-8")
endif(MSVC)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lua ${CMAKE_CURRENT_SOURCE_DIR}/bin/lua EXCLUDE_FROM_ALL)

set(LUA_LIBRARIES liblua)
set(LUA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lua/src;${CMAKE_CURRENT_SOURCE_DIR}/bin/lua)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tolua++ ${CMAKE_CURRENT_SOURCE_DIR}/bin/tolua++ EXCLUDE_FROM_ALL)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/zlib ${CMAKE_CURRENT_SOURCE_DIR}/bin/zlib EXCLUDE_FROM_ALL)

file(GLOB common_src CONFIGURE_DEPENDS
        common/PAC-driver/*.cpp
        common/util/*.c
        common/util/*.cpp
        common/*.cpp

        PAC/common/*.cpp
        PAC/common/sys/*.cpp
        PAC/IoT/*.cpp)

list(FILTER common_src EXCLUDE REGEX ".*OPCUA*|.*rfid*|snprintf\\.c")

if(LINUX)
    file(GLOB linux_src CONFIGURE_DEPENDS
            PAC/linux/*.cpp
            PAC/linux/PAC_PC/*.cpp)
endif()

if(WIN32)
    file(GLOB win_src CONFIGURE_DEPENDS
            common/win_util/*.cpp

            PAC/win/*.cpp
            PAC/win/PAC_PC/*.cpp)
endif()

if(LINUX)
    add_executable(${PROJECT_NAME} ${common_src} ${linux_src}
            PAC_control_projects/main_control_prg/main.cpp)

    add_library(main_control ${common_src} ${linux_src}
            PAC_control_projects/main_control_prg/main_control_dll_vc/main_control_dll_vc.cpp)
elseif(WIN32)
    add_executable(${PROJECT_NAME} ${common_src} ${win_src}
            PAC_control_projects/main_control_prg/main.cpp)

    add_library(main_control ${common_src} ${win_src}
            PAC_control_projects/main_control_prg/main_control_dll_vc/main_control_dll_vc.cpp
            PAC_control_projects/main_control_prg/main_control_dll_vc/dllmain.cpp)
endif()

target_compile_definitions(main_control PUBLIC _USRDLL)
#######################################################################################

################# project include-paths ###############################################

set(GENERAL_INCLUDES
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common/util>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common/PAC-driver>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/common>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/common/sys>

if (WIN32)
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common/win_util>

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/win>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/win/PAC_PC>
elseif (LINUX)
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/linux>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/linux/PAC_PC>
endif ()

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/IoT>

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tolua++/include>

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lua/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/bin/lua>

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/zlib>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/bin/zlib>)

target_include_directories(${PROJECT_NAME} PUBLIC ${GENERAL_INCLUDES})
target_include_directories(main_control PUBLIC ${GENERAL_INCLUDES})
#######################################################################################

################# set link options ####################################################
# WARNING: Without --no-undefined the linker will not check, whether all necessary    #
#          libraries are linked. When a library which is necessary is not linked,     #
#          the firmware will crash and there will be NO indication why it crashed.    #
#######################################################################################
if(LINUX)
    target_link_options(${PROJECT_NAME} PRIVATE LINKER:--no-undefined)
endif()
#######################################################################################

################# add link targets ####################################################

target_link_libraries(${PROJECT_NAME} liblua_static toluapp_lib_static zlibstatic)
target_link_libraries(main_control toluapp_lib_static zlibstatic)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
    target_link_libraries(main_control wsock32 ws2_32)

    if(DEFINED ENV{LUA_DEV})
        target_link_libraries(main_control $ENV{LUA_DEV}/lib/lua51.lib)
    else()
        target_link_libraries(main_control liblua)
    endif()

elseif (LINUX)
    target_link_libraries(main_control liblua_static)
endif()
#######################################################################################

install_executable(${PROJECT_NAME})
install_library(main_control)

enable_testing()
add_test(run_no_command_line_arguments ${PROJECT_NAME})
#remove tests from add_subdirectory() added projects
configure_file(${CMAKE_SOURCE_DIR}/CTestCustom.cmake ${CMAKE_BINARY_DIR} @ONLY)
