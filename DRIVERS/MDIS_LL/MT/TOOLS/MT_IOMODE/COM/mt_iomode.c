/****************************************************************************
 ************                                                    ************
 ************                 M T _ I O M O D E                  ************
 ************                                                    ************
 ****************************************************************************
 *  
 *       Author: see
 *        $Date: 2009/10/07 10:52:57 $
 *    $Revision: 1.6 $
 *
 *  Description: Verify MDIS channel i/o mode handling (using MT driver)
 *               - set/verify current channel
 *               - no autoincrement mode
 *               - autoincrement mode after success
 *               - autoincrement mode after error
 *                      
 *               NOTE: "MT" driver must be used for this test
 *                      
 *     Required: usr_oss.l usr_utl.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: mt_iomode.c,v $
 * Revision 1.6  2009/10/07 10:52:57  CKauntz
 * R: path not 64 bit compatible
 * M: Changed path type to MDIS_PATH
 *
 * Revision 1.5  1999/07/20 12:20:21  Franke
 * cosmetics
 *
 * Revision 1.4  1999/07/05 08:14:51  kp
 * cosmetics
 *
 * Revision 1.3  1998/09/10 10:20:41  see
 * ported to changed USR_UTL calls
 *
 * Revision 1.2  1998/07/02 16:56:10  see
 * OS-9 code removed, changed for using UOS and UTL lib
 * include usr_oss.h and usr_utl.h
 *
 * Revision 1.1  1998/07/01 16:46:37  see
 * Initial Revision
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
 
static const char RCSid[]="$Id: mt_iomode.c,v 1.6 2009/10/07 10:52:57 CKauntz Exp $";

#include <stdio.h>
#include <stdlib.h>

#include <MEN/men_typs.h>
#include <MEN/mdis_err.h>
#include <MEN/mdis_api.h>
#include <MEN/usr_oss.h>
#include <MEN/usr_utl.h>
#include <MEN/mt_drv.h>

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#define REV "V1.0"      /* program revision */

