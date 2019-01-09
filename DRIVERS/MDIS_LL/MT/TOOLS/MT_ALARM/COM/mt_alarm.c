/****************************************************************************
 ************                                                    ************
 ************                M T _ A L A R M                     ************
 ************                                                    ************
 ****************************************************************************
 *  
 *       Author: see
 *        $Date: 2009/10/07 10:52:45 $
 *    $Revision: 1.7 $
 *
 *  Description: Alarm testing (using MT driver)
 *                      
 *     Required: usr_oss.l usr_utl.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: mt_alarm.c,v $
 * Revision 1.7  2009/10/07 10:52:45  CKauntz
 * R:1. path not 64 bit compatible
 *   2. Setstat for MSG_BLK not 64 bit compatible
 * M:1. Changed path type to MDIS_PATH
 *   2. Changed to blk cast to INT32_OR_64
 *
 * Revision 1.6  1999/07/20 12:19:03  Franke
 * cosmetics
 *
 * Revision 1.5  1999/07/05 08:09:51  kp
 * cosmetics
 *
 * Revision 1.4  1998/09/18 14:06:13  see
 * bug fixed: error check was slightly wrong
 *
 * Revision 1.3  1998/09/10 10:20:38  see
 * changes according to new UOS/UTL release
 * include usr_err.h
 * don't call SigMask
 *
 * Revision 1.2  1998/07/02 16:56:04  see
 * OS-9 code removed, changed for using UOS and UTL lib
 * include usr_oss.h and usr_utl.h
 *
 * Revision 1.1  1998/07/01 16:46:35  see
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/
 
static char RCSid[]="$Id: mt_alarm.c,v 1.7 2009/10/07 10:52:45 CKauntz Exp $";

#include <stdio.h>

#include <MEN/men_typs.h>
#include <MEN/mdis_api.h>
#include <MEN/usr_oss.h>
#include <MEN/usr_err.h>
#include <MEN/usr_utl.h>
#include <MEN/mt_drv.h>

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
	printf("(c) 1998 by MEN mikro elektronik GmbH, %s\n\n",RCSid);
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
		alm.signal = UOS_SIG_USR1+n;

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
			case UOS_SIG_USR1+0: info = "ALARM 0";	alarmcnt++;		break;
			case UOS_SIG_USR1+1: info = "ALARM 1";	alarmcnt++;		break;
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





