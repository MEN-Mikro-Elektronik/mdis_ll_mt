/****************************************************************************
 ************                                                    ************
 ************                M T _ S H M E M                     ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: see
 *        $Date: 2009/10/07 10:53:16 $
 *    $Revision: 1.5 $
 *
 *  Description: Shared memory testing (using MT driver)
 *
 *               This program allows to get access to a shared memory area
 *               and to activate cyclic/single driver access to this area.
 *
 *               Access can be get by creating/linking or automatic mode,
 *               where automatic first tries to create and then to link.
 *
 *               The first long word of the shared memory area is polled and
 *               displayed endless or until the read value has n-times been
 *               incremented.
 *
 *     Required: usr_oss.l usr_utl.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: mt_shmem.c,v $
 * Revision 1.5  2009/10/07 10:53:16  CKauntz
 * R:1. path not 64 bit compatible
 *   2. Setstat for MSG_BLK not 64 bit compatible
 * M:1. Changed path type to MDIS_PATH
 *   2. Changed to blk cast to INT32_OR_64
 *
 * Revision 1.4  1999/07/05 08:17:32  kp
 * cosmetics
 *
 * Revision 1.3  1999/04/16 16:26:43  Franke
 * cosmetics
 * removed exit call
 *
 * Revision 1.2  1999/03/22 15:30:34  see
 * changes according to modified shared mem interface
 *
 * Revision 1.1  1998/09/18 14:06:53  see
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

static char RCSid[]="$Id: mt_shmem.c,v 1.5 2009/10/07 10:53:16 CKauntz Exp $";

#include <stdio.h>
#include <stdlib.h>

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
/* none */

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static void usage(void);
static void print_mdis_err(char *info);
static void print_uos_err(char *info, int32 error);


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
	printf("Usage: mt_shmem [<opts>] <device> [<opts>]\n");
	printf("Function: Shared memory testing\n");
	printf("Options:\n");
	printf("    device       devicename (MT)                     [none]\n");
	printf("    -l=<mode>    initialization mode                 [automatic]\n");
	printf("                 0 = create    (UOS_SharedMemSet)\n");
	printf("                 1 = link      (UOS_SharedMemLink)\n");
	printf("                 2 = automatic (create or link)\n");
	printf("    -i=<nr>      shared memory area index            [0]\n");
	printf("    -s=<size>    shared memory area size [bytes]     [4]\n");
	printf("    -r=<rate>    shared memory access rate [msec]    [500]\n");
	printf("    -m=<mode>    shared memory access mode           [1]\n");
	printf("                 0 = single\n");
	printf("                 1 = cyclic\n");
	printf("    -n=<n>       exit after n-accesses               [endless]\n");
	printf("    -q           quiet mode                          [OFF]\n");
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
    int32 n,msec,cyclic,error,quiet,initmode,maxacc;
	u_int32 smNr, smSize, smCreated=0;
	u_int32 startVal, currVal, dVal=0;
	char *device=NULL,*str,*errstr,buf[40];
	UOS_SHMEM_HANDLE *smHdl=NULL;
	M_SG_BLOCK blk;
	MT_BLK_SHMEM sm;
	u_int32 *appAddr=NULL;

	/*--------------------+
    |  check arguments    |
    +--------------------*/
	if ((errstr = UTL_ILLIOPT("i=s=r=m=n=l=q?", buf))) {	/* check args */
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

	smNr	 = ((str=UTL_TSTOPT("i=")) ? atoi(str) : 0);
	smSize	 = ((str=UTL_TSTOPT("s=")) ? atoi(str) : 4);
	msec	 = ((str=UTL_TSTOPT("r=")) ? atoi(str) : 500);
	cyclic	 = ((str=UTL_TSTOPT("m=")) ? atoi(str) : 1);
	maxacc	 = ((str=UTL_TSTOPT("n=")) ? atoi(str) : 0);
	initmode = ((str=UTL_TSTOPT("l=")) ? atoi(str) : 2);
	quiet	 = (UTL_TSTOPT("q") ? 1 : 0);

	if (!IN_RANGE(initmode,0,2)) {
		printf("*** illegal initmode\n");
		return(1);
	}

	/*--------------------+
    |  init device        |
    +--------------------*/
	if ((path = M_open(device)) < 0) {
		print_mdis_err("open path");
		return(1);
	}

	/*----------------------------+
    |  init shared memory         |
    +----------------------------*/
	if ((error = UOS_SharedMemInit(path, &smHdl))) {
		print_uos_err("UOS_SharedMemInit", error);
		goto abort;
	}

	/*----------------------------+
    |  create/link shared memory  |
    +----------------------------*/
	switch(initmode) {
		/*--------------+
		|  create       |
		+--------------*/
		case 0:
			printf("try to create shared memory: nr=%u size=%u\n",
					 (unsigned int)smNr, (unsigned int)smSize);

			if ((error = UOS_SharedMemSet(smHdl, smNr, smSize, (void*)&appAddr))) {
				print_uos_err("UOS_SharedMemSet", error);
				goto abort;
			}

			printf("created shared memory: nr=%u size=%u addr=0x%08x\n",
				   (unsigned int)smNr, (unsigned int)smSize, (unsigned int)appAddr);
			smCreated = 1;
			break;
		/*--------------+
		|  link         |
		+--------------*/
		case 1:
			printf("try to link to shared memory: nr=%u\n", (unsigned int)smNr);

			if ((error = UOS_SharedMemLink(smHdl, smNr, &smSize, (void*)&appAddr))) {
				print_uos_err("UOS_SharedMemLink", error);
				goto abort;
			}

			printf("linked to shared memory: nr=%u size=%u addr=0x%08x\n",
				   (unsigned int)smNr, (unsigned int)smSize, (unsigned int)appAddr);
			break;
		/*--------------+
		|  automatic    |
		+--------------*/
		case 2:
			printf("try to create shared memory: nr=%u size=%u\n",
					(unsigned int)smNr, (unsigned int)smSize);

			/* try to create shared memory */
			if ((error = UOS_SharedMemSet(smHdl, smNr, smSize, (void*)&appAddr))) {
				/* if already exist: link to shared memory */
				if (error == ERR_UOS_SETSTAT) {
					printf("try to link to shared memory: nr=%u\n",(unsigned int)smNr);

					if ((error = UOS_SharedMemLink(smHdl, smNr, &smSize,
												   (void*)&appAddr))) {
						print_uos_err("UOS_SharedMemLink", error);
						goto abort;
					}

					printf("linked to shared memory: nr=%u size=%u addr=0x%08x\n",
						   (unsigned int)smNr, (unsigned int)smSize, (unsigned int)appAddr);
				}
				/* if other error: exit */
				else {
					print_uos_err("UOS_SharedMemSet", error);
					goto abort;
				}
			}
			else {
				printf("created shared memory: nr=%u size=%u addr=0x%08x\n",
					   (unsigned int)smNr, (unsigned int)smSize, (unsigned int)appAddr);
				smCreated = 1;
			}

			break;
	}

	/*--------------------------------+
    |  activate shared memory access  |
    |  if shared mem was created      |
    +--------------------------------*/
	if (smCreated) {
		blk.data = &sm;
		blk.size = sizeof(sm);

		sm.nr     = smNr;
		sm.msec   = msec;
		sm.cyclic = cyclic;

		printf("activate shared memory access: %s, %u msec\n",
			   sm.cyclic ? "cyclic":"single", (unsigned int)sm.msec);

		if (M_setstat(path,MT_BLK_SHMEM_SET,(INT32_OR_64)&blk)) {
			print_mdis_err("setstat MT_BLK_SHMEM_SET");
			goto abort;
		}
	}

	/*--------------------------------+
    |  scan shared memory             |
    +--------------------------------*/
	printf("monitoring shared memory %s\n", "(press any key for exit)" );

	startVal = *appAddr;

	do {
		/* wait 10ms */
		UOS_Delay(10);

		/* read/print value */
		currVal = *appAddr;

		if (!quiet) {
			printf("%08x\b\b\b\b\b\b\b\b", (unsigned int)currVal);
			fflush(stdout);
		}

		/* check for n-accesses */
		dVal = currVal - startVal;

		if (maxacc && dVal >= maxacc)
			break;

	} while( UOS_KeyPressed() == -1 );

	if (!quiet)
		printf("\nshared memory access: %u times occured\n", (unsigned int)dVal);

	/*--------------------------------+
    |  de-activate shared memory acc. |
    +--------------------------------*/
	if (smCreated) {
		printf("de-activate shared memory access\n");

		if (M_setstat(path,MT_SHMEM_CLR,smNr) < 0)
			print_mdis_err("setstat MT_BLK_SHMEM_CLR");
	}

	abort:
	/*----------------------------+
    |  cleanup shared memory      |
    +----------------------------*/
	if (smHdl) {
		/* unlink */
		if (appAddr && (error = UOS_SharedMemClear(smHdl, smNr)))
			print_uos_err("UOS_SharedMemClear", error);

		/* exit */
		if ((error = UOS_SharedMemExit(&smHdl)))
			print_uos_err("UOS_SharedMemExit", error);
	}

	M_close(path);                               /* close path */
	return( 0 );
}/*main*/


void print_mdis_err(char *info)
{
	printf("*** can't %s: %s\n", info, M_errstring(UOS_ErrnoGet()));
}


void print_uos_err(char *info, int32 error)
{
	if (error == ERR_UOS_SETSTAT)
		print_mdis_err(info);
	else
		printf("*** can't %s: %s\n", info, UOS_ErrString(error));
}





