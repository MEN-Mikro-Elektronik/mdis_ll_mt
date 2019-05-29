/****************************************************************************
 ************                                                    ************
 ************                 M T _ S L E E P                    ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: see
 *        $Date: 2009/10/07 10:53:18 $
 *    $Revision: 1.6 $
 *
 *  Description: Sleep in MDIS call and calculate elapsed time
 *
 *               Calculates elapsed time and check for timeout
 *               (e.g. for testing process locking)
 *
 *               NOTE: "MT" driver must be used for this test
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

static const char RCSid[]="$Id: mt_sleep.c,v 1.6 2009/10/07 10:53:18 CKauntz Exp $";

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
#define REV "V1.1"      /* program revision */

#define VERBOSE(_x_) 	if (verbose && !quiet) printf _x_

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/

/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
char *callName[6] = {
	"READ", "WRITE", "BLOCKREAD", "BLOCKWRITE", "GETSTAT", "SETSTAT"
};

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
	printf("Usage: mt_sleep [<opts>] <device> [<opts>]\n");
	printf("Function: Sleep in MDIS call and calculate elapsed time\n");
	printf("Options:\n");
	printf("    device       device name (MT)                [none]\n");
	printf("    -c=<chan>    channel number                  [0]\n");
	printf("                 0..3 = channel\n");
	printf("    -m=<call>    MDIS call                       [0]\n");
	printf("                 0 = M_read\n");
	printf("                 1 = M_write\n");
	printf("                 2 = M_getblock\n");
	printf("                 3 = M_setblock\n");
	printf("                 4 = M_getstat\n");
	printf("                 5 = M_setstat\n");
	printf("    -t=<msec>    time to sleep [msec]            [300]\n");
	printf("    -l           loop mode                       [off]\n");
	printf("    -q           quiet mode (print only errors)  [off]\n");
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
    MDIS_PATH path=0;
	int32 chan,call,msec,loopmode,quiet;
	int32 n,value,code,ret,dt,t1,tickrate,tickmsec,timeout;
	char *device,*str,*errstr,buf[40];
	u_int8 bufdata;
	u_int8 *blkbuf = &bufdata;

	/*--------------------+
    |  check arguments    |
    +--------------------*/
	if ((errstr = UTL_ILLIOPT("c=m=t=lq?", buf))) {	/* check args */
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

	chan      = ((str = UTL_TSTOPT("c=")) ? atoi(str) : 0);
	call      = ((str = UTL_TSTOPT("m=")) ? atoi(str) : 0);
	msec      = ((str = UTL_TSTOPT("t=")) ? atoi(str) : 300);
	loopmode  = (UTL_TSTOPT("l") ? 1 : 0);
	quiet     = (UTL_TSTOPT("q") ? 1 : 0);

	if (!IN_RANGE(chan,0,3)) {
		printf("*** illegal channel\n");
		return 1;
	}

	if (!IN_RANGE(call,0,5)) {
		printf("*** illegal MDIS call\n");
		return 1;
	}

	/*--------------------+
    |  open path          |
    |  set current chan   |
    +--------------------*/
	if ((path = M_open(device)) < 0) {
		printf("*** can't open path: %s\n",M_errstring(UOS_ErrnoGet()));
		return 1;
	}

	if ((M_setstat(path, M_MK_CH_CURRENT, chan)) < 0) {
		printf("*** can't setstat M_MK_CH_CURRENT: %s\n",
			   M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	/*--------------------+
    |  calc timeout       |
    +--------------------*/
	if ((M_getstat(path, M_MK_TICKRATE, &tickrate)) < 0) {
		printf("*** can't getstat M_MK_TICKRATE: %s\n",
			   M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	tickmsec = 1000 / tickrate;

	if (msec < (3*tickmsec)) {
		printf("*** sleeptime too less (minimum=%ld)\n",(3*tickmsec));
		goto abort;
	}

	timeout = (3 * msec) / 2;

	/*--------------------+
    |  config one-shot    |
    +--------------------*/
	t1 = UOS_MsecTimerGet();

	if ((M_setstat(path, MT_ONE_SLEEP, !loopmode)) < 0) {
		printf("*** can't setstat MT_xxx_SLEEP: %s\n",
			   M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	dt = UOS_MsecTimerGet() - t1;

	/* check if SETSTAT timeout already occured */
	/* (channel or call is already locked) */
	if (dt >= timeout) {
		printf("call %-10s function (chan=%ld): ","SETSTAT",chan);
		printf(" %04ld msec elapsed  *TIMEOUT*\n",dt);
		printf("(timeout when setting one-shot sleepmode)\n");
		goto abort;
	}

	/*--------------------+
    |  config sleep mode  |
    +--------------------*/
	switch(call) {
		case 0:	code = MT_READ_SLEEP;			break;
		case 1:	code = MT_WRITE_SLEEP;			break;
		case 2:	code = MT_BLOCKREAD_SLEEP;		break;
		case 3:	code = MT_BLOCKWRITE_SLEEP;		break;
		case 4:	code = MT_GETSTAT_SLEEP;		break;
		case 5:	code = MT_SETSTAT_SLEEP;		break;
	    default: goto abort;
	}

	t1 = UOS_MsecTimerGet();

	if ((M_setstat(path, code, msec)) < 0) {
		printf("*** can't setstat MT_xxx_SLEEP: %s\n",
			   M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	dt = UOS_MsecTimerGet() - t1;

		/* check if SETSTAT timeout already occured */
		/* (channel or call is already locked) */
	if (dt >= timeout) {
		printf("call %-10s function (chan=%ld): ","SETSTAT",chan);
		printf(" %04ld msec elapsed  *TIMEOUT*\n",dt);
		printf("(timeout when setting %s sleepmode)\n",callName[call]);
		goto abort;
	}

	/*--------------------+
    |  call loop ...      |
    +--------------------*/
	do {
		if (!quiet) {
			printf("call %-10s function (chan=%ld): ",callName[call],chan);
			fflush(stdout);
		}

		t1 = UOS_MsecTimerGet();

		switch(call) {
			case 0:	ret = M_read(path, &value);		break;
			case 1:	ret = M_write(path, value);		break;
			case 2:	ret = M_getblock(path, blkbuf, 0);	break;
			case 3:	ret = M_setblock(path, blkbuf, 0);	break;
			case 4:	ret = M_getstat(path, MT_NOACTION, &value);		break;
			case 5:	ret = M_setstat(path, MT_NOACTION, value);		break;
		    default: ret=-1; break;
		}

		dt = UOS_MsecTimerGet() - t1;

		if (ret < 0)
			printf("*** %s\n",M_errstring(UOS_ErrnoGet()));
		else {
			if (!quiet) {
				printf(" %04ld msec elapsed",dt);

				if (dt >= timeout)
					printf("  *TIMEOUT*\n");
				else
					printf("\n");
			}
		}

	} while(loopmode && ret>=0);

	/*--------------------+
    |  clear sleep mode   |
    |  (if loopmode)      |
    +--------------------*/
	if (loopmode) {
		t1 = UOS_MsecTimerGet();

		if ((M_setstat(path, code, 0)) < 0) {
			printf("*** can't setstat MT_xxx_SLEEP: %s\n",
				   M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

		dt = UOS_MsecTimerGet() - t1;

		/* check if SETSTAT timeout already occured */
		/* (channel or call is already locked) */
		if (dt >= timeout) {
			printf("*** timeout when clearing %s sleepmode\n",callName[call]);
			goto abort;
		}
	}

	/*--------------------+
    |  cleanup            |
    +--------------------*/
	abort:

	if (M_close(path) < 0)
		printf("*** can't close path: %s\n",M_errstring(UOS_ErrnoGet()));

	return 0;
}



