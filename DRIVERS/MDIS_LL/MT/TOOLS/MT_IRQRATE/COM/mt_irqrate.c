/****************************************************************************
 ************                                                    ************
 ************                M T _ I R Q R A T E                 ************
 ************                                                    ************
 ****************************************************************************
 *  
 *       Author: see
 *
 *  Description: Interrupt generator (using M99 driver)
 *                      
 *     Required: usr_oss.l usr_utl.l
 *     Switches: -
 *
 *
 *---------------------------------------------------------------------------
 * Copyright (c) 1996-2019, MEN Mikro Elektronik GmbH
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
#include <MEN/m99_drv.h>

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
	printf("Usage: mt_irqrate [<opts>] <device> [<opts>]\n");
	printf("Function: Interrupt generator\n");
	printf("Options:\n");
	printf("    device     devicename (M99)        [none]\n");
	printf("    -r=<rate>  irq rate (1..250000 Hz) [1]\n");
	printf("    -j         irq rate jitter enable  [off]\n");
	printf("    -n=<n>     exit after n interrupts [endless]\n");
	printf("\n");
	printf("Copyright (c) 1996-2019, MEN Mikro Elektronik GmbH\n%s\n\n",IdentString);
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




