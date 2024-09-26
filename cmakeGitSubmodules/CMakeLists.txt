

#	fatal error equal function
function(F_FATAL_EQUAL data1 data2 errorMessage)
	if(NOT ${data1} EQUAL ${data2})
		message(FATAL_ERROR ${errorMessage})
	endif()
endfunction()

#   find git package
find_package(Git QUIET)
if(NOT GIT_FOUND)
    message(FATAL_ERROR "Could not find_package(Git QUIET)!")
endif()

#	git init
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/.git")
	message(STATUS "Inits Git for submodules download")
	execute_process(COMMAND ${GIT_EXECUTABLE} init
                    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                    RESULT_VARIABLE process_result)
	F_FATAL_EQUAL(${process_result} 0 "git init => failed with: ${process_result}")
endif()

function(F_TRY_CLONE_ASSIMP)
	execute_process(COMMAND ${GIT_EXECUTABLE} clone "https://github.com/assimp/assimp.git"
					WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/external
					RESULT_VARIABLE process_result)

	if(NOT ${process_result} EQUAL 0)
		F_TRY_CLONE_ASSIMP()	#	recurcive call while not load assimp
	else()
		#	checkout
		message(STATUS "Git checkout assimp to 97257a3")
		execute_process(COMMAND ${GIT_EXECUTABLE} checkout "97257a3"
						WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/external/assimp
						RESULT_VARIABLE process_result)
		F_FATAL_EQUAL(${process_result} 0 "git checkout assimp 97257a3 => failed with: ${process_result}")
	endif()
endfunction()

# F_TRY_CLONE_ASSIMP()	#start trying to clone assimp

#	git submodule update --init --recursive (updates(clone from remote repository) all submodules and their submodules(--recursive),
#	and makes (checkout) for each submodule, to commit saved in git).
message(STATUS "Git submodule update --init --recursive")
execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                RESULT_VARIABLE process_result)
F_FATAL_EQUAL(${process_result} 0 "git submodule update --init --recursive => failed with: ${process_result}")

#	git clone assimp recursive while cloned
if (NOT EXISTS "${PROJECT_SOURCE_DIR}/external/assimp/CMakeLists.txt")
	message(STATUS "Git clone assimp")
	F_TRY_CLONE_ASSIMP()
endif()


#	DELETE ON RELEASE AND ADD ZeroCore submodule last stabel commit!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
execute_process(COMMAND ${GIT_EXECUTABLE} fetch		#	update ZeroCore to last commit in repository
				WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/external/ZeroCore
                RESULT_VARIABLE process_result)
if (NOT ${process_result} EQUAL 0)	# if not last commit, make merge to last 
	execute_process(COMMAND ${GIT_EXECUTABLE} merge origin main		#	update ZeroCore to last commit in repository
					WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/external/ZeroCore
					RESULT_VARIABLE process_result)
	F_FATAL_EQUAL(${process_result} 0 "git merge origin/main, for (.../external/ZeroCore) => failed with: ${process_result}")
endif()