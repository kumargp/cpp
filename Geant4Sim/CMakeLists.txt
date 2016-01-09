#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(Sim)

# The version number.
set(${PROJECT_NAME}_VERSION "1.0")
#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

# make sure that the default is a RELEASE
if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release."
      FORCE)
endif (NOT CMAKE_BUILD_TYPE)
# or pass this comman line -DCMAKE_BUILD_TYPE=Debug

#----------------------------------------------------------------------------
#valgrind --leak-check=yes --log-file=out ./Sim run.mac  AFTER enablin the -g -00 ..

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall ") #-g -00 GLIBCXX_FORCE_NEW -fno-inline")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wextra -Winline -Wmissing-include-dirs")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wuninitialized -Wvla -Winit-self -Wshadow")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdisabled-optimization") 
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunsafe-loop-optimizations -Wsystem-headers -Wcast-qual")
endif()

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" SUPPORTS_CXX0X)
if(SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    MESSAGE("====> using C++11")
elseif(SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	MESSAGE("====> using C++0x")
endif()

#----------------------------------------------------------------------------
#boost
#get help command: cmake --help-module FindBoost
# in bashrc export BOOST_ROOT="/home/gopan/Programs/Boost"
#SET(Boost_USE_MULTITHREAD ON) #set a flag
#FIND_PACKAGE(Boost)
#INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} ${Boost_INCLUDE_DIRS})
#LINK_DIRECTORIES(${LINK_DIRECTORIES} ${Boost_LIBRARY_DIRS})

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})


#----------------------------------------------------------------------------
# Find ROOT (optional package)
#
find_package(ROOT)
if(ROOT_FOUND)
  add_definitions(-DG4ANALYSIS_USE_ROOT)
endif()

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include 
                    ${PROJECT_SOURCE_DIR}/SNS/include
					#${PROJECT_SOURCE_DIR}/CosI/include
                    ${Geant4_INCLUDE_DIR}
                    ${ROOT_INCLUDE_DIR})
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc
${PROJECT_SOURCE_DIR}/SNS/src/*.cc 
#${PROJECT_SOURCE_DIR}/CosI/src/*.cc
)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh
${PROJECT_SOURCE_DIR}/SNS/include/*.hh  
#${PROJECT_SOURCE_DIR}/CosI/include/*.hh
)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(Sim ${sources} ${headers})
target_link_libraries(Sim ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} -lrt)

#-lrt for time.h clock_gettime function
#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build rdecay02. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(sim_SCRIPTS
    run.mac
	runBeamLoss.mac
	run_spectrum.mac
    vis.mac
  )

foreach(_script ${sim_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY 
    )
endforeach()
#NAMELINK_ONLY COPYONLY

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
add_custom_target(Work_current DEPENDS Sim)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS Sim DESTINATION bin)

# -std=gnu++0x on the CMAKE_CXX_FLAGS


MESSAGE("using  ${CMAKE_COMMAND}" )
MESSAGE("compiling ${PROJECT_NAME} from src ${PROJECT_SOURCE_DIR}" )
MESSAGE("G4DIR $ENV{G4DIR}") 