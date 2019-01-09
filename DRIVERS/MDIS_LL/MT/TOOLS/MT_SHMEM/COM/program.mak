#***************************  M a k e f i l e  *******************************
#
#         Author: see
#          $Date: 1999/04/16 16:26:41 $
#      $Revision: 1.2 $
#
#    Description: Makefile definitions for MT tools
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: program.mak,v $
#   Revision 1.2  1999/04/16 16:26:41  Franke
#   cosmetics
#
#   Revision 1.1  1998/09/18 14:06:50  see
#   Added by mcvs
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=mt_shmem

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/usr_oss$(LIB_SUFFIX)     \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/usr_utl$(LIB_SUFFIX)     \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/mdis_api$(LIB_SUFFIX)    \
         
MAK_INCL=$(MEN_INC_DIR)/mt_drv.h      \
         $(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/mdis_api.h    \
         $(MEN_INC_DIR)/mdis_err.h    \
         $(MEN_INC_DIR)/usr_oss.h     \
         $(MEN_INC_DIR)/usr_utl.h     \

MAK_OPTIM=$(OPT_1)

MAK_INP1=mt_shmem$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

