/****************************************************************************
 ************                                                    ************
 ************                   MT_SIMP                        ************
 ************                                                    ************
 ****************************************************************************
 *  
 *       Author: see
 *        $Date: 2009/10/07 10:52:42 $
 *    $Revision: 1.3 $
 *
 *  Description: Simple example program for the MT driver 
 *                      
 *     Required: MDIS user interface library
 *     Switches: NO_MAIN_FUNC	(for systems with one namespace)
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: mt_simp.c,v $
 * Revision 1.3  2009/10/07 10:52:42  CKauntz
 * R: path not 64 bit compatible
 * M: Changed path type to MDIS_PATH
 *
 * Revision 1.2  1999/04/16 16:26:32  Franke
 * cosmetics
 *
 * Revision 1.1  1998/07/31 17:24:34  see
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/
 
static const char RCSid[]="$Id: mt_simp.c,v 1.3 2009/10/07 10:52:42 CKauntz Exp $";

#include <stdio.h>
#include <string.h>
#include <MEN/men_typs.h>
#include <MEN/mdis_api.h>
#include <MEN/mdis_err.h>
#include <MEN/usr_oss.h>
#include <MEN/mt_drv.h>

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static int32 _mt_simple(char *arg1, char *arg2, char *arg3);


/********************************* main *************************************
 *
 *  Description: MAIN entry (redefined in systems with one namespace)
 *			   
 *---------------------------------------------------------------------------
 *  Input......: argc, argv		command line arguments/counter
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/
int main( int  argc, char *argv[] )
{
    return( _mt_simple(argv[1], argv[2], argv[3]) );
}


/******************************* mt_simple *********************************
 *
 *  Description:  Example (directly called in systems with one namespace)
 *
 *---------------------------------------------------------------------------
 *  Input......:  arg1..argn	arguments
 *  Output.....:  return        success (0) or error (1)
 *  Globals....:  ---
 *
 ****************************************************************************/
int32 _mt_simple(char *arg1, char *arg2, char *arg3)
{
	MDIS_PATH path;
	char *device;

	if (strcmp(arg1,"-?") == 0) {
		printf("mt_simpl <device>\n");
		return(1);
	}

	device = arg1;

	printf("open %s\n",device);

	if ((path = M_open(device)) < 0) {
		printf("*** %s ***\n", M_errstring(UOS_ErrnoGet()));
		return(1);
	}

	/* ... */

	M_close(path);
	return(0);
}


