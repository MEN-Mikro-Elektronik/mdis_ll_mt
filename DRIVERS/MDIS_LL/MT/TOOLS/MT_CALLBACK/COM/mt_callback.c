/****************************************************************************
 ************                                                    ************
 ************                M T _ C A L L B A C K               ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: see
 *        $Date: 2009/10/07 10:52:55 $
 *    $Revision: 1.7 $
 *
 *  Description: Callback testing (using MT driver)
 *
 *               This program allows to install callback funtions 0 and/or 1
 *               and to activate cyclic/single callback alarm on driver.
 *
 *               For time measurement, trigger access can be configured,
 *               which is done in the callback routines.
 *
 *               NOTE: The trigger access feature can only be used
 *                     on systems, where memory mapped hardware access
 *                     can be done from applications.
 *
 *     Required: usr_oss.l usr_utl.l
 *     Switches: TRIG_ACCESS	use trigger access
 *               OS9            for OS-9 specific code (permit access)
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: mt_callback.c,v $
 * Revision 1.7  2009/10/07 10:52:55  CKauntz
 * R:1. path not 64 bit compatible
 *   2. Setstat for MSG_BLK not 64 bit compatible
 * M:1. Changed path type to MDIS_PATH
 *   2. Changed to blk cast to INT32_OR_64
 *
 * Revision 1.6  1999/07/20 12:20:11  Franke
 * cosmetics
 *
 * Revision 1.5  1999/07/05 08:14:33  kp
 * added -M option
 * adapted to new Callback API
 *
 * Revision 1.4  1999/04/16 16:26:34  Franke
 * cosmetics
 *
 * Revision 1.3  1999/03/22 15:30:31  see
 * allow msec=0 to disable callback installing
 * V1.1
 *
 * Revision 1.2  1998/09/18 14:06:40  see
 * options -c and -t added for error testing
 * time measurement removed
 * print create info
 *
 * Revision 1.1  1998/09/10 10:21:01  see
 * Added by mcvs
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

static char RCSid[]="$Id: mt_callback.c,v 1.7 2009/10/07 10:52:55 CKauntz Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef OS9
#include <process.h>
#endif

#include <MEN/men_typs.h>
#include <MEN/mdis_api.h>
#include <MEN/usr_oss.h>
#include <MEN/usr_utl.h>
#include <MEN/usr_err.h>
#include <MEN/mt_drv.h>

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
static u_int32 G_CallbackCnt_0;
static u_int32 G_CallbackCnt_1;
static u_int32 G_TrigAddr[2];
static u_int32 G_Quiet;
static u_int32 G_startTime;

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static void usage(void);

/****************************** CallbackFkt_i *******************************
 *
 *  Description: Callback functions 0 and 1
 *
 *---------------------------------------------------------------------------
 *  Input......: appArg		argument from application
 *               drvArg		argument from driver
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/

static void CallbackFkt_0(void *appArg, void *drvArg)
{
#ifdef TRIG_ACCESS
	u_int16 dummy;

	if (G_TrigAddr[0])
		dummy = *(VOLATILE u_int16*)G_TrigAddr[0];
#endif

	if (!G_Quiet)
		printf(">>> %8ld[ms]: CallbackFkt_0: appArg=0x%x, drvArg=0x%x\n",
			   UOS_MsecTimerGet()-G_startTime,
			   (intptr_t)appArg, (int)drvArg);

	G_CallbackCnt_0++;
}

static void CallbackFkt_1(void *appArg, void *drvArg)
{
#ifdef TRIG_ACCESS
	u_int16 dummy;

	if (G_TrigAddr[1])
		dummy = *(VOLATILE u_int16*)G_TrigAddr[1];
#endif

	if (!G_Quiet)
		printf(">>> %8ld[ms]: CallbackFkt_1: appArg=0x%x, drvArg=0x%x\n",
			   UOS_MsecTimerGet()-G_startTime,
			   (intptr_t)appArg, (int)drvArg);

	G_CallbackCnt_1++;
}

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
	printf("Usage: mt_callback [<opts>] <device> [<opts>]\n");
	printf("Function: Callback testing\n");
	printf("Options:\n");
	printf("    device       devicename (MT)                  [none]\n");
	printf("    -0=<msec>    callback 0 rate [msec] (0=none)  [500]\n");
	printf("    -1=<msec>    callback 1 rate [msec] (0=none)  [1000]\n");
	printf("    -m=<mode>    callback test mode               [1]\n");
	printf("                 0 = single\n");
	printf("                 1 = cyclic\n");
	printf("    -M           test masking of callbacks        [OFF]\n");
#ifdef TRIG_ACCESS
	printf("    -A=<addr>    callback 0 trigger address       [none]\n");
	printf("    -B=<addr>    callback 1 trigger address       [none]\n");
#endif
	printf("    -c           produce error: CLEAR illegal callback\n");
	printf("    -t           produce error: SET callback twice\n");
	printf("    -n=<n>       exit after n-callbacks           [endless]\n");
	printf("    -q           quiet mode                       [OFF]\n");
	printf("\n");
	printf("(c) 1998..1999 by MEN mikro elektronik GmbH\n%s\n\n",RCSid);
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
int main( int argc, char **argv )
{
    MDIS_PATH path;
	int32 n,msec[2],cyclic,maxcallbacks,error,clearErr,twiceErr,testMask;
	char *device=NULL,*str,*errstr,buf[40];
	UOS_CALLBACK_HANDLE *cbHdl;
	M_SG_BLOCK blk;
	MT_BLK_CALLBACK call;

	/*--------------------+
    |  check arguments    |
    +--------------------*/
	if ((errstr = UTL_ILLIOPT("0=1=m=n=qA=B=ctM?", buf))) {	/* check args */
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
	for (n=1; n<argc; n++)   		/* search for device */
		if (*argv[n] != '-') {
			device = argv[n];
			break;
		}

	if (!device) {						/* help requested ? */
		usage();
		return(1);
	}

	msec[0]	      = ((str=UTL_TSTOPT("0=")) ? atoi(str) : 500);
	msec[1]	      = ((str=UTL_TSTOPT("1=")) ? atoi(str) : 1000);
	cyclic	      = ((str=UTL_TSTOPT("m=")) ? atoi(str) : 1);
	maxcallbacks  = ((str=UTL_TSTOPT("n=")) ? atoi(str) : 0);
	clearErr      = (UTL_TSTOPT("c") ? 1 : 0);
	twiceErr      = (UTL_TSTOPT("t") ? 1 : 0);
	testMask      = (UTL_TSTOPT("M") ? 1 : 0);
	G_TrigAddr[0] = ((str=UTL_TSTOPT("A=")) ? UTL_Atox(str) : 0);
	G_TrigAddr[1] = ((str=UTL_TSTOPT("B=")) ? UTL_Atox(str) : 0);
	G_Quiet	      = (UTL_TSTOPT("q") ? 1 : 0);
	G_CallbackCnt_0 = 0;
	G_CallbackCnt_1 = 0;

	/*--------------------+
    |  prepare trigger    |
    +--------------------*/
#ifdef TRIG_ACCESS
	if (G_TrigAddr[0])
		printf("CALLBACK 0 trigger address=0x%08x\n", G_TrigAddr[0]);

	if (G_TrigAddr[1])
		printf("CALLBACK 1 trigger address=0x%08x\n", G_TrigAddr[1]);

# ifdef OS9
	/* permit hardware access */
	if (G_TrigAddr[0] && (error = _os_permit((void*)G_TrigAddr[0], 2, 0, 0))) {
		printf("*** can't permit access to 0x%08x: %s\n",
			   G_TrigAddr[0],M_errstring(error));
		return(1);
	}

	if (G_TrigAddr[1] && (error = _os_permit((void*)G_TrigAddr[1], 2, 0, 0))) {
		printf("*** can't permit access to 0x%08x: %s\n",
			   G_TrigAddr[1],M_errstring(error));
		return(1);
	}
# endif
#endif

	/*--------------------+
    |  init device        |
    +--------------------*/
	if ((path = M_open(device)) < 0) {
		printf("*** can't open path: %s\n",M_errstring(UOS_ErrnoGet()));
		return(1);
	}

	/*--------------------+
    |  init callback      |
    +--------------------*/
	/* create callback handle */
	if ((error = UOS_CallbackInit(path, &cbHdl))) {
		printf("*** can't UOS_CallbackInit: %s\n", UOS_ErrString(error));

		if (error == ERR_UOS_GETSTAT)
			printf("(%s)\n",M_errstring(UOS_ErrnoGet()));

		goto abort;
	}

	/*--------------------+
    |  produce errors     |
    +--------------------*/
	/* CLEAR illegal callback */
	if (clearErr) {
		printf("try to remove CALLBACK 100 routine\n");

		if ((error = UOS_CallbackClear(cbHdl, 100))) {
			printf("*** can't UOS_CallbackClear: %s\n", UOS_ErrString(error));
			if (error == ERR_UOS_SETSTAT)
				printf("(%s)\n",M_errstring(UOS_ErrnoGet()));
		}

		goto abort2;
	}

	/* SET callback twice */
	if (twiceErr) {
		if ((error = UOS_CallbackSet(cbHdl, 0, CallbackFkt_0, (void*)0xaaaa))) {
			printf("*** can't UOS_CallbackSet: %s\n", UOS_ErrString(error));
			if (error == ERR_UOS_SETSTAT)
				printf("(%s)\n",M_errstring(UOS_ErrnoGet()));
		}

		printf("try to SET CALLBACK 0 twice\n");

		if ((error = UOS_CallbackSet(cbHdl, 0, CallbackFkt_0, (void*)0xaaaa))) {
			printf("*** can't UOS_CallbackSet: %s\n", UOS_ErrString(error));
			if (error == ERR_UOS_SETSTAT)
				printf("(%s)\n",M_errstring(UOS_ErrnoGet()));
		}

		UOS_CallbackClear(cbHdl, 0);
		goto abort2;
	}

	/*--------------------+
    |  install callbacks  |
    +--------------------*/
	/* install callback functions */
	if (msec[0]) {
		printf("install CALLBACK 0 routine: appArg=0xaaaa\n");

		if ((error = UOS_CallbackSet(cbHdl, 0, CallbackFkt_0,
									 (void*)0xaaaa))) {
			printf("*** can't UOS_CallbackSet: %s\n", UOS_ErrString(error));
			if (error == ERR_UOS_SETSTAT)
				printf("(%s)\n",M_errstring(UOS_ErrnoGet()));

			goto abort;
		}
	}

	if (msec[1]) {
		printf("install CALLBACK 1 routine: appArg=0x5555\n");

		if ((error = UOS_CallbackSet(cbHdl, 1, CallbackFkt_1,
									 (void*)0x5555))) {
			printf("*** can't UOS_CallbackSet: %s\n", UOS_ErrString(error));
			if (error == ERR_UOS_SETSTAT)
				printf("(%s)\n",M_errstring(UOS_ErrnoGet()));

			goto abort;
		}
	}
	G_startTime = UOS_MsecTimerGet();

	/*--------------------+
    |  generate callbacks |
    +--------------------*/
	blk.data = &call;
	blk.size = sizeof(call);

	for (n=0; n<2; n++) {
		call.nr = n;
		call.msec = msec[n];
		call.cyclic = cyclic;

		if (call.msec) {
			printf("activate CALLBACK %ld generating: %s, %ld msec\n",
				   call.nr, call.cyclic ? "cyclic":"single", call.msec);

			if (M_setstat(path,MT_BLK_CALLBACK_SET,(INT32_OR_64)&blk) > 0) {
				printf("*** can't setstat MT_BLK_CALLBACK_SET: %s\n",
					   M_errstring(UOS_ErrnoGet()));
				goto abort;
			}
		}
	}

	/*--------------------+
    |  display callbacks  |
    +--------------------*/
	printf("waiting for callbacks %s ..\n",
		   cyclic ? "(press any key for exit)" : "");

	if( !testMask )
	{
		if( cyclic )
		{
			do {
				/* delay a bit */
				UOS_Delay(10);

				if (maxcallbacks &&
					(G_CallbackCnt_0+G_CallbackCnt_1) >= maxcallbacks)
					break;

			} while(UOS_KeyPressed() == -1);
		}
		else
			UOS_Delay(msec[0]+msec[1]);	/* delay a bit */
	}
	else {
		printf("Callbacks unmasked...\n");

		if( cyclic )
		{
			do {
				UOS_Delay(msec[0]+msec[1]);	/* delay a bit */

				UOS_CallbackMask(cbHdl);
				printf("Callbacks masked...\n");
				UOS_Delay(msec[0]+msec[1]);	/* delay a bit */

				printf("Callbacks unmasked...\n");
				UOS_CallbackUnMask(cbHdl);
				UOS_Delay(msec[0]+msec[1]);	/* delay a bit */

				if (maxcallbacks &&
					(G_CallbackCnt_0+G_CallbackCnt_1) >= maxcallbacks)
					break;

			} while(UOS_KeyPressed() == -1);
		}
		else
		{
			/* this test case is wrong, because the alarm
			   is not installed as cyclic - 0 callbacks should happens */
			UOS_Delay(msec[0]+msec[1]);	/* delay a bit */

			UOS_CallbackMask(cbHdl);
			printf("Callbacks masked...\n");
			UOS_Delay(msec[0]+msec[1]);	/* delay a bit */

			UOS_CallbackUnMask(cbHdl);
			printf("Callbacks unmasked...\n");
			UOS_Delay(msec[0]+msec[1]);	/* delay a bit */
		}/*if*/
	}
abort:
	/*--------------------+
    |  remove callbacks   |
    +--------------------*/
	if (cbHdl) {
		if (msec[0] && (error = UOS_CallbackClear(cbHdl, 0))) {
			printf("*** can't UOS_CallbackClear: %s\n", UOS_ErrString(error));
			if (error == ERR_UOS_SETSTAT)
				printf("(%s)\n",M_errstring(UOS_ErrnoGet()));
		}

		if (msec[1] && (error = UOS_CallbackClear(cbHdl, 1))) {
			printf("*** can't UOS_CallbackClear: %s\n", UOS_ErrString(error));
			if (error == ERR_UOS_SETSTAT)
				printf("(%s)\n",M_errstring(UOS_ErrnoGet()));
		}
	}

	/*--------------------+
    |  print statistics   |
    +--------------------*/
	if (msec[0])
		printf("CALLBACK 0: %ld times occured\n",G_CallbackCnt_0);

	if (msec[1])
		printf("CALLBACK 1: %ld times occured\n",G_CallbackCnt_1);

    abort2:
	/*--------------------+
    |  de-init callbacks  |
    +--------------------*/
	if (cbHdl) {
		if ((error = UOS_CallbackExit(&cbHdl)))
			printf("*** can't UOS_CallbackExit: %s\n", UOS_ErrString(error));
	}

	M_close(path);                               /* close path */
	return 0;
}










