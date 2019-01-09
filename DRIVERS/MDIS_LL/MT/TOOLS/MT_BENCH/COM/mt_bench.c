/****************************************************************************
 ************                                                    ************
 ************                 M T _ B E N C H                    ************
 ************                                                    ************
 ****************************************************************************
 *  
 *       Author: see
 *        $Date: 2009/10/07 10:52:47 $
 *    $Revision: 1.8 $
 *
 *  Description: MDIS benchmark test using MT driver
 *                      
 *               NOTE: "MT" driver must be used for this test                     
 *                      
 *     Required: usr_oss.l usr_utl.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: mt_bench.c,v $
 * Revision 1.8  2009/10/07 10:52:47  CKauntz
 * R: path not 64 bit compatible
 * M: Changed path type to MDIS_PATH
 *
 * Revision 1.7  2004/03/19 11:08:00  ub
 * QNX settings added
 *
 * Revision 1.6  2002/06/11 16:00:22  DSchmidt
 * VCIRTX defines were wrong
 *
 * Revision 1.5  2000/08/29 16:42:09  Schmidt
 * 1) VCIRTX specific includes moved to VCIRTX section
 * 2) WINNT specifics added
 *
 * Revision 1.4  2000/06/14 14:38:09  Schmidt
 * macros for high resolution timer added
 * timer resolution now from UOS_MsecTimerResolution function, cosmetics
 * added VCIRTX switches
 *
 * Revision 1.3  1999/07/05 08:10:31  kp
 * added OS9000 switches
 * cosmetics
 *
 * Revision 1.2  1999/04/14 17:31:21  Franke
 * removed bug if first M_open dt != tickres
 * cosmetics
 *
 * Revision 1.1  1998/09/10 10:21:11  see
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/
 
static const char RCSid[]="$Id: mt_bench.c,v 1.8 2009/10/07 10:52:47 CKauntz Exp $";

#include <stdio.h>

#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>
#include <MEN/usr_utl.h>
#include <MEN/mdis_api.h>
#include <MEN/mt_drv.h>

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#define TASKLOCK
#define TASKUNLOCK

#ifdef WINNT
#include <windows.h>
# define MAX_OPENS 1000	/* max number of opened paths */
# define CALLS   10000  /* number of function callcnt */
# define OPENS   10     /* number of opened paths */
# define HRES_VAR 															  \
	LARGE_INTEGER	rt1,rt2,rf;												  \
	double			rdt, rres;												  \
	QueryPerformanceFrequency( &rf );										  \
	rres = (double)1000000 / (double)rf.u.LowPart;							  \
	printf("NT High-Resolution Timer : resolution=%f usec per tick\n", rres);

# define HRES_GET_TIME1 {				\
	QueryPerformanceCounter( &rt1 );	\
}
# define HRES_GET_TIME2 {				\
	QueryPerformanceCounter( &rt2 );	\
}
# define HRES_PRINT {														\
	rdt = ((double)(rt2.u.LowPart-rt1.u.LowPart)) * rres;					\
	printf("                    :              %8.3f total usec"            \
           "  %8.3f usec/call\n", rdt, rdt/callcnt);						\
}
# define HRES_PRINT_OPENCLOSE {												\
	rdt = ((double)(rt2.u.LowPart-rt1.u.LowPart)) * rres;					\
	printf("                    :              %8.3f total usec  "          \
           "%8.3f usec/call\n", rdt, rdt);                                  \
}
# define HRES_PRINT_OPENCLOSE_FURTHER {										\
	rdt = ((double)(rt2.u.LowPart-rt1.u.LowPart)) * rres;					\
	printf("                    :              %8.3f total usec  "          \
           "%8.3f usec/call\n",  rdt, rdt/pathcnt);							\
}
#endif

#ifdef OS9
# define MAX_OPENS 256	/* max number of opened paths */
# define CALLS   10000  /* number of function callcnt */
# define OPENS   10     /* number of opened paths */
#endif

#ifdef OS9000
# define MAX_OPENS 256	/* max number of opened paths */
# define CALLS   100000 /* number of function callcnt */
# define OPENS   28     /* number of opened paths */
#endif

