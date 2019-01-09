#***************************  M a k e f i l e  *******************************
#
#         Author: dieter.pfeuffer@men.de
#          $Date: 2008/08/29 11:45:59 $
#      $Revision: 1.2 $
#
#    Description: Makefile definitions for the MT driver
#		  		  - makes never a HW access and requests no address space
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver_nohw.mak,v $
#   Revision 1.2  2008/08/29 11:45:59  dpfeuffer
#   R: unused libs and header specified
#   M: unused libs and header removed
#
#   Revision 1.1  2004/06/22 13:53:50  dpfeuffer
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=mt_nohw

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)     \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)      \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/dbg$(LIB_SUFFIX)     \


MAK_INCL=$(MEN_INC_DIR)/mt_drv.h     \
         $(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/oss.h         \
         $(MEN_INC_DIR)/mdis_err.h    \
         $(MEN_INC_DIR)/maccess.h     \
         $(MEN_INC_DIR)/desc.h        \
         $(MEN_INC_DIR)/mdis_api.h    \
         $(MEN_INC_DIR)/mdis_com.h    \
         $(MEN_INC_DIR)/ll_defs.h     \
         $(MEN_INC_DIR)/ll_entry.h    \
         $(MEN_INC_DIR)/dbg.h    \

MAK_SWITCH=$(SW_PREFIX)LOCKMODE=0      \
		   $(SW_PREFIX)MAC_MEM_MAPPED  \
		   $(SW_PREFIX)NO_HWACCESS

MAK_INP1=mt_drv$(INP_SUFFIX)
MAK_INP2=

MAK_INP=$(MAK_INP1) \
        $(MAK_INP2)

