/****************************************************************************
 ************                                                    ************
 ************                M T _ I R Q M A X                   ************
 ************                                                    ************
 ****************************************************************************
 *  
 *       Author: see
 *        $Date: 2009/10/07 10:53:00 $
 *    $Revision: 1.7 $
 *
 *  Description: Test maximum interrupt rate (using M99 driver)
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
 
static char RCSid[]="$Id: mt_irqmax.c,v 1.7 2009/10/07 10:53:00 CKauntz Exp $";

#include <stdio.h>
#include <stdlib.h>

#include <MEN/men_typs.h>
#include <MEN/mdis_api.h>
#include <MEN/usr_oss.h>
#include <MEN/usr_utl.h>
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
	printf("Usage: mt_irqmax [<opts>] <device> [<opts>]\n");
	printf("Function: Test maximum interrupt rate\n");
	printf("Options:\n");
	printf("    device     devicename (M99)        [none]\n");
    printf("    -d=<diff>  max deviation (%%)       [10]\n");
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
int main(int argc, char **argv )
{
	MDIS_PATH path;
	int32 n,rate=0,irqcnt,diff,maxdiff,irqrate,timerval;
	char *device=NULL,*str,*errstr,buf[40];

	/*--------------------+
    |  check arguments    |
    +--------------------*/
	if ((errstr = UTL_ILLIOPT("d=?", buf))) {	/* check args */
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
	
	maxdiff = ((str = UTL_TSTOPT("d=")) ? atoi(str) : 10);

	/*--------------------+
    |  init device        |
    +--------------------*/
	if ((path = M_open(device)) < 0) {
		printf("*** can't open path: %s\n",M_errstring(UOS_ErrnoGet()));
		return 1;
	}

	if (M_setstat(path,M99_JITTER,0)	  < 0   ||   /* no jitter */
		M_setstat(path,M_MK_IRQ_ENABLE,1) < 0) {     /* enable interrupt */
		printf("*** can't setstat: %s\n",M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	timerval = 250;
	do {
		if( timerval > 20 )
			timerval /= 2;
		else
			timerval--;

		rate = 250000 / (timerval+1);

		printf("irqrate [Hz]: generated=%ld ",rate);
		fflush(stdout);

		if (M_setstat(path,M99_TIMERVAL,timerval) < 0   ||   /* set irq rate */
			M_setstat(path,M_MK_IRQ_COUNT,0)      < 0   ||   /* clr irq counter */
			M_setstat(path,M_MK_IRQ_ENABLE,1)     < 0) {     /* enable irq */
			printf("*** can't setstat: %s\n",M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

		UOS_Delay(500);

		if (M_setstat(path,M_MK_IRQ_ENABLE,0) < 0) {        /* disable irq */
			printf("*** can't setstat: %s\n",	
				   M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

		if (M_getstat(path,M_MK_IRQ_COUNT,&irqcnt) < 0) {   /* get irq counter */
			printf("*** can't getstat M_MK_IRQ_COUNT: %s\n",
				   M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

		irqrate = irqcnt * 2;
		diff = ((irqrate-rate)*100)/rate;

		printf("measured=%ld (%ld%%)\n",irqrate,diff);

	} while (abs(diff) < maxdiff && timerval>1);

	if( abs(diff) >= maxdiff )
		printf("max deviataion reached\n");
	else 
		printf("sorry, no higher timer frequency available!\n");
	abort:
	M_close(path);                               /* close path */
	return 0;
}