#ifdef VCIRTX
#include <windows.h>
#include <rtapi.h>
# define MAX_OPENS 256	/* max number of opened paths */
# define CALLS   10000  /* number of function callcnt */
# define OPENS   10     /* number of opened paths */
# define HRES_VAR 				\
	LARGE_INTEGER	rt1,rt2;	\
	u_int32			rdt;
# define HRES_GET_TIME1 {						\
	RtGetClockTime( CLOCK_FASTEST, &rt1 );		\
}
# define HRES_GET_TIME2 {						\
	RtGetClockTime( CLOCK_FASTEST, &rt2 );		\
}
# define HRES_PRINT {																\
	rdt = (rt2.u.LowPart-rt1.u.LowPart)/10;											\
	printf("                    :              %6d total usec  %8.3f usec/call\n",	\
		  rdt, (float)rdt/callcnt);													\
}
# define HRES_PRINT_OPENCLOSE {																\
	rdt = (rt2.u.LowPart-rt1.u.LowPart)/10;											\
	printf("                    :              %6d total usec  %8.3f usec/call\n",	\
		  rdt, (float)rdt);													\
}
# define HRES_PRINT_OPENCLOSE_FURTHER {																\
	rdt = (rt2.u.LowPart-rt1.u.LowPart)/10;											\
	printf("                    :              %6d total usec  %8.3f usec/call\n",	\
		  rdt, (float)rdt/pathcnt);													\
}
#endif

#ifdef VXWORKS
# define MAX_OPENS	40	/* max number of opened paths */
# define CALLS   10000  /* number of function callcnt */
# define OPENS      10  /* number of opened paths */

# include <taskLib.h>
# undef  TASKLOCK
# undef  TASKUNLOCK
# define TASKLOCK	(taskLock())
# define TASKUNLOCK  (taskUnlock())
#endif

#ifdef __QNX__
# define MAX_OPENS 256	  /* max number of opened paths */
# define CALLS     100000 /* number of function callcnt */
# define OPENS     28     /* number of opened paths */
#endif


