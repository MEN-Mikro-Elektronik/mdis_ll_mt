#************************** MDIS4 device descriptor *************************
#
#        Author: kp
#         $Date: 1999/08/10 15:33:42 $
#     $Revision: 1.1 $
#
#   Description: Metadescriptor for MT_PCI (use with P10)
#
#****************************************************************************

MT_PCI_1  {
	#------------------------------------------------------------------------
	#	general parameters (don't modify)
	#------------------------------------------------------------------------
    DESC_TYPE       = U_INT32       1             # descriptor type (1=device)
    HW_TYPE         = STRING        MT_PCI	      # hardware name of device

	#------------------------------------------------------------------------
	#	base board configuration
	#------------------------------------------------------------------------
    BOARD_NAME      = STRING        A11_OBPCMIP   # device name of baseboard
    DEVICE_SLOT     = U_INT32       0             # used slot on baseboard
    IRQ_ENABLE      = U_INT32       0             # irq enabled after init

	#------------------------------------------------------------------------
	#	debug levels (optional)
	#------------------------------------------------------------------------
    DEBUG_LEVEL         = U_INT32   0xc0008007    # LL driver
    DEBUG_LEVEL_MK      = U_INT32   0xc0008007    # MDIS kernel
    DEBUG_LEVEL_OSS     = U_INT32   0xc0008002    # OSS calls
    DEBUG_LEVEL_DESC    = U_INT32   0xc0008002    # DESC calls

	#------------------------------------------------------------------------
	#	device parameters
	#------------------------------------------------------------------------
	PCI_FUNCTION		= U_INT32	0
	PCI_BASEREG_ASSIGN_0= U_INT32	0
	PCI_BASEREG_ASSIGN_1= U_INT32	1

	PCI_VENDOR_ID		= U_INT32	0x1415
	PCI_DEVICE_ID		= U_INT32	0x9501

	PCI_SUBSYS_VENDOR_ID= U_INT32	0x1415
	PCI_SUBSYS_ID		= U_INT32	0x0000

}
