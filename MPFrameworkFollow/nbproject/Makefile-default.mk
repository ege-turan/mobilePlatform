#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/MPFrameworkFollow.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/MPFrameworkFollow.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=FrameworkSource/ES_CheckEvents.c FrameworkSource/ES_DeferRecall.c FrameworkSource/ES_Framework.c FrameworkSource/ES_LookupTables.c FrameworkSource/ES_Port.c FrameworkSource/ES_PostList.c FrameworkSource/ES_Queue.c FrameworkSource/ES_Timers.c FrameworkSource/terminal.c FrameworkSource/circular_buffer_no_modulo_threadsafe.c FrameworkSource/dbprintf.c ProjectSource/EventCheckers.c ProjectSource/main.c ProjectSource/PIC32_IC_Lib.c ProjectSource/PIC32_PWM_Lib.c ProjectSource/PIC32_SPI_HAL.c ProjectSource/SPIFollowService.c ProjectSource/StepperService.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o ${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o ${OBJECTDIR}/FrameworkSource/ES_Framework.o ${OBJECTDIR}/FrameworkSource/ES_LookupTables.o ${OBJECTDIR}/FrameworkSource/ES_Port.o ${OBJECTDIR}/FrameworkSource/ES_PostList.o ${OBJECTDIR}/FrameworkSource/ES_Queue.o ${OBJECTDIR}/FrameworkSource/ES_Timers.o ${OBJECTDIR}/FrameworkSource/terminal.o ${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o ${OBJECTDIR}/FrameworkSource/dbprintf.o ${OBJECTDIR}/ProjectSource/EventCheckers.o ${OBJECTDIR}/ProjectSource/main.o ${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o ${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o ${OBJECTDIR}/ProjectSource/PIC32_SPI_HAL.o ${OBJECTDIR}/ProjectSource/SPIFollowService.o ${OBJECTDIR}/ProjectSource/StepperService.o
POSSIBLE_DEPFILES=${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o.d ${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o.d ${OBJECTDIR}/FrameworkSource/ES_Framework.o.d ${OBJECTDIR}/FrameworkSource/ES_LookupTables.o.d ${OBJECTDIR}/FrameworkSource/ES_Port.o.d ${OBJECTDIR}/FrameworkSource/ES_PostList.o.d ${OBJECTDIR}/FrameworkSource/ES_Queue.o.d ${OBJECTDIR}/FrameworkSource/ES_Timers.o.d ${OBJECTDIR}/FrameworkSource/terminal.o.d ${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o.d ${OBJECTDIR}/FrameworkSource/dbprintf.o.d ${OBJECTDIR}/ProjectSource/EventCheckers.o.d ${OBJECTDIR}/ProjectSource/main.o.d ${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o.d ${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o.d ${OBJECTDIR}/ProjectSource/PIC32_SPI_HAL.o.d ${OBJECTDIR}/ProjectSource/SPIFollowService.o.d ${OBJECTDIR}/ProjectSource/StepperService.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o ${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o ${OBJECTDIR}/FrameworkSource/ES_Framework.o ${OBJECTDIR}/FrameworkSource/ES_LookupTables.o ${OBJECTDIR}/FrameworkSource/ES_Port.o ${OBJECTDIR}/FrameworkSource/ES_PostList.o ${OBJECTDIR}/FrameworkSource/ES_Queue.o ${OBJECTDIR}/FrameworkSource/ES_Timers.o ${OBJECTDIR}/FrameworkSource/terminal.o ${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o ${OBJECTDIR}/FrameworkSource/dbprintf.o ${OBJECTDIR}/ProjectSource/EventCheckers.o ${OBJECTDIR}/ProjectSource/main.o ${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o ${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o ${OBJECTDIR}/ProjectSource/PIC32_SPI_HAL.o ${OBJECTDIR}/ProjectSource/SPIFollowService.o ${OBJECTDIR}/ProjectSource/StepperService.o

# Source Files
SOURCEFILES=FrameworkSource/ES_CheckEvents.c FrameworkSource/ES_DeferRecall.c FrameworkSource/ES_Framework.c FrameworkSource/ES_LookupTables.c FrameworkSource/ES_Port.c FrameworkSource/ES_PostList.c FrameworkSource/ES_Queue.c FrameworkSource/ES_Timers.c FrameworkSource/terminal.c FrameworkSource/circular_buffer_no_modulo_threadsafe.c FrameworkSource/dbprintf.c ProjectSource/EventCheckers.c ProjectSource/main.c ProjectSource/PIC32_IC_Lib.c ProjectSource/PIC32_PWM_Lib.c ProjectSource/PIC32_SPI_HAL.c ProjectSource/SPIFollowService.c ProjectSource/StepperService.c



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk ${DISTDIR}/MPFrameworkFollow.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=32MX170F256B
MP_LINKER_FILE_OPTION=
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o: FrameworkSource/ES_CheckEvents.c  .generated_files/flags/default/c334d18f0dbb987c9d548d06a2d30b432603c41c .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o FrameworkSource/ES_CheckEvents.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o: FrameworkSource/ES_DeferRecall.c  .generated_files/flags/default/bccc1e33ab305d2e61d60b8cc7c7decf9a2d43d9 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o FrameworkSource/ES_DeferRecall.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_Framework.o: FrameworkSource/ES_Framework.c  .generated_files/flags/default/e7a350ec75beb4f60c217e2c31f13ff6fc791ede .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Framework.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Framework.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_Framework.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_Framework.o FrameworkSource/ES_Framework.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_LookupTables.o: FrameworkSource/ES_LookupTables.c  .generated_files/flags/default/5ab334205f8861c036ae8e38c2307690e8691488 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_LookupTables.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_LookupTables.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_LookupTables.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_LookupTables.o FrameworkSource/ES_LookupTables.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_Port.o: FrameworkSource/ES_Port.c  .generated_files/flags/default/d050b59a37a2fa11b904b19f41770428c2d7ac78 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Port.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Port.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_Port.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_Port.o FrameworkSource/ES_Port.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_PostList.o: FrameworkSource/ES_PostList.c  .generated_files/flags/default/eee8d099b31dfcaa0432e9f0c0e2f93d5d73b3a5 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_PostList.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_PostList.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_PostList.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_PostList.o FrameworkSource/ES_PostList.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_Queue.o: FrameworkSource/ES_Queue.c  .generated_files/flags/default/a9749b82fe6927bc35ee3d151b9eb9cd58d27f07 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Queue.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Queue.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_Queue.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_Queue.o FrameworkSource/ES_Queue.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_Timers.o: FrameworkSource/ES_Timers.c  .generated_files/flags/default/639c881ae39466ed6049abbef5972ef9722d0d53 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Timers.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Timers.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_Timers.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_Timers.o FrameworkSource/ES_Timers.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/terminal.o: FrameworkSource/terminal.c  .generated_files/flags/default/6be08efb9ab60b0bbf9fd68c660f7499617e98dc .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/terminal.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/terminal.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/terminal.o.d" -o ${OBJECTDIR}/FrameworkSource/terminal.o FrameworkSource/terminal.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o: FrameworkSource/circular_buffer_no_modulo_threadsafe.c  .generated_files/flags/default/428d7062bb078fca7eceabfb40d8fd8e41dd8ef7 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o.d" -o ${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o FrameworkSource/circular_buffer_no_modulo_threadsafe.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/dbprintf.o: FrameworkSource/dbprintf.c  .generated_files/flags/default/5664470895387131424b1d76e2d653626bf218f1 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/dbprintf.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/dbprintf.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/dbprintf.o.d" -o ${OBJECTDIR}/FrameworkSource/dbprintf.o FrameworkSource/dbprintf.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/EventCheckers.o: ProjectSource/EventCheckers.c  .generated_files/flags/default/a088d3482aee8b91c66c11c95446a519dc66aa60 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/EventCheckers.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/EventCheckers.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/EventCheckers.o.d" -o ${OBJECTDIR}/ProjectSource/EventCheckers.o ProjectSource/EventCheckers.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/main.o: ProjectSource/main.c  .generated_files/flags/default/49d3b16779ff58a4e3c1398405bc56e0743a0e98 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/main.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/main.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/main.o.d" -o ${OBJECTDIR}/ProjectSource/main.o ProjectSource/main.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o: ProjectSource/PIC32_IC_Lib.c  .generated_files/flags/default/9b922d75d3537993a76cf04dbbb13026dfa95623 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o.d" -o ${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o ProjectSource/PIC32_IC_Lib.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o: ProjectSource/PIC32_PWM_Lib.c  .generated_files/flags/default/4e46e64a806d0073c510ed712306c715f10f23b1 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o.d" -o ${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o ProjectSource/PIC32_PWM_Lib.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/PIC32_SPI_HAL.o: ProjectSource/PIC32_SPI_HAL.c  .generated_files/flags/default/c330a6b4782f46088adc7bd4135ebf22aed07c11 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/PIC32_SPI_HAL.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/PIC32_SPI_HAL.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/PIC32_SPI_HAL.o.d" -o ${OBJECTDIR}/ProjectSource/PIC32_SPI_HAL.o ProjectSource/PIC32_SPI_HAL.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/SPIFollowService.o: ProjectSource/SPIFollowService.c  .generated_files/flags/default/62d8b496bf431e157b19916f2744df62a2969b39 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/SPIFollowService.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/SPIFollowService.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/SPIFollowService.o.d" -o ${OBJECTDIR}/ProjectSource/SPIFollowService.o ProjectSource/SPIFollowService.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/StepperService.o: ProjectSource/StepperService.c  .generated_files/flags/default/b700d1c08b2dd5c677a87131fe2fd5635733d5fd .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/StepperService.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/StepperService.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/StepperService.o.d" -o ${OBJECTDIR}/ProjectSource/StepperService.o ProjectSource/StepperService.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
else
${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o: FrameworkSource/ES_CheckEvents.c  .generated_files/flags/default/bfeb9ca1811519e02c939bff1d4329afe8a1a883 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_CheckEvents.o FrameworkSource/ES_CheckEvents.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o: FrameworkSource/ES_DeferRecall.c  .generated_files/flags/default/219e9323788e5c657c83a493494e9bf32d8a7190 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_DeferRecall.o FrameworkSource/ES_DeferRecall.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_Framework.o: FrameworkSource/ES_Framework.c  .generated_files/flags/default/ac501f21190d8bc16dcf32d5bf30a1bd8767c4c9 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Framework.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Framework.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_Framework.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_Framework.o FrameworkSource/ES_Framework.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_LookupTables.o: FrameworkSource/ES_LookupTables.c  .generated_files/flags/default/b473826e55d277cd393230c1e640629a4f2c345c .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_LookupTables.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_LookupTables.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_LookupTables.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_LookupTables.o FrameworkSource/ES_LookupTables.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_Port.o: FrameworkSource/ES_Port.c  .generated_files/flags/default/e90f7e51a0052d4e590ae5c00cdd8edc87da5637 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Port.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Port.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_Port.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_Port.o FrameworkSource/ES_Port.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_PostList.o: FrameworkSource/ES_PostList.c  .generated_files/flags/default/a6364a0cca26df3ad29cd524942b0ba79bcd139b .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_PostList.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_PostList.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_PostList.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_PostList.o FrameworkSource/ES_PostList.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_Queue.o: FrameworkSource/ES_Queue.c  .generated_files/flags/default/1e1fa0cac5d10dd4b5fe8c4d295a5df96f6e8058 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Queue.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Queue.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_Queue.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_Queue.o FrameworkSource/ES_Queue.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/ES_Timers.o: FrameworkSource/ES_Timers.c  .generated_files/flags/default/9dc9af7c14a5fb074c1c78d88f0615e3379b5d9c .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Timers.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/ES_Timers.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/ES_Timers.o.d" -o ${OBJECTDIR}/FrameworkSource/ES_Timers.o FrameworkSource/ES_Timers.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/terminal.o: FrameworkSource/terminal.c  .generated_files/flags/default/c259edef8ffc27469b7d4b2173218151cccf9f33 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/terminal.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/terminal.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/terminal.o.d" -o ${OBJECTDIR}/FrameworkSource/terminal.o FrameworkSource/terminal.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o: FrameworkSource/circular_buffer_no_modulo_threadsafe.c  .generated_files/flags/default/789cedd6d162a0396f9f35e0a4b6be1066462564 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o.d" -o ${OBJECTDIR}/FrameworkSource/circular_buffer_no_modulo_threadsafe.o FrameworkSource/circular_buffer_no_modulo_threadsafe.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/FrameworkSource/dbprintf.o: FrameworkSource/dbprintf.c  .generated_files/flags/default/3fb19f8154a76a2f0158fde457418212876728d7 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/FrameworkSource" 
	@${RM} ${OBJECTDIR}/FrameworkSource/dbprintf.o.d 
	@${RM} ${OBJECTDIR}/FrameworkSource/dbprintf.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/FrameworkSource/dbprintf.o.d" -o ${OBJECTDIR}/FrameworkSource/dbprintf.o FrameworkSource/dbprintf.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/EventCheckers.o: ProjectSource/EventCheckers.c  .generated_files/flags/default/2dc5175f7d3e344d57547da60ec10b44e0fcd030 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/EventCheckers.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/EventCheckers.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/EventCheckers.o.d" -o ${OBJECTDIR}/ProjectSource/EventCheckers.o ProjectSource/EventCheckers.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/main.o: ProjectSource/main.c  .generated_files/flags/default/ee5bf55d57128de9275dc5dbcbbcd7dbc871f997 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/main.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/main.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/main.o.d" -o ${OBJECTDIR}/ProjectSource/main.o ProjectSource/main.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o: ProjectSource/PIC32_IC_Lib.c  .generated_files/flags/default/84c3cd8da9f5ac66548955d57e0ca7f2ced747b3 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o.d" -o ${OBJECTDIR}/ProjectSource/PIC32_IC_Lib.o ProjectSource/PIC32_IC_Lib.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o: ProjectSource/PIC32_PWM_Lib.c  .generated_files/flags/default/9b2cbf821c63b4197b0c5e51069f4b8550a42164 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o.d" -o ${OBJECTDIR}/ProjectSource/PIC32_PWM_Lib.o ProjectSource/PIC32_PWM_Lib.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/PIC32_SPI_HAL.o: ProjectSource/PIC32_SPI_HAL.c  .generated_files/flags/default/e1098d850d23289dd635f471831f86ca28d8084e .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/PIC32_SPI_HAL.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/PIC32_SPI_HAL.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/PIC32_SPI_HAL.o.d" -o ${OBJECTDIR}/ProjectSource/PIC32_SPI_HAL.o ProjectSource/PIC32_SPI_HAL.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/SPIFollowService.o: ProjectSource/SPIFollowService.c  .generated_files/flags/default/41a836af8beed3fd847b2d85d7f9a6497a1cb61b .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/SPIFollowService.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/SPIFollowService.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/SPIFollowService.o.d" -o ${OBJECTDIR}/ProjectSource/SPIFollowService.o ProjectSource/SPIFollowService.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/ProjectSource/StepperService.o: ProjectSource/StepperService.c  .generated_files/flags/default/a74699712c11ca41d30b324cccde27f6901994f2 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/ProjectSource" 
	@${RM} ${OBJECTDIR}/ProjectSource/StepperService.o.d 
	@${RM} ${OBJECTDIR}/ProjectSource/StepperService.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"FrameworkHeaders" -I"ProjectHeaders" -fno-common -MP -MMD -MF "${OBJECTDIR}/ProjectSource/StepperService.o.d" -o ${OBJECTDIR}/ProjectSource/StepperService.o ProjectSource/StepperService.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${DISTDIR}/MPFrameworkFollow.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -g   -mprocessor=$(MP_PROCESSOR_OPTION)  -o ${DISTDIR}/MPFrameworkFollow.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)   -mreserve=data@0x0:0x1FC -mreserve=boot@0x1FC00490:0x1FC00BEF  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D=__DEBUG_D,--defsym=_min_heap_size=256,--no-code-in-dinit,--no-dinit-in-serial-mem,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,${DISTDIR}/memoryfile.xml -mdfp="${DFP_DIR}"
	
else
${DISTDIR}/MPFrameworkFollow.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o ${DISTDIR}/MPFrameworkFollow.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=256,--no-code-in-dinit,--no-dinit-in-serial-mem,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,${DISTDIR}/memoryfile.xml -mdfp="${DFP_DIR}"
	${MP_CC_DIR}\\xc32-bin2hex ${DISTDIR}/MPFrameworkFollow.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${OBJECTDIR}
	${RM} -r ${DISTDIR}

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(wildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
