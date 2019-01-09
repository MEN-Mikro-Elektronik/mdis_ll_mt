/****************************************************************************
 ************                                                    ************
 ************                M T _ I R Q R A T E                 ************
 ************                                                    ************
 ****************************************************************************
 *  
 *       Author: see
 *        $Date: 2009/10/07 10:53:02 $
 *    $Revision: 1.7 $
 *
 *  Description: Interrupt generator (using M99 driver)
 *                      
 *     Required: usr_oss.l usr_utl.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: mt_irqrate.c,v $
 * Revision 1.7  2009/10/07 10:53:02  CKauntz
 * R: path not 64 bit compatible
 * M: Changed path type to MDIS_PATH
 *
 * Revision 1.6  1999/07/20 12:20:40  Franke
 * cosmetics
 *
 * Revision 1.5  1999/07/05 08:16:24  kp
 * cosmetics
 *
 * Revision 1.4  1998/09/18 14:06:28  see
 * bug fixed: error check was slightly wrong
 *
 * Revision 1.3  1998/09/10 10:20:48  see
 * ported to changed USR_UTL calls
 *
 * Revision 1.2  1998/07/02 16:56:20  see
 * OS-9 code removed, changed for using UOS and UTL lib
 * include usr_oss.h and usr_utl.h
 *
 * Revision 1.1  1998/07/01 16:46:42  see
 * Initial Revision
 *
 * cloned from m99n_irqs.c
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1996 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/
 
static char RCSid[]="$Id: mt_irqrate.c,v 1.7 2009/10/07 10:53:02 CKauntz Exp $";

#include <stdio.h>
#include <stdlib.h>

#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>
#include <MEN/usr_utl.h>
#include <MEN/mdis_api.h>
#include <MEN/m99_drv.h>

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
	printf("Usage: mt_irqrate [<opts>] <device> [<opts>]\n");
	printf("Function: Interrupt generator\n");
	printf("Options:\n");
	printf("    device     devicename (M99)        [none]\n");
	printf("    -r=<rate>  irq rate (1..250000 Hz) [1]\n");
	printf("    -j         irq rate jitter enable  [off]\n");
	printf("    -n=<n>     exit after n interrupts [endless]\n");
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
	int32 n,rate,jitter,irqcnt,maxirqs,timerval;
	char *device=NULL,*str,*errstr,buf[40];

	/*--------------------+
    |  check arguments    |
    +--------------------*/
	if ((errstr = UTL_ILLIOPT("r=n=j?", buf))) {	/* check args */
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

	if (!device) {
		usage();
		return(1);
	}
	
	rate	= ((str=UTL_TSTOPT("r=")) ? atoi(str) : 1);
	maxirqs	= ((str=UTL_TSTOPT("n=")) ? atoi(str) : 0);
	jitter	= (UTL_TSTOPT("j") ? 1 : 0);

	if (!IN_RANGE(rate,1,250000)) {
		printf("*** illegal rate\n");
		return 1;
	}

	timerval = 250000 / rate;

	/*--------------------+
    |  init device        |
    +--------------------*/
	if ((path = M_open(device)) < 0) {
		printf("*** can't open path: %s\n",M_errstring(UOS_ErrnoGet()));
		return 1;
	}

	if (M_setstat(path,M99_JITTER,jitter)	  < 0   ||   /* set jitter */
		M_setstat(path,M99_TIMERVAL,timerval) < 0   ||   /* set irq rate */
		M_setstat(path,M_MK_IRQ_ENABLE,1)     < 0) {     /* enable interrupt */
		printf("*** can't setstat: %s\n",M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	printf("generating interrupts: %ld Hz, %s\n",
		   rate,(jitter ? "jittered" : "no jitter"));
	printf("(press any key for exit)\n");
	printf("IRQS=");

	while (UOS_KeyPressed() == -1) {
		UOS_Delay(200);

		if (M_getstat(path,M_MK_IRQ_COUNT,&irqcnt) < 0) {
			printf("*** can't getstat M_MK_IRQ_COUNT: %s\n",
				   M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

		printf("%08ld\b\b\b\b\b\b\b\b",irqcnt);
		fflush(stdout);

		if (maxirqs && irqcnt>=maxirqs)
			break;
	}

	printf("\n");

	abort:
	M_close(path);                               /* close path */
	return 0;
}




