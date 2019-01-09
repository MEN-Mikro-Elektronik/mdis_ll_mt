#***************************  M a k e f i l e  *******************************
#
#         Author: see
#          $Date: 2008/08/29 11:45:57 $
#      $Revision: 1.4 $
#
#    Description: Makefile definitions for the MT driver
#		  		  LOCKMODE 0 (unknown)
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver.mak,v $
#   Revision 1.4  2008/08/29 11:45:57  dpfeuffer
#   R: unused libs and header specified
#   M: unused libs and header removed
#
#   Revision 1.3  1999/07/05 08:04:47  kp
#   use lockmode=0
#
#   Revision 1.2  1999/04/16 16:26:20  Franke
#   added LOCKMODE switch
#   renamed MAK_NAME from mt to mt2
#
#   Revision 1.1  1998/05/28 09:58:30  see
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=mt

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
		   $(SW_PREFIX)MAC_MEM_MAPPED

MAK_INP1=mt_drv$(INP_SUFFIX)
MAK_INP2=

MAK_INP=$(MAK_INP1) \
        $(MAK_INP2)

