/****************************************************************************
 ************                                                    ************
 ************                 M T _ S P I N L T E S T            ************
 ************                                                    ************
 ****************************************************************************
 *  
 *       Author: dieter.pfeuffer@men.de
 *        $Date: 2010/12/01 12:20:20 $
 *    $Revision: 1.1 $
 *
 *  Description: Verify OSS_SpinLockXxx functions
 *
 *               Main thread (endless loop until keypress):
 *               - open specified number of pathes    
 *               - perform specified number of getstats with open pathes
 *               - close specified number of pathes
 *                      
 *     Required: usr_oss.l usr_utl.l
 *     Switches: none
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: mt_spinltest.c,v $
 * Revision 1.1  2010/12/01 12:20:20  dpfeuffer
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2010 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/

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

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static void usage(void);
static char IdentString[]="$Id: mt_spinltest.c,v 1.1 2010/12/01 12:20:20 dpfeuffer Exp $\n";

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
	printf("Usage: mt_spinltest [<opts>] <device> [<opts>]\n");
	printf("Function: Verify OSS_SpinLockXxx functions\n");
	printf("Options:\n");
	printf("    device       device name                            [none]\n");
	printf("    -d=<ms>      ms [ms] between M_getstat calls        [11]\n");
	printf("    -v           verbose                                [off]\n");
	printf("\n");
	printf("(c) 2010 by MEN mikro elektronik GmbH, V%.15s\n\n", IdentString+22);
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
	char *str, *errstr, buf[40];
	int32 verbose, ms;
	int32 n, err=0, yesErr=0, noErr=0;
	int32 noCount=0, yesCount=0, refCount=0;
	char *device;
	MDIS_PATH path=0;
	
	/*--------------------+
    |  check arguments    |
    +--------------------*/
	if ((errstr = UTL_ILLIOPT("d=v?", buf))) {	/* check args */
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
	for (device=NULL, n=1; n<argc; n++){
		if (*argv[n] != '-') {
			device = argv[n];
			break;
		}
	}

	if (!device) {	
		usage();
		return(1);
	}

	ms       = ((str = UTL_TSTOPT("d=")) ? atoi(str) : 11);
	verbose  = (UTL_TSTOPT("v") ? 1 : 0);


	/*--------------------+
    |  open               |
    +--------------------*/
	VERBOSE(("open path to %s\n", device));

	if ((path = M_open(device)) < 0) {
		err = UOS_ErrnoGet();
		printf("*** M_open failed: %s\n", M_errstring(err));
		goto abort;
	}

	printf("Press any key to abort...\n");
	do {
		/*--------------------+
	    |  getstats           |
	    +--------------------*/
		if ((M_getstat( path, MT_SPINL_NO, &noCount )) < 0) {
			err = UOS_ErrnoGet();
			printf("*** M_getstat MT_SPINL_NO failed: %s\n",
			  M_errstring(err));
			goto abort;
		}

		if ((M_getstat( path, MT_SPINL_YES, &yesCount )) < 0) {
			err = UOS_ErrnoGet();
			printf("*** M_getstat MT_SPINL_YES failed: %s\n",
			  M_errstring(err));
			goto abort;
		}
		UOS_Delay(ms);

		refCount++;
		
		VERBOSE(("ref=%08d,  spinlock=%08d, nolock=%08d\n",
			refCount, yesCount, noCount));
		
		/* should never occur !*/	
		if( refCount != yesCount+yesErr ){
			printf("*** spin lock failed (ref=%d, spinlock=%d)\n",
				refCount, yesCount);
			yesErr++;
		}

		/* may occur at SMP */	
		if( refCount != noCount+noErr ){
			printf("--- no lock race condition (ref=%d, nolock=%d)\n",
				refCount, noCount);
			noErr++;
		}

	} while(UOS_KeyPressed() == -1);

	abort:
	/*--------------------+
    |  result             |
    +--------------------*/
	printf("TEST RESULT: ");
	if( noErr && !yesErr ){
		printf("SUCCESS\n");
	} else if ( yesErr ){
		printf("*** FAILED ***\n");
	}
	else{
		printf("UNDEFINED (please repeat longer at SMP HW)\n");
	}
	printf("ref=%d,  spinlock=%d (%d errors), nolock=%d (%d errors)\n",
		refCount, yesCount, yesErr, noCount, noErr);

	/*--------------------+
    |  close              |
    +--------------------*/
	if (path)
		VERBOSE(("close path\n"));
		if ((M_close(path)) < 0) {
			err = UOS_ErrnoGet();
			printf("*** can't close path: %s\n",
				M_errstring(err));
		}

	return 0;
}

