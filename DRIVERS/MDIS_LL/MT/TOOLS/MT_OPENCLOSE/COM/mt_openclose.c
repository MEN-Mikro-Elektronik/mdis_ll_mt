/****************************************************************************
 ************                                                    ************
 ************                 M T _ O P E N C L O S E            ************
 ************                                                    ************
 ****************************************************************************
 *  
 *       Author: dieter.pfeuffer@men.de
 *
 *  Description: Verify MDIS open/close locking
 *
 *               Main thread (endless loop until keypress):
 *               - open specified number of pathes    
 *               - perform specified number of getstats with open pathes
 *               - close specified number of pathes
 *             
 *               For Windows only: 
 *               Second thread (endless until main thread terminates):
 *               - open local path
 *               - getstat with local path
 *               - getstat with open pathes from main thread
 *               - close local path
 *                      
 *               NOTE: any MDIS driver can be used for this test                     
 *                      
 *     Required: usr_oss.l usr_utl.l
 *     Switches: WINNT - compile for Windows
 *
 *
 *---------------------------------------------------------------------------
 * Copyright (c) 2010-2019, MEN Mikro Elektronik GmbH
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

#ifdef WINNT
#include <windows.h>
#endif /* WINNT */

#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>
#include <MEN/usr_utl.h>
#include <MEN/mdis_api.h>

static const char IdentString[]=MENT_XSTR(MAK_REVISION);

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#define VERBOSE(_x_) 	if (ctx.verbose) printf _x_

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
typedef struct {
	MDIS_PATH path;
	int32 opened;
} PATH;

typedef struct {
	int32 pathes;
	int32 verbose;
	int32 thread;
	char *device;	
	PATH *pt;
} THREAD_CTX;

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static void usage(void);
#ifdef WINNT
	static int32 ThreadEntry( THREAD_CTX *ctx );
#endif /* WINNT */

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
	printf("Usage: mt_openclose [<opts>] <device> [<opts>]\n");
	printf("Function: Verify MDIS open/close locking\n");
	printf("Options:\n");
	printf("    device       device name                            [none]\n");
	printf("    -p=<max>     number of opened pathes                [10]\n");
	printf("    -g=<n>       number of getstats at each path        [10]\n");
	printf("    -d=<ms>      ms [ms] between each MDIS_API call     [1]\n");
#ifdef WINNT
	printf("    -t=<ms>      additional getstats from second thread\n");
	printf("                  all <ms>. <ms>=0: no second thread    [0]\n");
#endif /* WINNT */
	printf("    -v           verbose                                [off]\n");
	printf("\n");
	printf("Copyright (c) 2010-2019, MEN Mikro Elektronik GmbH\n%s\n\n", IdentString);
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
	int32 getstats, ms;
	int32 n, p, g, count=0, size, err=0;
	int32 data;
	THREAD_CTX ctx;
#ifdef WINNT
	u_int32	thrId;
	HANDLE thrH=NULL;
