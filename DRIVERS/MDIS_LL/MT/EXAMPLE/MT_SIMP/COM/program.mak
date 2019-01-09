#***************************  M a k e f i l e  *******************************
#
#         Author: see
#          $Date: 1998/07/31 17:24:32 $
#      $Revision: 1.1 $
#
#    Description: Makefile definitions for the MT example program
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: program.mak,v $
#   Revision 1.1  1998/07/31 17:24:32  see
#   Added by mcvs
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=mt_simp

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/mdis_api$(LIB_SUFFIX)    \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/usr_oss$(LIB_SUFFIX)     \


MAK_INCL=$(MEN_INC_DIR)/mt_drv.h     \
         $(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/mdis_api.h    \
         $(MEN_INC_DIR)/mdis_err.h    \
         $(MEN_INC_DIR)/usr_oss.h     \


MAK_OPTIM=$(OPT_1)

MAK_INP1=mt_simp$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)
