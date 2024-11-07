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

set(Game_MY_DEBUG 1)     # 1 true, 0 false   =>  use if(Game_MY_DEBUG EQUAL 1) - true; if(NOT Game_MY_DEBUG) - false

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
set(ZC_AUDIO ON)
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

    ${PROJECT_SOURCE_DIR}/src/System/G_System.cpp
    ${PROJECT_SOURCE_DIR}/src/System/G_Config.cpp
    ${PROJECT_SOURCE_DIR}/src/System/G_DrawerCreator.cpp
    ${PROJECT_SOURCE_DIR}/src/System/G_Time.cpp
    ${PROJECT_SOURCE_DIR}/src/System/G_Func.cpp
    ${PROJECT_SOURCE_DIR}/src/System/G_FontData.cpp
    ${PROJECT_SOURCE_DIR}/src/System/G_LightUBO.cpp
    
    ${PROJECT_SOURCE_DIR}/src/Sound/G_GameSound.cpp
    ${PROJECT_SOURCE_DIR}/src/Sound/G_GameSoundSet.cpp
    ${PROJECT_SOURCE_DIR}/src/Sound/G_SystemSound.cpp
    ${PROJECT_SOURCE_DIR}/src/Sound/G_GameSounds.cpp
    
    ${PROJECT_SOURCE_DIR}/src/GUI/G_GUI_W_MainMenu.cpp
    ${PROJECT_SOURCE_DIR}/src/GUI/G_GUI_W_Options.cpp
    ${PROJECT_SOURCE_DIR}/src/GUI/G_GUI_W_PlayerWin.cpp
    ${PROJECT_SOURCE_DIR}/src/GUI/G_GUI_W_RestartLevel.cpp
    ${PROJECT_SOURCE_DIR}/src/GUI/G_GUI.cpp
    ${PROJECT_SOURCE_DIR}/src/GUI/Text/G_GUI_Text.cpp
    ${PROJECT_SOURCE_DIR}/src/GUI/Text/G_LanguageDependent.cpp
    ${PROJECT_SOURCE_DIR}/src/GUI/Text/G_GUI_Text.cpp
    ${PROJECT_SOURCE_DIR}/src/GUI/Text/G_LangText.cpp
    ${PROJECT_SOURCE_DIR}/src/GUI/Text/G_ZC_TextSceneTurnedToCameraLevel.cpp
    ${PROJECT_SOURCE_DIR}/src/GUI/Buttons/G_GUI_ButtonText.cpp
    ${PROJECT_SOURCE_DIR}/src/GUI/Buttons/G_GUI_CheckBox.cpp
    ${PROJECT_SOURCE_DIR}/src/GUI/Buttons/G_GUI_Button.cpp
    ${PROJECT_SOURCE_DIR}/src/GUI/Buttons/G_GUI_ButtonInt32Text.cpp
    ${PROJECT_SOURCE_DIR}/src/GUI/Config/G_GUI_W_ConfigSaveLevel.cpp
    
    ${PROJECT_SOURCE_DIR}/src/GamePlay/GUI/G_GUI_GM_W_LevelTimer.cpp
    ${PROJECT_SOURCE_DIR}/src/GamePlay/GUI/G_GUI_P_W_Health.cpp
    ${PROJECT_SOURCE_DIR}/src/GamePlay/GUI/G_GUI_GM_LevelNumber.cpp
    ${PROJECT_SOURCE_DIR}/src/GamePlay/GUI/G_GUI_GM_StartTimer.cpp
    ${PROJECT_SOURCE_DIR}/src/GamePlay/G_GameManager.cpp
    ${PROJECT_SOURCE_DIR}/src/GamePlay/G_Player.cpp
    ${PROJECT_SOURCE_DIR}/src/GamePlay/G_Map.cpp
    ${PROJECT_SOURCE_DIR}/src/GamePlay/G_Section.cpp
    ${PROJECT_SOURCE_DIR}/src/GamePlay/G_Camera.cpp

    ${PROJECT_SOURCE_DIR}/src/Objects/G_Object.cpp
    ${PROJECT_SOURCE_DIR}/src/Objects/G_ObjPlayable.cpp    
    ${PROJECT_SOURCE_DIR}/src/Objects/G_OP_MarbleSphere.cpp
    ${PROJECT_SOURCE_DIR}/src/Objects/G_PushSet.cpp
    ${PROJECT_SOURCE_DIR}/src/Objects/G_Star.cpp
    ${PROJECT_SOURCE_DIR}/src/Objects/Platforms/G_PlatformLightHolder.cpp
    ${PROJECT_SOURCE_DIR}/src/Objects/Platforms/G_Platform.cpp
    ${PROJECT_SOURCE_DIR}/src/Objects/Platforms/G_PlatformWind.cpp
    ${PROJECT_SOURCE_DIR}/src/Objects/Platforms/G_PlatformDamage.cpp
    ${PROJECT_SOURCE_DIR}/src/Objects/Platforms/G_PlatformScale.cpp
    ${PROJECT_SOURCE_DIR}/src/Objects/Platforms/G_PlatformDisappear.cpp
    ${PROJECT_SOURCE_DIR}/src/Objects/Platforms/G_PlatformTransforms.cpp
    ${PROJECT_SOURCE_DIR}/src/Objects/Platforms/G_PlatformWin.cpp
    ${PROJECT_SOURCE_DIR}/src/Objects/Particles/G_ParticlesDrawer.cpp
    ${PROJECT_SOURCE_DIR}/src/Objects/Particles/G_PS_Wind.cpp
    ${PROJECT_SOURCE_DIR}/src/Objects/Particles/G_ParticleMoveCircle.cpp
    ${PROJECT_SOURCE_DIR}/src/Objects/Particles/G_PS_Star.cpp

    ${PROJECT_SOURCE_DIR}/src/Model/G_Assimp_ZC_Converter.cpp
    ${PROJECT_SOURCE_DIR}/src/Model/G_ModelLoader.cpp
    ${PROJECT_SOURCE_DIR}/src/Model/G_ModelSet.cpp
    ${PROJECT_SOURCE_DIR}/src/Model/G_Models.cpp
    ${PROJECT_SOURCE_DIR}/src/Model/G_CubeModelCreator.cpp
    ${PROJECT_SOURCE_DIR}/src/Model/G_CubeMap.cpp
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
	PRIVATE	
			# $<$<CONFIG:Debug>:$<IF:$<BOOL:${WIN32}>,-W3,-Wall>>
			$<$<CONFIG:Release>:-O3>
    )

#========================= copy assets
add_custom_target(ZCR_COPY_ASSETS ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/${Game_ASSETS_DIRECTORY_NAME} ${Game_ASSETS_DESTINATION_PATH}
    COMMENT "Copy assets ${PROJECT_SOURCE_DIR}/${Game_ASSETS_DIRECTORY_NAME} => ${Game_ASSETS_DESTINATION_PATH}")
add_dependencies(${PROJECT_NAME} ZCR_COPY_ASSETS)


message(STATUS "======================== Game END ========================")