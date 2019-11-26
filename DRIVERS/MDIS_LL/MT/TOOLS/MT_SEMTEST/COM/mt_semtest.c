/****************************************************************************
 ************                                                    ************
 ************                M T _ S E M T E S T                 ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: ck
 *
 *  Description: Test oss semaphores
 *
 *     Required: usr_oss, usr_utl
 *     Switches: -
 *
 *---------------------------------------------------------------------------
 * Copyright 2009-2019, MEN Mikro Elektronik GmbH
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
#include <stdlib.h>

#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>
#include <MEN/usr_utl.h>
#include <MEN/mdis_api.h>
#include <MEN/mdis_err.h>
#include <MEN/mt_drv.h>

static const char IdentString[]=MENT_XSTR(MAK_REVISION);

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#define VERBOSE(_x_) 	if (verbose) printf _x_
#define DELAY 1000
#define LOOPS 3

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
	printf("Usage: mt_semtest [<opts>] <device> [<opts>]\n");
	printf("Function: Perform Semaphore tests with the OSS LIB\n");
	printf("Options:\n");
	printf("    device       device name (MT)           [none]\n");
	printf("    -t=<ms>      semaphore timeout in msec  [%d]\n",DELAY);
	printf("    -l=<loops>   number of repeat loops     [%d]\n",LOOPS);
	printf("\n");
	printf("(c) 2009-2019 by MEN mikro elektronik GmbH\n%s\n\n",IdentString);
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
	int32 n, delay=DELAY, loop=LOOPS;
	MDIS_PATH path;
	char *device,*str,*errstr,buf[40];

	/*--------------------+
    |  check arguments    |
    +--------------------*/
	if ((errstr = UTL_ILLIOPT("t=l=?", buf))) {	/* check args */
		printf("*** %s\n", errstr);
		return(1);
	}

	if (UTL_TSTOPT("?")) { /* help requested ? */
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

	delay = ((str = UTL_TSTOPT("t=")) ? atoi(str) : DELAY);
	loop  = ((str = UTL_TSTOPT("l=")) ? atoi(str) : LOOPS);

	/*--------------------+
    |  OPEN DEVICE        |
    +--------------------*/
	if ((path = M_open(device)) < 0) {
		printf("*** can't open path: %s\n",M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	for (n = 0; n < loop ;n++ )
	{
		/*--------------------+
		|  binary semaphore   |
		+--------------------*/
		printf(" Binary Semaphores: \n");
		/* take binary semaphore */
		if( M_setstat( path, MT_SEM_BIN_TAK, delay ) < 0 ){
			printf("*** setstat MT_SEM_BIN_TAK failed: %s\n",
					M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

		/* take binary semaphore once more -> get timeout */
		if( M_setstat( path, MT_SEM_BIN_TAK, delay ) == 0 ){
			printf("*** setstat second MT_SEM_BIN_TAK timeout failed: %s\n",
					M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

		/* take binary semaphore without waitstates -> get timeout */
		if( M_setstat( path, MT_SEM_BIN_TAK, 0 ) == 0 ){
			printf("*** setstat MT_SEM_BIN_TAK timeout failed: %s\n",
					M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

		/* release binary semaphore */
		if( M_setstat( path, MT_SEM_BIN_REL, 0 ) < 0 ){
			printf("*** setstat MT_SEM_BIN_REL failed: %s\n",
					M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

		/* take binary semaphore with wait forever */
		if( M_setstat( path, MT_SEM_BIN_TAK, -1 ) < 0 ){
			printf("*** setstat MT_SEM_BIN_TAK (forever) failed: %s\n",
					M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

		/* release binary semaphore */
		if( M_setstat( path, MT_SEM_BIN_REL, 0 ) < 0 ){
			printf("*** setstat MT_SEM_BIN_REL failed: %s\n",
					M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

		/* release binary semaphore second time -> no errorcode */
		if( M_setstat( path, MT_SEM_BIN_REL, 0 ) < 0 ){
			printf("*** setstat MT_SEM_BIN_REL failed: %s\n",
					M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

		/*--------------------+
		|  counter semaphore   |
		+--------------------*/
		printf(" Counter Semaphores: \n");
		/* take counter semaphore */
		if( M_setstat( path, MT_SEM_CNT_TAK, delay ) < 0 ){
			printf("*** setstat MT_SEM_CNT_TAK failed: %s\n",
					M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

		/* take counter semaphore 2. time */
		if( M_setstat( path, MT_SEM_CNT_TAK, -1 ) < 0 ){
			printf("*** setstat second MT_SEM_CNT_TAK (2. delay) failed: %s\n",
					M_errstring(UOS_ErrnoGet()));
			goto abort;
		}
		/* take counter semaphore without waitstates -> get timeout */
		if( M_setstat( path, MT_SEM_CNT_TAK, 0 ) == 0 ){
			printf("*** setstat MT_SEM_CNT_TAK (no) failed: %s\n",
					M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

		/* release 2nd counter semaphore */
		if( M_setstat( path, MT_SEM_CNT_REL, 0 ) < 0 ){
			printf("*** setstat MT_SEM_CNT_REL failed: %s\n",
					M_errstring(UOS_ErrnoGet()));
			goto abort;
		}
		/* release counter semaphore second time */
		if( M_setstat( path, MT_SEM_CNT_REL, 0 ) < 0 ){
			printf("*** setstat MT_SEM_CNT_REL failed: %s\n",
					M_errstring(UOS_ErrnoGet()));
			goto abort;
		}
	} /* LOOP END */

	printf(" Test OK!\n");
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
