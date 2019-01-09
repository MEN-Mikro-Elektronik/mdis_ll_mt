/****************************************************************************
 ************                                                    ************
 ************                 M T _ C A L L                      ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: see
 *        $Date: 2009/10/07 10:52:52 $
 *    $Revision: 1.4 $
 *
 *  Description: Execute MDIS call and calculate elapsed time
 *
 *               Calculates elapsed time and check for timeout
 *               (e.g. for testing process locking)
 *
 *               NOTE: "MT" driver must be used for this test
 *
 *     Required: usr_oss.l usr_utl.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: mt_call.c,v $
 * Revision 1.4  2009/10/07 10:52:52  CKauntz
 * R: path not 64 bit compatible
 * M: Changed path type to MDIS_PATH
 *
 * Revision 1.3  1999/07/05 08:12:50  kp
 * cosmetics
 *
 * Revision 1.2  1999/04/16 16:26:38  Franke
 * cosmetics
 *
 * Revision 1.1  1998/09/10 10:21:06  see
 * Added by mcvs
 *
 * cloned from mt_sleep.c Revision 1.2
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Id: mt_call.c,v 1.4 2009/10/07 10:52:52 CKauntz Exp $";

#include <stdio.h>

#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>
#include <MEN/usr_utl.h>
#include <MEN/mdis_api.h>
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
	printf("Usage: mt_call [<opts>] <device> [<opts>]\n");
	printf("Function: Execute MDIS call and calculate elapsed time\n");
	printf("Options:\n");
	printf("    device       device name (MT)                [none]\n");
	printf("    -c=<chan>    channel number                  [none]\n");
	printf("                 0..3 = channel\n");
	printf("    -m=<call>    MDIS call                       [0]\n");
	printf("                 0 = M_read\n");
	printf("                 1 = M_write\n");
	printf("                 2 = M_getblock\n");
	printf("                 3 = M_setblock\n");
	printf("                 4 = M_getstat\n");
	printf("                 5 = M_setstat\n");
	printf("    -t=<msec>    timeout [msec]                  [0]\n");
	printf("                 0 = no timeout\n");
	printf("    -l           loop mode                       [off]\n");
	printf("    -q           quiet mode (print only errors)  [off]\n");
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
int main( int  argc, char *argv[] )
{
    MDIS_PATH path=0;
	int32 chan,call,loopmode,quiet;
	int32 n,value,ret,dt,t1,timeout;
	char *device,*str,*errstr,buf[40];
	u_int8 bufdata;
	u_int8 *blkbuf = &bufdata;

	ret = 0;
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

	chan      = ((str = UTL_TSTOPT("c=")) ? atoi(str) : -1);
	call      = ((str = UTL_TSTOPT("m=")) ? atoi(str) : 0);
	timeout   = ((str = UTL_TSTOPT("t=")) ? atoi(str) : 0);
	loopmode  = (UTL_TSTOPT("l") ? 1 : 0);
	quiet     = (UTL_TSTOPT("q") ? 1 : 0);

	if (chan != -1 && !IN_RANGE(chan,0,3)) {
		printf("*** illegal channel\n");
		return(1);
	}

	if (!IN_RANGE(call,0,5)) {
		printf("*** illegal MDIS call\n");
		return(1);
	}

	/*--------------------+
    |  open path          |
    |  set current chan   |
    +--------------------*/
	if ((path = M_open(device)) < 0) {
		printf("*** can't open path: %s\n",M_errstring(UOS_ErrnoGet()));
		return(1);
	}

	if (chan != -1)
		if ((M_setstat(path, M_MK_CH_CURRENT, chan)) < 0) {
			printf("*** can't setstat M_MK_CH_CURRENT: %s\n",
				   M_errstring(UOS_ErrnoGet()));
			goto ABORT;
		}

	/*--------------------+
    |  call loop ...      |
    +--------------------*/
	do {
		if (!quiet) {
			if (chan != -1)
				printf("call %-10s function (chan=%d): ",callName[call],(int)chan);
			else
				printf("call %-10s function          : ",callName[call]);

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
		}

		dt = UOS_MsecTimerGet() - t1;

		if (ret < 0)
			printf("*** %s\n",M_errstring(UOS_ErrnoGet()));
		else {
			if (!quiet) {
				printf(" %04d msec elapsed",(int)dt);

				if (timeout && (dt >= timeout))
					printf("  *TIMEOUT*\n");
				else
					printf("\n");
			}
		}

	} while(loopmode && ret>=0);

	/*--------------------+
    |  cleanup            |
    +--------------------*/
ABORT:
	if (M_close(path) < 0)
		printf("*** can't close path: %s\n",M_errstring(UOS_ErrnoGet()));
	return( 0 );
}


