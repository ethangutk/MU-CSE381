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
	${OBJECTDIR}/sqlair_starter/SQLAir.o \
	${OBJECTDIR}/sqlair_starter/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-fsanitize=address -DGNUCXX_DEBUG
CXXFLAGS=-fsanitize=address -DGNUCXX_DEBUG

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=sqlair_starter/libsqlair_lib.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk homework08

homework08: sqlair_starter/libsqlair_lib.a

homework08: ${OBJECTFILES}
	${LINK.cc} -o homework08 ${OBJECTFILES} ${LDLIBSOPTIONS} -lboost_system -lpthread -lmysqlpp

${OBJECTDIR}/sqlair_starter/SQLAir.o: sqlair_starter/SQLAir.cpp
	${MKDIR} -p ${OBJECTDIR}/sqlair_starter
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/sqlair_starter/SQLAir.o sqlair_starter/SQLAir.cpp

${OBJECTDIR}/sqlair_starter/main.o: sqlair_starter/main.cpp
	${MKDIR} -p ${OBJECTDIR}/sqlair_starter
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/sqlair_starter/main.o sqlair_starter/main.cpp

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
