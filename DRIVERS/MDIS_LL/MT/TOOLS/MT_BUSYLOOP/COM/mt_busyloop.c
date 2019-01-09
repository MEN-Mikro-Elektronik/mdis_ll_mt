/****************************************************************************
 ************                                                    ************
 ************                 M T _ B U S Y L O O P              ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: kp
 *        $Date: 2009/10/07 10:52:50 $
 *    $Revision: 1.3 $
 *
 *  Description: Perform MT driver's MT_BUSYLOOP setstat
 *
 *               NOTE: "MT" driver must be used for this test
 *
 *     Required: usr_oss.l usr_utl.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: mt_busyloop.c,v $
 * Revision 1.3  2009/10/07 10:52:50  CKauntz
 * R: path not 64 bit compatible
 * M: Changed path type to MDIS_PATH
 *
 * Revision 1.2  1999/07/20 12:19:57  Franke
 * cosmetics
 *
 * Revision 1.1  1999/07/05 08:12:13  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Id: mt_busyloop.c,v 1.3 2009/10/07 10:52:50 CKauntz Exp $";

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
#define LOOPS 1000000

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
	printf("Usage: mt_busyloop [<opts>] <device> [<opts>]\n");
	printf("Function: Perform MDIS MT drivers MT_BUSYLOOP setstat\n");
	printf("Options:\n");
	printf("    device       device name (MT)      [none]\n");
	printf("    -n=<n>       nr of busy loops      [%d]\n",LOOPS);
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
	int32 n,loops=LOOPS,dt,t1;
	char *device,*str,*errstr,buf[40];

	/*--------------------+
    |  check arguments    |
    +--------------------*/
	if ((errstr = UTL_ILLIOPT("n=?", buf))) {	/* check args */
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

	loops     = ((str = UTL_TSTOPT("n=")) ? atoi(str) : LOOPS);

	/*--------------------+
    |  OPEN DEVICE        |
    +--------------------*/
	if ((path = M_open(device)) < 0) {
		printf("*** can't open path: %s\n",M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	t1 = UOS_MsecTimerGet();

	if( M_setstat( path, MT_BUSYLOOP, loops ) < 0 ){
		printf("*** setstat MT_BUSYLOOP failed: %s\n",
			   M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	dt = UOS_MsecTimerGet() - t1;


	printf("elapsed time        : %-5ld loops  %-4d total msec\n", loops,
		   (int)dt);

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



