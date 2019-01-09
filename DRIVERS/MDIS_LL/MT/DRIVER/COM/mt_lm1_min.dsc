#************************** MDIS4 device descriptor *************************
#
#        Author: see
#         $Date: 1999/07/05 08:05:47 $
#     $Revision: 1.1 $
#
#   Description: Metadescriptor for MT_LM1
#
#****************************************************************************

MT_LM1_1  {
	#------------------------------------------------------------------------
	#	general parameters (don't modify)
	#------------------------------------------------------------------------
    DESC_TYPE       = U_INT32       1             # descriptor type (1=device)
    HW_TYPE         = STRING        MT_LM1        # hardware name of device

	#------------------------------------------------------------------------
	#	base board configuration
	#------------------------------------------------------------------------
    BOARD_NAME      = STRING        A201_1        # device name of baseboard
    DEVICE_SLOT     = U_INT32       0             # used slot on baseboard
}
