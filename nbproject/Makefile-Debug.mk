#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Vision.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=`pkg-config --libs libavcodec-ffmpeg` `pkg-config --libs libavformat-ffmpeg` `pkg-config --libs libavutil-ffmpeg` `pkg-config --libs libswresample-ffmpeg` `pkg-config --libs libswscale-ffmpeg` `pkg-config --libs opencv` -lpthread  ../lib/libLog.so ../lib/libLog.so  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../lib/libVision.${CND_DLIB_EXT}

../lib/libVision.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ../lib
	${LINK.cc} -o ../lib/libVision.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -shared -fPIC

${OBJECTDIR}/Vision.o: Vision.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g `pkg-config --cflags libavcodec-ffmpeg` `pkg-config --cflags libavformat-ffmpeg` `pkg-config --cflags libavutil-ffmpeg` `pkg-config --cflags libswresample-ffmpeg` `pkg-config --cflags libswscale-ffmpeg` `pkg-config --cflags opencv` -std=c++11  -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Vision.o Vision.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
