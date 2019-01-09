#************************** MDIS4 device descriptor *************************
#
#        Author: see
#         $Date: 1999/07/05 08:05:46 $
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
    ID_ERROR            = U_INT32   0             # produce ID prom error
	CALLBACK_SIGNAL     = U_INT32   0x100         # signal used for callbacks

	#--- trigger enable
	ENABLE_TRIG			= U_INT32   0             # enable trigger access

	#--- trigger addresses
	#	if trigger access is enabled, a 16-bit read access
	#	to the specified trigger address is done at each
	#	trigger condition
	CALLBACK_0_TRIG     = U_INT32   0x100         # callback 0 trigger address
	CALLBACK_1_TRIG     = U_INT32   0x102         # callback 1 trigger address
	ALARM_0_TRIG        = U_INT32   0x110         # alarm 0 trigger address
	ALARM_1_TRIG        = U_INT32   0x112         # alarm 1 trigger address
}