#ifndef HRES_VAR
# define HRES_VAR
#endif
#ifndef HRES_GET_TIME1
# define HRES_GET_TIME1
#endif
#ifndef HRES_GET_TIME2
# define HRES_GET_TIME2
#endif
#ifndef HRES_PRINT
# define HRES_PRINT
#endif
#ifndef HRES_PRINT_OPENCLOSE
# define HRES_PRINT_OPENCLOSE
#endif
#ifndef HRES_PRINT_OPENCLOSE_FURTHER
# define HRES_PRINT_OPENCLOSE_FURTHER
#endif

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
	printf("Usage: mt_bench [<opts>] <device> [<opts>]\n");
	printf("Function: MDIS benchmark test\n");
	printf("Options:\n");
	printf("    device       device name (MT)      [none]\n");
	printf("    -p=<n>       nr of opened pathes   [%d]\n",OPENS);   
	printf("    -c=<n>       nr of function calls  [%d]\n",CALLS);   
	printf("\n");
	printf("(c) 1998..2000 by MEN mikro elektronik GmbH\n%s\n\n",RCSid);
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
	int32 callcnt,pathcnt,tickres;
	MDIS_PATH path[MAX_OPENS+1];
	int32 rdval, wrval;
	int32 n,dt,t1;
	char *device,*str,*errstr,buf[40];
	u_int8 blkbuf;
	HRES_VAR;

    /* init local variables */
    for( n=0; n<(MAX_OPENS+1); n++ )
		path[n] = -1;

	/*--------------------+
    |  check arguments    |
    +--------------------*/
	if ((errstr = UTL_ILLIOPT("p=c=?", buf))) {	/* check args */
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

	callcnt     = ((str = UTL_TSTOPT("c=")) ? atoi(str) : CALLS);
	pathcnt     = ((str = UTL_TSTOPT("p=")) ? atoi(str) : OPENS);

    printf("mt_bench pathcount=%d callcount=%d devicename=%s\n\n",
			(int)pathcnt, (int)callcnt, device ); 

	if (pathcnt > MAX_OPENS) {
		printf("*** too many pathes (max=%d)\n",MAX_OPENS);
		return(1);
	}

	/*--------------------+
    |  initial OPEN       |
    +--------------------*/
    TASKLOCK;
	t1 = UOS_MsecTimerGet(); HRES_GET_TIME1;

	if ((path[0] = M_open(device)) < 0) {
		printf("*** can't open path: %s\n",M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	dt = UOS_MsecTimerGet() - t1; HRES_GET_TIME2;
    TASKUNLOCK;

	tickres = UOS_MsecTimerResolution();

	if (M_close(path[0]) < 0) 
	{
		printf("*** can't close path: %s\n",M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	printf("Initial OPEN        : %5d calls  %6d total msec  ", 1, (int)dt);
	printf("%8.3f msec/call\n", (float)dt);
	HRES_PRINT_OPENCLOSE;

	/*----------------------------------------------+
	|  OPEN+CLOSE becaus one open is to slow        |
   	+----------------------------------------------*/
	{

	    TASKLOCK;
		t1 = UOS_MsecTimerGet();
		n = 0;

		do {
			if ((path[0] = M_open(device)) < 0) {
				printf("*** can't open path: %s\n",M_errstring(UOS_ErrnoGet()));
				goto abort;
			}

			if (M_close(path[0]) < 0) {
				printf("*** can't close path: %s\n",M_errstring(UOS_ErrnoGet()));
				goto abort;
			}

			n++;
			dt = UOS_MsecTimerGet() - t1;
		
		} while ((dt = UOS_MsecTimerGet() - t1) <= tickres);
	    TASKUNLOCK;

		printf("Initial OPEN+CLOSE  : %5d calls  %6d total msec  ", (int)n, (int)dt);
		printf("%8.3f msec/(OPEN+CLOSE)\n", (float)dt/n);
	}

	if ((path[0] = M_open(device)) < 0) {
		printf("*** can't open path: %s\n",M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	/*--------------------+
    |  further OPEN's     |
    +--------------------*/
	t1 = UOS_MsecTimerGet(); HRES_GET_TIME1;

	for (n=1; n<=pathcnt; n++)
		if ((path[n] = M_open(device)) < 0) {
			printf("*** can't open path: %s\n",M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

	dt = UOS_MsecTimerGet() - t1; HRES_GET_TIME2;

	printf("Further OPEN's      : %5d calls  %6d total msec  ", (int)pathcnt, (int)dt);

	if (dt < tickres)
		printf("   ?.??? msec/call\n");
	else
		printf("%8.3f msec/call\n", (float)dt/pathcnt);
	HRES_PRINT_OPENCLOSE_FURTHER;

	/*--------------------+
    |  GETSTAT            |
    +--------------------*/
    TASKLOCK;
	t1 = UOS_MsecTimerGet(); HRES_GET_TIME1;

	for (n=0; n<callcnt; n++)
		if (M_getstat(path[pathcnt], MT_NOACTION, &rdval) < 0) {
			printf("*** can't getstat: %s\n",M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

	dt = UOS_MsecTimerGet() - t1; HRES_GET_TIME2;
    TASKUNLOCK;
    
	printf("GETSTAT             : %5d calls  %6d total msec  ", (int)callcnt, (int)dt);

	if (dt < tickres)
		printf("   ?.??? msec/call\n");
	else
		printf("%8.3f msec/call\n", (float)dt/callcnt);
	HRES_PRINT;

	/*--------------------+
    |  SETSTAT            |
    +--------------------*/
    wrval = 0;
    TASKLOCK;
	t1 = UOS_MsecTimerGet(); HRES_GET_TIME1;

	for (n=0; n<callcnt; n++)
		if (M_setstat(path[pathcnt], MT_NOACTION, wrval) < 0) {
			printf("*** can't setstat: %s\n",M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

	dt = UOS_MsecTimerGet() - t1; HRES_GET_TIME2;
    TASKUNLOCK;

	printf("SETSTAT             : %5d calls  %6d total msec  ", (int)callcnt, (int)dt);

	if (dt < tickres)
		printf("   ?.??? msec/call\n");
	else
		printf("%8.3f msec/call\n", (float)dt/callcnt);
	HRES_PRINT;

	/*--------------------+
    |  READ               |
    +--------------------*/
	t1 = UOS_MsecTimerGet(); HRES_GET_TIME1;

	for (n=0; n<callcnt; n++)
		if (M_read(path[pathcnt], &rdval) < 0) {
			printf("*** can't read: %s\n",M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

	dt = UOS_MsecTimerGet() - t1; HRES_GET_TIME2;

	printf("READ                : %5d calls  %6d total msec  ", (int)callcnt, (int)dt);

	if (dt < tickres)
		printf("   ?.??? msec/call\n");
	else
		printf("%8.3f msec/call\n", (float)dt/callcnt);
	HRES_PRINT;

	/*--------------------+
    |  WRITE              |
    +--------------------*/
    TASKLOCK;
	t1 = UOS_MsecTimerGet(); HRES_GET_TIME1;

	for (n=0; n<callcnt; n++)
		if (M_write(path[pathcnt], wrval) < 0) {
			printf("*** can't write: %s\n",M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

	dt = UOS_MsecTimerGet() - t1; HRES_GET_TIME2;
    TASKUNLOCK;

	printf("WRITE               : %5d calls  %6d total msec  ", (int)callcnt, (int)dt);

	if (dt < tickres)
		printf("   ?.??? msec/call\n");
	else
		printf("%8.3f msec/call\n", (float)dt/callcnt);
	HRES_PRINT;

	/*--------------------+
    |  GETBLOCK           |
    +--------------------*/
    TASKLOCK;
	t1 = UOS_MsecTimerGet(); HRES_GET_TIME1;

	for (n=0; n<callcnt; n++)
		if (M_getblock(path[pathcnt], &blkbuf, 1) < 0) {
			printf("*** can't getblock: %s\n",M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

	dt = UOS_MsecTimerGet() - t1; HRES_GET_TIME2;
    TASKUNLOCK;

	printf("GETBLOCK            : %5d calls  %6d total msec  ", (int)callcnt, (int)dt);

	if (dt < tickres)
		printf("   ?.??? msec/call\n");
	else
		printf("%8.3f msec/call\n", (float)dt/callcnt);
	HRES_PRINT;

	/*--------------------+
    |  SETBLOCK           |
    +--------------------*/
    TASKLOCK;
	t1 = UOS_MsecTimerGet(); HRES_GET_TIME1;

	for (n=0; n<callcnt; n++)
		if (M_setblock(path[pathcnt], &blkbuf, 1) < 0) {
			printf("*** can't setblock: %s\n",M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

	dt = UOS_MsecTimerGet() - t1; HRES_GET_TIME2;
    TASKUNLOCK;

	printf("SETBLOCK            : %5d calls  %6d total msec  ", (int)callcnt, (int)dt);

	if (dt < tickres)
		printf("   ?.??? msec/call\n");
	else
		printf("%8.3f msec/call\n", (float)dt/callcnt);
	HRES_PRINT;

	/*--------------------+
    |  further CLOSE's    |
    +--------------------*/
    TASKLOCK;
	t1 = UOS_MsecTimerGet(); HRES_GET_TIME1;

	for (n=pathcnt; n>0; n--)
	{
		if (M_close(path[n]) < 0)
		{
			printf("*** can't close path: %s\n",M_errstring(UOS_ErrnoGet()));
			goto abort;
		}
	}

	dt = UOS_MsecTimerGet() - t1; HRES_GET_TIME2;
    TASKUNLOCK;

	printf("Further CLOSES's    : %5d calls  %6d total msec  ", (int)pathcnt, (int)dt);

	if (dt < tickres)
		printf("   ?.??? msec/call\n");
	else
		printf("%8.3f msec/call\n", (float)dt/pathcnt);
	HRES_PRINT_OPENCLOSE_FURTHER;

	/*--------------------+
    |  terminating CLOSE  |
    +--------------------*/
    TASKLOCK;
	t1 = UOS_MsecTimerGet(); HRES_GET_TIME1;

	if (M_close(path[0]) < 0) {
		printf("*** can't close path: %s\n",M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	dt = UOS_MsecTimerGet() - t1; HRES_GET_TIME2;
    TASKUNLOCK;

	printf("Terminating CLOSE   : %5d calls  %6d total msec  ", 1, (int)dt);

	if (dt < tickres)
		printf("   ?.??? msec/call\n");
	else
		printf("%8.3f msec/call\n", (float)dt);
	HRES_PRINT_OPENCLOSE;

	printf("(Timer Resolution: %d msec)\n", (int)tickres);
	return(0);

	/*--------------------+
    |  cleanup            |
    +--------------------*/
abort:
	printf("*** ERROR occured\n");
	return(1);
}





