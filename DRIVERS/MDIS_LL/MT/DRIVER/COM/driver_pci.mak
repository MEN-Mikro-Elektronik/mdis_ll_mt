#***************************  M a k e f i l e  *******************************
#
#         Author: kp
#          $Date: 2009/10/07 10:28:41 $
#      $Revision: 1.2 $
#
#    Description: Makefile definitions for the MT_PCI driver
#		  		  MK functionality test with P10
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver_pci.mak,v $
#   Revision 1.2  2009/10/07 10:28:41  CKauntz
#   R: PLD library not used
#   M: Removed pld library
#
#   Revision 1.1  1999/08/11 10:51:53  kp
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=mt_pci

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)     \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/mbuf$(LIB_SUFFIX)     \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)      \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/id$(LIB_SUFFIX)      \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/dbg$(LIB_SUFFIX)     \


MAK_INCL=$(MEN_INC_DIR)/mt_drv.h     \
         $(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/oss.h         \
         $(MEN_INC_DIR)/mdis_err.h    \
         $(MEN_INC_DIR)/mbuf.h        \
         $(MEN_INC_DIR)/maccess.h     \
         $(MEN_INC_DIR)/desc.h        \
         $(MEN_INC_DIR)/mdis_api.h    \
         $(MEN_INC_DIR)/mdis_com.h    \
         $(MEN_INC_DIR)/modcom.h      \
         $(MEN_INC_DIR)/ll_defs.h     \
         $(MEN_INC_DIR)/ll_entry.h    \
         $(MEN_INC_DIR)/dbg.h    \

MAK_SWITCH=$(SW_PREFIX)LOCKMODE=0 \
		   $(SW_PREFIX)MT_PCI\
		   $(SW_PREFIX)MAC_MEM_MAPPED

MAK_INP1=mt_drv$(INP_SUFFIX)
MAK_INP2=

MAK_INP=$(MAK_INP1) \
        $(MAK_INP2)