#define VERBOSE(_x_) 	if (verbose) printf _x_

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
typedef struct {
	MDIS_PATH path;
	int32 chan;
	int32 opened;
} IOMTEST;

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
	printf("Usage: mt_iomode [<opts>] <device> [<opts>]\n");
	printf("Function: Verify MDIS channel i/o mode\n");
	printf("Options:\n");
	printf("    device       device name (MT)      [none]\n");
	printf("    -n=<max>     nr of opened pathes   [3]\n");
	printf("    -v           verbose               [off]\n");
	printf("\n");
	printf("(c) 1998 by MEN mikro elektronik GmbH, %s\n\n",REV);
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
int main(int  argc, char *argv[])
{
	int32 pathMax,chanMax,verbose;
	int32 chan,n,size,value,rand=0;
	char *device,*str,*errstr,buf[40];
	IOMTEST *pt = NULL;		/* ptr to path data array */

	/*--------------------+
    |  check arguments    |
    +--------------------*/
	if ((errstr = UTL_ILLIOPT("n=v?", buf))) {	/* check args */
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

	if (!device) {						/* help requested ? */
		usage();
		return(1);
	}

	pathMax   = ((str = UTL_TSTOPT("n=")) ? atoi(str) : 3);
	verbose   = (UTL_TSTOPT("v") ? 1 : 0);

	/*--------------------+
    |  alloc mem          |
    +--------------------*/
	size = pathMax * sizeof(IOMTEST);

	if ((pt = (IOMTEST*)malloc(size)) == NULL) {
		printf("*** can't alloc %ld bytes\n",size);
	    goto abort;
	}

	for (n=0; n<pathMax; n++)		/* clear all */
		pt[n].opened = FALSE;

	/*--------------------+
    |  open pathes        |
    +--------------------*/
	VERBOSE(("open %ld pathes\n",pathMax));

	for (n=0; n<pathMax; n++) {
		if ((pt[n].path = M_open(device)) < 0) {
			printf("*** can't open path[%ld]: %s\n",
				   n,M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

		pt[n].chan   = 0;
		pt[n].opened = TRUE;
	}

	/*--------------------+
    |  get nr of channels |
    +--------------------*/
	if ((M_getstat(pt[0].path, M_LL_CH_NUMBER, &chanMax)) < 0) {
		printf("*** can't setstat M_LL_CH_NUMBER: %ld %s\n",
			   n,M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	VERBOSE(("device has %ld channels\n",chanMax));

	if (chanMax < 2) {
		printf("*** too less channels for this test");
		goto abort;
	}

	/*--------------------+
    |  channel range      |
    +--------------------*/
	printf("--- channel range test ---\n");
	printf("check legal/illegal channel numbers\n");
	
	VERBOSE(("set channels %d..%ld\n",0,chanMax-1));

	for (chan=0; chan<chanMax; chan++) {
		if ((M_setstat(pt[0].path, M_MK_CH_CURRENT, chan)) < 0) {
			printf("*** can't setstat M_MK_CH_CURRENT: %ld %s\n",
				   chan,M_errstring(UOS_ErrnoGet()));
			goto abort;
		}
	}

	VERBOSE(("set illegal channel=%ld\n",chanMax));

	if ((M_setstat(pt[0].path, M_MK_CH_CURRENT, chanMax)) >= 0) {
		printf("*** error expected when setting illegal channel=%ld: %s\n",
				 chan,M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	if (UOS_ErrnoGet() != ERR_MK_ILL_PARAM) {
		printf("*** unexpected error code: %s\n",M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	printf("ok.\n");

	/*--------------------+
    |  set/verify chan    |
    +--------------------*/
	printf("--- set/verify channel ---\n");
	printf("set/verify channels of various pathes\n");

	/* set current channel of all pathes */
	for (n=0; n<pathMax; n++) {
		rand = UOS_Random(rand);
		pt[n].chan = UOS_RandomMap(rand, 0, chanMax-1);
		VERBOSE((" path[%ld]: set channel=%ld\n",n,pt[n].chan));

		if ((M_setstat(pt[n].path, M_MK_CH_CURRENT, pt[n].chan)) < 0) {
			printf("*** can't setstat M_MK_CH_CURRENT: %s\n",
				   M_errstring(UOS_ErrnoGet()));
			goto abort;
		}
	}

	/* verify current channel of all pathes */
	VERBOSE(("verify channels\n"));

	for (n=0; n<pathMax; n++) {
		if ((M_getstat(pt[n].path, M_MK_CH_CURRENT, &chan)) < 0) {
			printf("*** can't getstat M_MK_CH_CURRENT: %s\n",
				   M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

		if (chan != pt[n].chan) {
			printf("*** path[%ld]: wrong channel=%ld\n",n,pt[n].chan);
			goto abort;
		}
	}

	printf("ok.\n");

	/*--------------------+
    |  no autoinc mode    |
    +--------------------*/
	printf("--- no autoincrement ---\n");
	printf("check channel after successful READ\n");
	VERBOSE(("set channel=0, iomode=M_IO_EXEC\n"));

	/* set chan=0 */
	if ((M_setstat(pt[0].path, M_MK_CH_CURRENT, 0)) < 0) {
		printf("*** can't setstat M_MK_CH_CURRENT: %s\n",
			   M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	/* set iomode=IO_EXEC */
	if ((M_setstat(pt[0].path, M_MK_IO_MODE, M_IO_EXEC)) < 0) {
		printf("*** can't setstat M_MK_IO_MODE: %s\n",
			   M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	/* call READ */
	VERBOSE(("read from channel\n"));
	if ((M_read(pt[0].path, &value)) < 0) {
		printf("*** can't read: %s\n",M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	/* get/check chan */
	if ((M_getstat(pt[0].path, M_MK_CH_CURRENT, &chan)) < 0) {
		printf("*** can't getstat M_MK_CH_CURRENT: %s\n",
			   M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	if (chan != 0) {
		printf("*** channel has changed: channel=%ld\n",chan);
		goto abort;
	}
	else
		VERBOSE(("channel=%ld, not incremented\n",chan));

	printf("ok.\n");

	/*--------------------+
    |  autoinc (success)  |
    +--------------------*/
	printf("--- autoincrement mode (success) ---\n");
	VERBOSE(("set channel=0, iomode=M_IO_EXEC_INC\n"));

	/* set chan=0 */
	if ((M_setstat(pt[0].path, M_MK_CH_CURRENT, 0)) < 0) {
		printf("*** can't setstat M_MK_CH_CURRENT: %s\n",
			   M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	/* set iomode=IO_EXEC_INC */
	if ((M_setstat(pt[0].path, M_MK_IO_MODE, M_IO_EXEC_INC)) < 0) {
		printf("*** can't setstat M_MK_IO_MODE: %s\n",M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	printf("check channel after successful READ\n");

	/* for chan=0..chanMax */
	for (n=0; n<chanMax; n++) {
		/* call READ */
		VERBOSE(("read from channel=%ld\n",n));
		if ((M_read(pt[0].path, &value)) < 0) {
			printf("*** can't read: %s\n",M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

		/* get/check chan */
		if ((M_getstat(pt[0].path, M_MK_CH_CURRENT, &chan)) < 0) {
			printf("*** can't getstat M_MK_CH_CURRENT: %s\n",
				   M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

		if (n==chanMax-1) {
			if (chan != 0) {		/* check wrapping */
				printf("*** channel not wrapped: channel=%ld\n",chan);
				goto abort;
			}
			else
				VERBOSE(("channel=%ld, wrapped\n",chan));
		}
		else {
			if (chan != n+1) {		/* check increment */
				printf("*** channel increment error: channel should=%ld <=> is=%ld\n",
						 n+1,chan);
				goto abort;
			}
			else
				VERBOSE(("channel=%ld, incremented\n",chan));
		}
	}

	printf("ok.\n");

	printf("check channel after successful WRITE\n");

	/* for chan=0..chanMax */
	for (n=0; n<chanMax; n++) {
		/* call WRITE */
		VERBOSE(("write to channel=%ld\n",n));
		if ((M_write(pt[0].path, 0x00)) < 0) {
			printf("*** can't write: %s\n",M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

		/* get/check chan */
		if ((M_getstat(pt[0].path, M_MK_CH_CURRENT, &chan)) < 0) {
			printf("*** can't getstat M_MK_CH_CURRENT: %s\n",
				   M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

		if (n==chanMax-1) {
			if (chan != 0) {		/* check wrapping */
				printf("*** channel not wrapped: channel=%ld\n",chan);
				goto abort;
			}
			else
				VERBOSE(("channel=%ld, wrapped\n",chan));
		}
		else {
			if (chan != n+1) {		/* check increment */
				printf("*** channel increment error: channel should=%ld <=> is=%ld\n",
						 n+1,chan);
				goto abort;
			}
			else
				VERBOSE(("channel=%ld, incremented\n",chan));
		}
	}

	printf("ok.\n");

	/*--------------------+
    |  autoinc (error)    |
    +--------------------*/
	printf("--- autoincrement mode (error) ---\n");
	VERBOSE(("set channel=0, iomode=M_IO_EXEC_INC\n"));

	/* set chan=0 */
	if ((M_setstat(pt[0].path, M_MK_CH_CURRENT, 0)) < 0) {
		printf("*** can't setstat M_MK_CH_CURRENT: %s\n",
			   M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	/* set iomode=IO_EXEC_INC */
	if ((M_setstat(pt[0].path, M_MK_IO_MODE, M_IO_EXEC_INC)) < 0) {
		printf("*** can't setstat M_MK_IO_MODE: %s\n",
			   M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	printf("check channel after failed READ\n");

	/* set read error mode */
	if ((M_setstat(pt[0].path, MT_READ_ERROR, TRUE)) < 0) {
		printf("*** can't setstat MT_READ_ERROR: %s\n",
			   M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	/* call READ */
	VERBOSE(("read (error) from channel\n"));
	if ((M_read(pt[0].path, &value)) >= 0) {
		printf("*** read should produce error");
		goto abort;
	}

	/* get/check chan */
	if ((M_getstat(pt[0].path, M_MK_CH_CURRENT, &chan)) < 0) {
		printf("*** can't getstat M_MK_CH_CURRENT: %s\n",
			   M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	if (chan != 0) {
		printf("*** channel has changed: channel=%ld\n",chan);
		goto abort;
	}
	else
		VERBOSE(("channel not incremented\n"));

	printf("ok.\n");

	printf("check channel after failed WRITE\n");

	/* set write error mode */
	if ((M_setstat(pt[0].path, MT_WRITE_ERROR, TRUE)) < 0) {
		printf("*** can't setstat MT_WRITE_ERROR: %s\n",
			   M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	/* call WRITE */
	VERBOSE(("write (error) to channel\n"));
	if ((M_write(pt[0].path, 0x00)) >= 0) {
		printf("*** write should produce error");
		goto abort;
	}

	/* get/check chan */
	if ((M_getstat(pt[0].path, M_MK_CH_CURRENT, &chan)) < 0) {
		printf("*** can't getstat M_MK_CH_CURRENT: %s\n",
			   M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	if (chan != 0) {
		printf("*** channel has changed: channel=%ld\n",chan);
		goto abort;
	}
	else
		VERBOSE(("channel not incremented\n"));

	printf("ok.\n");

	/*--------------------+
    |  cleanup            |
    +--------------------*/
	abort:

	for (n=0; n<pathMax; n++)
		if (pt[n].opened) {
			if (M_close(pt[n].path) < 0)
				printf("*** can't close path: %s\n",
					   M_errstring(UOS_ErrnoGet()));
		}

	if (pt)
		free(pt);

	return 0;
}


