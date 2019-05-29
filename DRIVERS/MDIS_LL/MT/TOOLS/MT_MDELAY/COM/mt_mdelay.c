/****************************************************************************
 ************                                                    ************
 ************                 M T _ M D E L A Y                  ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: kp
 *        $Date: 2009/10/07 10:53:04 $
 *    $Revision: 1.3 $
 *
 *  Description: Perform MT driver's MT_MDELAY setstat
 *
 *               NOTE: "MT" driver must be used for this test
 *
 *     Required: usr_oss.l usr_utl.l
 *     Switches: -
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

static const char RCSid[]="$Id: mt_mdelay.c,v 1.3 2009/10/07 10:53:04 CKauntz Exp $";

#include <stdio.h>
#include <stdlib.h>

#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>
#include <MEN/usr_utl.h>
#include <MEN/mdis_api.h>
#include <MEN/mt_drv.h>

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#define VERBOSE(_x_) 	if (verbose) printf _x_
#define DELAY 1000000

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/

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
 *  Description: Print program usage
 *
 *---------------------------------------------------------------------------
 *  Input......: -
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/
static void usage(void)
{
	printf("Usage: mt_mdelay [<opts>] <device> [<opts>]\n");
	printf("Function: Perform MDIS MT drivers MT_MDELAY setstat\n");
	printf("Options:\n");
	printf("    device       device name (MT)      [none]\n");
	printf("    -t=<us>      usec to delay         [%d]\n",DELAY);
	printf("\n");
	printf("(c) 1999 by MEN mikro elektronik GmbH\n%s\n\n",RCSid);
}

/********************************* main *************************************
 *
 *  Description: Program main function
 *
 *---------------------------------------------------------------------------
 *  Input......: argc,argv	argument counter, data ..
 *  Output.....: return	0=ok, >0=error
 *  Globals....: -
 ****************************************************************************/
int main( int  argc, char *argv[] )
{
    MDIS_PATH path=-1;
	int32 n,delay=DELAY,dt,t1;
	char *device,*str,*errstr,buf[40];

	/*--------------------+
    |  check arguments    |
    +--------------------*/
	if ((errstr = UTL_ILLIOPT("t=?", buf))) {	/* check args */
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

	delay     = ((str = UTL_TSTOPT("t=")) ? atoi(str) : DELAY);

	/*--------------------+
    |  OPEN DEVICE        |
    +--------------------*/
	if ((path = M_open(device)) < 0) {
		printf("*** can't open path: %s\n",M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	t1 = UOS_MsecTimerGet();

	if( M_setstat( path, MT_MDELAY, delay ) < 0 ){
		printf("*** setstat MT_MDELAY failed: %s\n",
			   M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	dt = UOS_MsecTimerGet() - t1;


	printf("MikroDelay usec: %ld, System Timer usec:%d\n", delay,
		   (int)dt * 1000);

	M_close(path);
	return(0);

	/*--------------------+
    |  cleanup            |
    +--------------------*/
abort:
	if( path >=0 ) M_close(path);
	printf("*** ERROR occured\n");
	return(1);
}