#endif /* WINNT */
	
	/*--------------------+
    |  check arguments    |
    +--------------------*/
	if ((errstr = UTL_ILLIOPT("p=g=d=t=v?", buf))) {	/* check args */
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
	for (ctx.device=NULL, n=1; n<argc; n++){
		if (*argv[n] != '-') {
			ctx.device = argv[n];
			break;
		}
	}

	if (!ctx.device) {	
		usage();
		return(1);
	}

	ctx.pathes   = ((str = UTL_TSTOPT("p=")) ? atoi(str) : 10);
	getstats = ((str = UTL_TSTOPT("g=")) ? atoi(str) : 10);
	ms       = ((str = UTL_TSTOPT("d=")) ? atoi(str) : 1);
	ctx.verbose  = (UTL_TSTOPT("v") ? 1 : 0);

	ctx.pt = NULL;

	/*--------------------+
    |  alloc mem          |
    +--------------------*/
	size = ctx.pathes * sizeof(PATH);

	if ((ctx.pt = (PATH*)malloc(size)) == NULL) {
		printf("*** can't alloc %ld bytes\n", size);
	    goto abort;
	}

	for (p=0; p<ctx.pathes; p++)		/* clear all */
		ctx.pt[p].opened = FALSE;

#ifdef WINNT
	ctx.thread = ((str = UTL_TSTOPT("t=")) ? atoi(str) : 0);
	if (ctx.thread){
		/* create new thread */
		if( (thrH = CreateThread( NULL, 0,
				(u_int32 (__stdcall *)(VOID*))ThreadEntry,
				&ctx, 0, &thrId )) == NULL ) {
			printf("*** CreateThread() failed (0x%x)\n", GetLastError());
			goto abort;
		}
	}
#endif /* WINNT */

	do {
		printf("run #%d: open %d pathes, %d getstats per path, close %d pathes, delay=%dms\n",
			++count, ctx.pathes, getstats, ctx.pathes, ms);
		printf("---------- press any key to abort ----------\n");
		/*--------------------+
	    |  open pathes        |
	    +--------------------*/
		VERBOSE((" open %d pathes\n", ctx.pathes));

		for (p=0; p<ctx.pathes; p++) {
			if ((ctx.pt[p].path = M_open(ctx.device)) < 0) {
				err = UOS_ErrnoGet();
				printf("*** can't open path[%d]: %s\n",
					p, M_errstring(err));
				goto abort;
			}
			ctx.pt[p].opened = TRUE;
			UOS_Delay(ms);
		}

		/*--------------------+
	    |  getstats           |
	    +--------------------*/
		for (p=0; p<ctx.pathes; p++) {

			VERBOSE((" %d getstats for path[%d]\n", getstats, p));

			for (g=0; g<getstats; g++) {
				if ((M_getstat( ctx.pt[p].path, M_LL_CH_NUMBER, &data )) < 0) {
					err = UOS_ErrnoGet();
					printf("*** M_getstat failed for path[%d]: %s\n",
					 p, M_errstring(err));
					goto abort;
				}
				UOS_Delay(ms);
			}
		}

		/*--------------------+
	    |  close pathes       |
	    +--------------------*/
		VERBOSE((" close %d pathes\n", ctx.pathes));

		for (p=0; p<ctx.pathes; p++) {
			ctx.pt[p].opened = FALSE;

			if ((M_close(ctx.pt[p].path)) < 0) {
				err = UOS_ErrnoGet();
				printf("*** can't close path[%d]: %s\n",
					p, M_errstring(err));
				goto abort;
			}
			UOS_Delay(ms);
		}

	} while(UOS_KeyPressed() == -1);

	/*--------------------+
    |  cleanup            |
    +--------------------*/
	abort:

	if( err ){
		for (p=0; p<ctx.pathes; p++) {
			if (ctx.pt[p].opened) {
				if ((M_close(ctx.pt[p].path)) < 0)
					printf("*** can't close path[%d]: %s\n",
						 p, M_errstring(UOS_ErrnoGet()));
			}
		}
	}

#ifdef WINNT
	if (thrH)
		CloseHandle( thrH );
#endif /* WINNT */

	if (ctx.pt)
		free(ctx.pt);

	return 0;
}

#ifdef WINNT
/********************************** ThreadEntry *****************************
 *
 *  Description: Thread entry function
 *                         
 *---------------------------------------------------------------------------
 *  Input......:  ---
 *  Output.....:  ---
 *  Globals....:  several
 ****************************************************************************/
static int32 ThreadEntry( THREAD_CTX *ctx )
{
	int32 p;
	int32 data;
	MDIS_PATH localPath;

	while ( 1 ){

		/* open local path */
		if ((localPath = M_open(ctx->device)) < 0) {
			printf(" SECOND THREAD: *** can't open localPath: %s\n",
				M_errstring(UOS_ErrnoGet()));
		}

		for (p=0; p<ctx->pathes; p++) {

			
			/* getstat with local path */
			if ((M_getstat( localPath, M_LL_DEBUG_LEVEL, &data )) < 0) {
				printf(" SECOND THREAD: *** WARNING: M_getstat failed for localPath: %s\n",
				 M_errstring(UOS_ErrnoGet()));
			}

			/* getstat with open pathes from main thread */
			if( ctx->pt[p].opened ){
				if (ctx->verbose)
					printf(" SECOND THREAD: getstat for path[%d]\n", p);
				/* Note: path may be closed between 'if' and 'M_getstat' */
				if ((M_getstat( ctx->pt[p].path, M_LL_DEBUG_LEVEL, &data )) < 0) {
					printf(" SECOND THREAD: *** WARNING: M_getstat failed for path[%d]: %s\n",
					 p, M_errstring(UOS_ErrnoGet()));
				}
				UOS_Delay( ctx->thread );
			}
		}
	
		/* close local path */
		if ((M_close(localPath)) < 0) {
			printf(" SECOND THREAD: *** can't close localPath: %s\n",
				M_errstring(UOS_ErrnoGet()));
		}
	
	}

	return 0;
}
#endif /* WINNT */
