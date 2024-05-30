# Obtaining the project root path (to project's path)
Prdir:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

# Setting of the general compiled grammer
## The Compiler, library, level of the compiler optimization, detected information, WALL and message
## Obtaining the all compiler setting string from globalCompiling.ini and the function, iniParser_getIni, in the file, iniParser.sh;
## the return value is "compiler.STD=-std=c++11;compiler.DETAILINFO=-g0; ..."
KVPAIR:=$(shell source ${Prdir}/Shells/iniParser.sh && echo $$(iniParser_getIni ${Prdir}/.Ini/globalCompiling.ini))
## Obtaining the compiling information
CC			=$(shell source ${Prdir}/Shells/iniParser.sh && echo $$(getVariableValue "${KVPAIR}" "compiler.CC"))
STD			=$(shell source ${Prdir}/Shells/iniParser.sh && echo $$(getVariableValue "${KVPAIR}" "compiler.STD"))
CMPOPT		=$(shell source ${Prdir}/Shells/iniParser.sh && echo $$(getVariableValue "${KVPAIR}" "compiler.CMPOPT"))
DETAILINFO	=$(shell source ${Prdir}/Shells/iniParser.sh && echo $$(getVariableValue "${KVPAIR}" "compiler.DETAILINFO"))
WALL		=$(shell source ${Prdir}/Shells/iniParser.sh && echo $$(getVariableValue "${KVPAIR}" "compiler.WALL"))
FMSG		=$(shell source ${Prdir}/Shells/iniParser.sh && echo $$(getVariableValue "${KVPAIR}" "compiler.FMSG"))
CFLAGS		=$(shell source ${Prdir}/Shells/iniParser.sh && echo $$(getVariableValue "${KVPAIR}" "compiler.CFLAGS"))
LDFLAGS		=$(shell source ${Prdir}/Shells/iniParser.sh && echo $$(getVariableValue "${KVPAIR}" "compiler.LDFLAGS"))
LDLIBS		=$(shell source ${Prdir}/Shells/iniParser.sh && echo $$(getVariableValue "${KVPAIR}" "compiler.LDLIBS"))

# ------ 
Headers=${Prdir}/Headers
Sources=${Prdir}/Sources

## Project execution's name
PjN:= $(word $(words $(subst /, ,${Prdir})), $(subst /, ,${Prdir}))

# Make's Processes
.Suffixs: .c .h .cpp .hpp

.Phony: all
all:	${Prdir}/${PjN}
	@sudo chown root:root ${Prdir}/${PjN}
	@sudo chmod 4755 ${Prdir}/${PjN}
	@make run
	

.Phony: build
build: ${Prdir}/${PjN}/build

.Phony: clean
clean:
	@clear
	@rm -rf ${Prdir}/*.o
	@rm -rf ${Prdir}/*/*.o
	@rm -rf ${Prdir}/*/*/*.o
	@rm -rf ${Prdir}/*/*/*/*.o
	@rm -rf ${Prdir}/${PjN}

.Phony: cmakeClean
cmakeClean:
	@clear
	@rm -rf ${Prdir}/build/*


##----------------------------------------------------------------------
## Nick editted manually
## ---------------------------------------------------------------------
##dbsecure ALL=NOPASSWD: /bin/rm -rf /home/dbsecure/trafficMonitor/trafficMonitor
.Phony: run
run:	
	@echo "=========[start]========="
	@${Prdir}/${PjN}
	@echo "=========[end]========="


# Build libraries
${Prdir}/${PjN}/build : 
			

##================================================================
# Create a application
${Prdir}/${PjN}: 	${Prdir}/Main.o \
					${Sources}/LibFileSystem.o \
					${Sources}/LibPFRingPacketLost.o

	${CC} ${STD} ${CMPOPT} ${DETAILINFO} ${WALL} ${FMSG} -o ${Prdir}/${PjN} ${Prdir}/Main.o \
	${Sources}/LibFileSystem.o \
	${Sources}/LibPFRingPacketLost.o \
	${LDLIBS}

# Main
${Prdir}/Main.o:	${Headers}/LibFileSystem.h ${Prdir}/Main.c
	${CC} ${STD} ${CMPOPT} ${DETAILINFO} ${WALL} ${Prdir}/Main.c -c ${FMSG} -o ${Prdir}/Main.o

# LibFileSystem
${Sources}/LibFileSystem.o:	${Headers}/LibFileSystem.h ${Sources}/LibFileSystem.c
	${CC} ${STD} ${CMPOPT} ${DETAILINFO} ${WALL} ${Sources}/LibFileSystem.c -c ${FMSG} -o ${Sources}/LibFileSystem.o

#LibPFRingPacketLost
${Sources}/LibPFRingPacketLost.o:	${Headers}/LibPFRingPacketLost.h ${Sources}/LibPFRingPacketLost.c
	${CC} ${STD} ${CMPOPT} ${DETAILINFO} ${WALL} ${Sources}/LibPFRingPacketLost.c -c ${FMSG} -o ${Sources}/LibPFRingPacketLost.o