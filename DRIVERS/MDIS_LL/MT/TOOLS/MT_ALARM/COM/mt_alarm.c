/****************************************************************************
 ************                                                    ************
 ************                M T _ A L A R M                     ************
 ************                                                    ************
 ****************************************************************************
 *  
 *       Author: see
 *
 *  Description: Alarm testing (using MT driver)
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
 

#include <stdlib.h>
#include <stdio.h>

#include <MEN/men_typs.h>
#include <MEN/mdis_api.h>
#include <MEN/usr_oss.h>
#include <MEN/usr_err.h>
#include <MEN/usr_utl.h>
#include <MEN/mt_drv.h>

static const char IdentString[]=MENT_XSTR(MAK_REVISION);

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/

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
 *  Description: Print program usage and exit
 *			   
 *---------------------------------------------------------------------------
 *  Input......: no		exitcode
 *  Output.....: 
 *  Globals....: -
 ****************************************************************************/
static void usage(void)
{
	printf("Usage: mt_alarm [<opts>] <device> [<opts>]\n");
	printf("Function: Alarm testing\n");
	printf("Options:\n");
	printf("    device       devicename (MT)         [none]\n");
	printf("    -0=<msec>    alarm 0 rate [msec]     [1000]\n");
	printf("    -1=<msec>    alarm 1 rate [msec]     [1000]\n");
	printf("    -m=<mode>    alarm mode              [1]\n");
	printf("                 0 = single\n");
	printf("                 1 = cyclic\n");
	printf("    -n=<n>       exit after n-alarms     [endless]\n");
	printf("\n");
	printf("Copyright (c) 1998-2019, MEN Mikro Elektronik GmbH\n%s\n\n", IdentString);
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
	int32 n,msec[2],cyclic,maxalarms,alarmcnt=0,error;
	u_int32 signal, timeout=0;
	char *device=NULL,*str,*info,*errstr,buf[40];
	M_SG_BLOCK blk;
	MT_BLK_ALARM alm;

	/*--------------------+
    |  check arguments    |
    +--------------------*/
	if ((errstr = UTL_ILLIOPT("0=1=m=n=j?", buf))) {	/* check args */
		printf("*** %s\n", errstr);
		return(1);
	}

	if (UTL_TSTOPT("?")) {						/* help requested ? */
		usage();
		return(1);
	}

	UOS_SigInit(NULL);
	UOS_SigInstall(UOS_SIG_USR1);
	UOS_SigInstall(UOS_SIG_USR2);

	/*--------------------+
    |  get arguments      |
    +--------------------*/
	for (n=1; n<argc; n++)   		/* search for device */
		if (*argv[n] != '-') {
			device = argv[n];
			break;
		}

	if (!device) {
		usage();
		return(1);
	}

	msec[0]	  = ((str=UTL_TSTOPT("0=")) ? atoi(str) : 1000);
	msec[1]	  = ((str=UTL_TSTOPT("1=")) ? atoi(str) : 1000);
	cyclic	  = ((str=UTL_TSTOPT("m=")) ? atoi(str) : 1);
	maxalarms = ((str=UTL_TSTOPT("n=")) ? atoi(str) : 0);

	/*--------------------+
    |  init device        |
    +--------------------*/
	if ((path = M_open(device)) < 0) {
		printf("*** can't open path: %s\n",M_errstring(UOS_ErrnoGet()));
		return 1;
	}

	blk.data = &alm;
	blk.size = sizeof(alm);

	for (n=0; n<2; n++) {
		alm.nr = n;
		alm.msec = msec[n];
		alm.cyclic = cyclic;
		alm.signal = (n == 0) ? UOS_SIG_USR1 : UOS_SIG_USR2;

		if (alm.msec) {
			timeout = (alm.msec > timeout ? alm.msec : timeout);

			printf("install ALARM %ld: %s, signal=0x%04x, %d msec\n",
				   alm.nr, alm.cyclic ? "cyclic":"single", (int)alm.signal, (int)alm.msec);

			if (M_setstat(path,MT_BLK_ALARM_SET,(INT32_OR_64)&blk) < 0) {
				printf("*** can't setstat MT_BLK_ALARM_SET: %s\n",
					   M_errstring(UOS_ErrnoGet()));
				goto abort;
			}
		}
	}

	timeout *= 2;

	printf("waiting for signals (%ld msec timeout) ..\n", timeout);

	if (cyclic)
		printf("(press any key for exit)\n");

	do {
		if( (error = UOS_SigWait(timeout, &signal)) ) {	/* wait for signals ... */
			switch (error) {
				case ERR_UOS_TIMEOUT:
					if (cyclic)
						printf("*** timeout: no signal received\n");
					else
						printf("success: no more signals received\n");

					break;
				case ERR_UOS_ABORTED:
					printf("*** aborted\n");
					break;
				default:
					printf("*** error=0x%04x\n",(int)error);
			}

			goto abort;
		}

		switch(signal) {
			case UOS_SIG_USR1: info = "ALARM 0";	alarmcnt++;		break;
			case UOS_SIG_USR2: info = "ALARM 1";	alarmcnt++;		break;
			default:			 info = "(unknown)";
		}

		printf(">> signal 0x%04x - %s\n",(int)signal,info);

		if (maxalarms && alarmcnt==maxalarms)
			break;

	} while(UOS_KeyPressed() == -1);

abort:
	printf("(%ld alarms occured)\n",alarmcnt);

	UOS_SigExit();
	M_close(path);                               /* close path */
	return 0;
}





