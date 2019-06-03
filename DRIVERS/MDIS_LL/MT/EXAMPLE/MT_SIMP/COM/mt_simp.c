/****************************************************************************
 ************                                                    ************
 ************                   MT_SIMP                        ************
 ************                                                    ************
 ****************************************************************************
 *  
 *       Author: see
 *
 *  Description: Simple example program for the MT driver 
 *                      
 *     Required: MDIS user interface library
 *     Switches: NO_MAIN_FUNC	(for systems with one namespace)
 *
 *
 *---------------------------------------------------------------------------
 * Copyright (c) 1998-2019, MEN Mikro Elektronik GmbH
 ****************************************************************************/

 /*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 

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


