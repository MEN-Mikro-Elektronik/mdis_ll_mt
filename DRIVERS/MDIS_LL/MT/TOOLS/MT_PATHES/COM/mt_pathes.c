/****************************************************************************
 ************                                                    ************
 ************                 M T _ P A T H E S                  ************
 ************                                                    ************
 ****************************************************************************
 *  
 *       Author: see
 *        $Date: 2009/10/07 10:53:07 $
 *    $Revision: 1.6 $
 *
 *  Description: Verify MDIS path handling (using MT driver)
 *               - open max pathes    
 *               - random open/close
 *                      
 *               NOTE: "MT" driver must be used for this test                     
 *                      
 *     Required: usr_oss.l usr_utl.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: mt_pathes.c,v $
 * Revision 1.6  2009/10/07 10:53:07  CKauntz
 * R: path not 64 bit compatible
 * M: Changed path type to MDIS_PATH
 *
 * Revision 1.5  1999/07/20 12:20:58  Franke
 * cosmetics
 *
 * Revision 1.4  1999/07/05 08:17:10  kp
 * cosmetics
 *
 * Revision 1.3  1998/09/10 10:20:51  see
 * ported to changed USR_UTL calls
 *
 * Revision 1.2  1998/07/02 16:56:25  see
 * OS-9 code removed, changed for using UOS and UTL lib
 * include usr_oss.h and usr_utl.h
 *
 * Revision 1.1  1998/07/01 16:46:44  see
 * Initial Revision
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
 
static const char RCSid[]="$Id: mt_pathes.c,v 1.6 2009/10/07 10:53:07 CKauntz Exp $";

#include <stdio.h>
#include <stdlib.h>

#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>
#include <MEN/usr_utl.h>
#include <MEN/mdis_api.h>

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#define REV "V1.0"      /* program revision */

#define VERBOSE(_x_) 	if (verbose) printf _x_

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
typedef struct {
	MDIS_PATH path;
	int32 opened;
} PATHTEST;

/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static void usage(void);

/********************************* usage ************************************
 *
 *  Description: Print program usage and exit
 *			   
 *---------------------------------------------------------------------------
 *  Input......: no		exitcode
 *  Output.....: 
 *  Globals....: -
 ****************************************************************************/
static void usage(void)
{
	printf("Usage: mt_pathes [<opts>] <device> [<opts>]\n");
	printf("Function: Verify MDIS path open/close\n");
	printf("Options:\n");
	printf("    device       device name (MT)      [none]\n");
	printf("    -n=<max>     nr of opened pathes   [32]\n");
	printf("    -r=<n>       nr of random tests    [10]\n");
	printf("    -v           verbose               [off]\n");
	printf("\n");
	printf("(c) 1998 by MEN mikro elektronik GmbH, %s\n\n",REV);
}

/********************************* main *************************************
 *
 *  Description: Program main function
 *			   
 *---------------------------------------------------------------------------
 *  Input......: argc,argv	argument counter, data ..
 *  Output.....: exitcode	0=ok, >0=error
 *  Globals....: -
 ****************************************************************************/
int main( int  argc, char *argv[] )
{
	int32 pathCnt,rand=0,randMax,pathMax,verbose;
	int32 n,k,i,num,index,size;
	char *device,*str,*errstr,buf[40];
	PATHTEST *pt = NULL;		/* ptr to path data array */

	/*--------------------+
    |  check arguments    |
    +--------------------*/
	if ((errstr = UTL_ILLIOPT("n=r=v?", buf))) {	/* check args */
		printf("*** %s\n", errstr);
		return(1);
	}

	if (UTL_TSTOPT("?")) {						/* help requested ? */
		usage();
		return(1);
	}

	/*--------------------+
    |  get arguments      |
    +--------------------*/
	for (device=NULL, n=1; n<argc; n++)
		if (*argv[n] != '-') {
			device = argv[n];
			break;
		}

	if (!device) {	
		usage();
		return(1);
	}

	pathMax   = ((str = UTL_TSTOPT("n=")) ? atoi(str) : 32);
	randMax   = ((str = UTL_TSTOPT("r=")) ? atoi(str) : 10);
	verbose   = (UTL_TSTOPT("v") ? 1 : 0);

	/*--------------------+
    |  alloc mem          |
    +--------------------*/
	size = pathMax * sizeof(PATHTEST);

	if ((pt = (PATHTEST*)malloc(size)) == NULL) {
		printf("*** can't alloc %ld bytes\n",size);
	    goto abort;
	}

	for (n=0; n<pathMax; n++)		/* clear all */
		pt[n].opened = FALSE;

	/*--------------------+
    |  open max pathes    |
    +--------------------*/
	printf("--- open max pathes ---\n");

	/* open pathes */
	printf("open %ld pathes\n",pathMax);

	for (n=0; n<pathMax; n++) {
		if ((pt[n].path = M_open(device)) < 0) {
			printf("*** can't open path[%ld]: %s\n",n,M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

		VERBOSE((" open path=%ld\n",pt[n].path));
		pt[n].opened = TRUE;
	}

	/* close pathes */
	printf("close %ld pathes\n",n);

	while (n--) {
		VERBOSE((" close path=%ld\n",pt[n].path));

		if (M_close(pt[n].path) < 0) {
			printf("*** can't close path: %s\n",M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

		pt[n].opened = FALSE;
	}

	printf("ok.\n");

	/*--------------------+
    |  random open/close  |
    +--------------------*/
	printf("--- random open/close pathes ---\n");
	printf("open/close pathes\n");
	pathCnt = 0;

	for (k=0; k<randMax; k++) {
		/* open random number of pathes */
		rand = UOS_Random(rand);
		num = UOS_RandomMap(rand,1,pathMax-pathCnt);

		for (index=i=0; i<num; i++) {
			for (index=0; index<pathMax; index++)		/* find free entry */
				if (pt[index].opened == FALSE)
					break;

			if ((pt[index].path = M_open(device)) < 0) {
				printf("*** can't open path: %s\n",M_errstring(UOS_ErrnoGet()));
				goto abort;
			}

			VERBOSE((" opened path=%ld\n",pt[index].path));
			pt[index].opened = TRUE;
			pathCnt++;
		}

		/* close random number of pathes */
		rand = UOS_Random(rand);
		num = UOS_RandomMap(rand,1,pathCnt);

		for (i=0; i<num; i++) {
			do {										/* find used entry */
				rand = UOS_Random(rand);
				index = UOS_RandomMap(rand,0,pathMax-1);
			} while (pt[index].opened == FALSE);

			VERBOSE((" close path=%ld\n",pt[index].path));

			if (M_close(pt[index].path) < 0) {
				printf("*** can't close path: %s\n",M_errstring(UOS_ErrnoGet()));
				goto abort;
			}

			pt[index].opened = FALSE;
			pathCnt--;
		}
	}

	printf("done.\n");

	/*--------------------+
    |  cleanup            |
    +--------------------*/
	abort:

	for (index=0; index<pathMax; index++)
		if (pt[index].opened) {
			VERBOSE((" cleanup path=%ld\n",pt[index].path));

			if (M_close(pt[index].path) < 0)
				printf("*** can't close path: %s\n",M_errstring(UOS_ErrnoGet()));
		}

	if (pt)
		free(pt);

	return 0;
}


