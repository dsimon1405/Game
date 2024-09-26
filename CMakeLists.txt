cmake_minimum_required(VERSION 3.22)
project(Game)

message(STATUS "======================= Game START =======================")

#========================= compiler
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#========================= assets path
set(Game_ASSETS_DIRECTORY_NAME assets)
set(Game_ASSETS_DESTINATION_PATH 
    ${PROJECT_BINARY_DIR}/$<$<PLATFORM_ID:Windows>:$<IF:$<CONFIG:Debug>,Debug/,Release/>>${Game_ASSETS_DIRECTORY_NAME})   #   windows?

set(Game_MY_DEBUG 0)     # 1 true, 0 false   =>  use if(Game_MY_DEBUG EQUAL 1) - true; if(NOT Game_MY_DEBUG) - false

#========================= external libs
#   add submodules
if(NOT Game_MY_DEBUG)
    add_subdirectory(cmakeGitSubmodules)
endif()

set(Game_INCLUDE_DIRS)
set(Game_LINK_LIBS)

    #   ZeroCore
set(ZC_PC ON)
set(ZC_ANDROID OFF)
set(ZC_PARANT_BIN_DIR ${Game_ASSETS_DESTINATION_PATH})
set(ZC_AUDIO OFF)
set(ZC_IMGUI OFF)
set(ZC_STATIC ON)
# set(ZC_STATIC OFF)
if(Game_MY_DEBUG EQUAL 1)
    set(ZC_MY_DEBUG 1)
    add_subdirectory(ZeroCore)
else()
    set(ZC_MY_DEBUG 0)
    add_subdirectory(external/ZeroCore)
endif()
set(Game_INCLUDE_DIRS ${Game_INCLUDE_DIRS} PUBLIC $<IF:${Game_MY_DEBUG},${PROJECT_SOURCE_DIR}/ZeroCore/include,${PROJECT_SOURCE_DIR}/external/ZeroCore/include>)
set(Game_LINK_LIBS ${Game_LINK_LIBS} PUBLIC ZeroCore)

    #   assimp
if(Game_MY_DEBUG EQUAL 1)
    set(Game_INCLUDE_DIRS ${Game_INCLUDE_DIRS} PUBLIC /home/dmitry/projects/external/assimp/include)
else()
        #   set up
    set(BUILD_SHARED_LIBS OFF)
    set(ASSIMP_BUILD_TESTS OFF)
    set(ASSIMP_WARNINGS_AS_ERRORS OFF)
        #   add subdiractory
    add_subdirectory(external/assimp)
        #   include
    set(Game_INCLUDE_DIRS ${Game_INCLUDE_DIRS}
        PUBLIC ${PROJECT_SOURCE_DIR}/external/assimp/include
        PUBLIC ${PROJECT_BINARY_DIR}/external/assimp/include)
endif()
set(Game_LINK_LIBS ${Game_LINK_LIBS} PUBLIC $<IF:${Game_MY_DEBUG},/home/dmitry/projects/external/assimp/lib/libassimp.so,assimp>)

#========================= ZERO CREATOR =========================

#========================= sources
set(Game_SRC_FILES
    ${PROJECT_SOURCE_DIR}/src/main.cpp
    ${PROJECT_SOURCE_DIR}/src/G_Camera.cpp
    ${PROJECT_SOURCE_DIR}/src/G_GameManager.cpp
    ${PROJECT_SOURCE_DIR}/src/Objects/G_Object.cpp
    ${PROJECT_SOURCE_DIR}/src/Objects/G_Platform.cpp
    ${PROJECT_SOURCE_DIR}/src/Objects/G_Player.cpp

    ${PROJECT_SOURCE_DIR}/src/Model/G_Assimp_ZC_Converter.cpp
    ${PROJECT_SOURCE_DIR}/src/Model/G_ModelLoader.cpp
    ${PROJECT_SOURCE_DIR}/src/Model/G_ModelSet.cpp
    ${PROJECT_SOURCE_DIR}/src/Model/G_Models.cpp
    ${PROJECT_SOURCE_DIR}/src/Model/G_CubeModelCreator.cpp
    )
    
add_executable(${PROJECT_NAME} ${Game_SRC_FILES})

#========================= include dirs
target_include_directories(${PROJECT_NAME} ${Game_INCLUDE_DIRS}
    PRIVATE ${PROJECT_SOURCE_DIR}/src
    # PUBLIC /home/dmitry/projects/external/glm/include
    )

#========================= link libs
target_link_libraries(${PROJECT_NAME} ${Game_LINK_LIBS}
    )

#========================= compiler params
target_compile_options(${PROJECT_NAME}
    PRIVATE	$<IF:$<BOOL:${WIN32}>,-W3,-Wall>
            $<$<CONFIG:Release>:-O3>
    )

#========================= copy assets
add_custom_target(ZCR_COPY_ASSETS ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/${Game_ASSETS_DIRECTORY_NAME} ${Game_ASSETS_DESTINATION_PATH}
    COMMENT "Copy assets ${PROJECT_SOURCE_DIR}/${Game_ASSETS_DIRECTORY_NAME} => ${Game_ASSETS_DESTINATION_PATH}")
add_dependencies(${PROJECT_NAME} ZCR_COPY_ASSETS)


message(STATUS "======================== Game END ========================")