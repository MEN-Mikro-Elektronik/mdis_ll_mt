/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: mt_drv.c
 *      Project: MT module driver (MDIS V4.x)
 *
 *       Author: see
 *        $Date: 2010/12/01 12:20:17 $
 *    $Revision: 1.20 $
 *
 *  Description: Low level driver for MDIS test purposes
 *
 *     This driver allows to test MDIS functionality.
 *
 *     It makes no hardware access if NO_HWACCESS is specified
 *     or as long triggering is disabled.
 *     Signals, callbacks and shared memory access are software
 *     triggered using alarms.
 *
 *     Following features are configurable via compiler switch:
 *     - simulate process lock modes 0..3 (LOCKMODE):
 *       0 = unknown          return ERR_LL_ILL_PARAM
 *       1 = no locking       return 1
 *       2 = call locking     return 2
 *       3 = channel locking  return 3
 *
 *     Following features are configurable via descriptor entry:
 *     - simulate init error (wrong ID)
 *     - make trigger access at alarms+callbacks
 *
 *     Following features are configurable via status calls:
 *     - do nothing (empty call)
 *     - simulate read/write errors
 *     - let process sleep within a read/write/getstat/setstat call
 *     - produce cyclic/single signals   (2 differnt signals)
 *     - produce cyclic/single callbacks (2 differnt callbacks)
 *     - produce cyclic/single shared memory access (2 differnt areas)
 *
 *     Following i/o can be done:
 *     - read: read a previously written value from i/o buffer
 *     - write: write value into i/o buffer
 *     - block read: read a previously written block from block buffer
 *     - block write: write block into block buffer
 *
 *     NOTE: Maximum alarm rate depends on the systems ticker
 *           resolution (as returned via M_MK_TICKRATE getstat)
 *
 *     If MT_PCI switch is set, sets up 3 addresspaces (one with PCICFG)
 *	   to test MK's PCI functionality. Only the open call makes sense
 *
 *     Shared memory functionality will only be used if the switch
 *     NO_SHARED_MEM is not set.
 *
 *     Callback functionality will only be used if the switch
 *     NO_CALLBACK is not set.
 *
 *     Required: ---
 *     Switches: _ONE_NAMESPACE_PER_DRIVER_
 *               LOCKMODE		: lock mode processing (LL_INFO_LOCKMODE)
 *				 MT_PCI			: see above. LOCKMODE must be set to 0
 *               NO_SHARED_MEM	: shared memory not supported
 *               NO_CALLBACK	: callback not supported
 *               NO_HWACCESS	: makes never a HW access and requests no address space
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: mt_drv.c,v $
 * Revision 1.20  2010/12/01 12:20:17  dpfeuffer
 * R: OSS_SpinLockXxx introduction
 * M: getstats for spin lock test implemented
 *
 * Revision 1.19  2009/10/07 10:32:10  CKauntz
 * R:1. Porting to MDIS5 (according porting guide rev. 0.7)
 *   2. Semaphore tests included to start in Kernel Mode
 * M:1.a) added support for 64bit
 *     b) adapted DBG prints for 64bit pointers
 *   2. Added semaphore set and getstats
 *
 * Revision 1.18  2008/08/29 11:46:01  dpfeuffer
 * R: MT_SetStat/MT_GetStat not MDIS5 compatibel
 * M: MT_SetStat/MT_GetStat modified for MDIS5
 *
 * Revision 1.17  2006/09/13 13:29:06  DPfeuffer
 * MT_SetStat(MT_BLK_ALARM_SET): errorhandling fixed
 *
 * Revision 1.16  2004/06/22 13:53:52  dpfeuffer
 * NO_HWACCESS switch for mt_nohw variant added
 *
 * Revision 1.15  2000/06/14 14:38:02  Schmidt
 * MT_BlockWrite/Read: size-check added to prevent writing behind the buffer
 * int32 MT_Info: bugfix for LOCKMODE switch
 * NO_SHARED_MEM and NO_CALLBACK switches added to make driver usable for
 * MDIS4 without this features
 *
 * Revision 1.14  1999/08/11 10:15:41  Schmidt
 * dbg.h must be included before oss.h#
 * some casts added
 *
 * Revision 1.13  1999/08/10 15:33:35  kp
 * Added MT_PCI variant
 *
 * Revision 1.12  1999/07/20 12:18:42  Franke
 * cosmetics
 *
 * Revision 1.11  1999/07/05 08:09:21  kp
 * check if devSem exists. Only do SemSignal and SemWait if present
 * adapted to new Callback API
 * added setstats MT_BUSYLOOP, MT_MDELAY
 *
 * Revision 1.10  1999/04/16 16:26:24  Franke
 * cosmetics
 * added return at MT_Irq()
 * changes MT_Exit()...MT_Info() now static
 *
 * Revision 1.9  1999/03/22 15:30:28  see
 * shared memory handling changed according to interface modifications
 *
 * Revision 1.8  1998/09/18 14:06:03  see
 * callbacks: changes according to OSS update
 * alarms: don't install+send signal if signal=0
 * shared memory functions/setstats added
 * all alarm functions: got (void*) as arg
 *
 * Revision 1.7  1998/09/10 10:20:24  see
 * one-shot sleep mode added (MT_ONE_SLEEP)
 * bug fixed: MT_GetStat used setstatSleep
 * simulate init error (wrong ID)
 * make trigger access at alarms+callbacks
 *
 * Revision 1.6  1998/08/24 14:50:18  see
 * io_buf and blk_buf added for (block) read/write buffering
 * callback functions/setstats added
 *
 * Revision 1.4  1998/07/31 17:24:26  see
 * idFuncTbl is now located in LL_HANDLE
 * idFuncTbl is now initialized in Init
 * IdFuncTbl() removed
 * ID_SIZE was wrong
 * use OSS_DBG_DEFAULT
 *
 * Revision 1.3  1998/07/02 16:52:46  see
 * alarm functions/setstats added
 *
 * Revision 1.2  1998/06/09 15:28:55  Franke
 * change DBG_ALL with OSS_DBG_LEVEL for OS specific default setting
 *
 * Revision 1.1  1998/05/28 09:58:40  see
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

static const char RCSid[]="MT - MT Low Level Driver: $Id: mt_drv.c,v 1.20 2010/12/01 12:20:17 dpfeuffer Exp $";

#define _NO_LL_HANDLE		/* ll_defs.h: don't define LL_HANDLE struct */

#include <MEN/men_typs.h>   /* system dependend definitions   */
#include <MEN/maccess.h>    /* hw access macros and types     */
#include <MEN/dbg.h>
#include <MEN/oss.h>        /* oss functions                  */
#include <MEN/desc.h>       /* descriptor functions           */
#include <MEN/mdis_api.h>   /* MDIS global defs               */
#include <MEN/mdis_com.h>   /* MDIS common defs               */
#include <MEN/mdis_err.h>   /* MDIS error codes               */
#include <MEN/ll_defs.h>    /* low level driver definitions   */


/*-----------------------------------------+
|  DEFINES                                 |
+-----------------------------------------*/
/* limits */
#define MAX_BLOCK_SIZE		256			/* max block data size */
#define MAX_ALARM			2			/* max nr of alarms */
#define MAX_CALLBACK		2			/* max nr of callback functions */
#define MAX_QUEUE			10			/* max nr of queued callbacks */
#define MAX_SHMEM			2			/* max nr of shared mem areas */
#define MAX_SHMEM_SIZE		1024		/* max shared mem size */
#define MAX_SEMAPHORE_CNT	2			/* max count sempahores value */
#define MAX_SLALARM			2			/* max nr of alarms for spin lock test */

/* general */
#define CH_NUMBER			4			/* nr of device channels */
#define ID_SIZE				128			/* id prom size [bytes] */

#ifdef NO_HWACCESS
# define ADDRSPACE_COUNT		0		/* nr of required address spaces */
# define USE_IRQ				FALSE	/* interrupt not required  */
#else
# ifndef MT_PCI
#  define ADDRSPACE_COUNT		1		/* nr of required address spaces */
#  define USE_IRQ				FALSE	/* interrupt not required  */
# else
#  define ADDRSPACE_COUNT		3
#  define USE_IRQ				TRUE	/* interrupt required  */
# endif
#endif /* NO_HWACCESS */

/* debug definitions */
#define DBH				llHdl->dbgHdl
#define DBG_MYLEVEL		llHdl->dbgLevel

/*-----------------------------------------+
|  TYPEDEFS                                |
+-----------------------------------------*/
/* ll handle */
typedef struct {
	/* general */
    int32           memAlloc;		/* size allocated for the handle */
    OSS_HANDLE      *osHdl;         /* oss handle */
    OSS_SEM_HANDLE  *devSemHdl;     /* device semaphore handle */
    OSS_IRQ_HANDLE  *irqHdl;        /* irq handle */
    DESC_HANDLE     *descHdl;       /* desc handle */
#ifndef NO_HWACCESS
    MACCESS         ma;             /* hw access handle */
#endif
	MDIS_IDENT_FUNCT_TBL idFuncTbl;	/* id function table */
	/* debug */
    u_int32         dbgLevel;		/* debug level */
	DBG_HANDLE      *dbgHdl;        /* debug handle */
	/* id */
    u_int32         idError;		/* produce id error */
	/* irq */
    u_int32         irqCount;       /* interrupt counter */
	/* error modes */
    u_int32         readError;      /* produce read errors */
    u_int32         writeError;     /* produce write errors */
	/* sleep modes */
    u_int32         readSleep;      /* read sleep time [msec] */
    u_int32         writeSleep;     /* read sleep time [msec] */
    u_int32         blockReadSleep; /* read sleep time [msec] */
    u_int32         blockWriteSleep;/* read sleep time [msec] */
    u_int32         setstatSleep;   /* read sleep time [msec] */
    u_int32         getstatSleep;   /* read sleep time [msec] */
    u_int32         oneSleep;       /* one-shot sleep mode */
	/* triggering */
    u_int32         trigEnable;					/* trig enable */
    u_int32         trigCall[MAX_CALLBACK];		/* callback trig addr */
    u_int32         trigAlarm[MAX_ALARM];		/* alarm trig addr */
	/* alarms */
    OSS_ALARM_HANDLE *alarmHdl[MAX_ALARM];  /* alarm handles */
    OSS_SIG_HANDLE   *sigHdl[MAX_ALARM];    /* signal handles */
#ifndef NO_CALLBACK
	/* callbacks */
    OSS_CALLBACK_HANDLE *cbHdl;  			   /* callback handle */
    OSS_ALARM_HANDLE *cbAlmHdl[MAX_CALLBACK];  /* callback alarm handles */
	u_int32         cbAlmInst[MAX_CALLBACK];   /* callback alarm installed */
	u_int32         cbCount[MAX_CALLBACK];	   /* callback counters */
#endif
	/* shared mem areas */
#ifndef NO_SHARED_MEM
    OSS_SHMEM_HANDLE *smHdl;  			     /* shared mem handle */
#endif
    OSS_ALARM_HANDLE *smAlmHdl[MAX_SHMEM];   /* shared mem alarm handles */
	u_int32         smAlmInst[MAX_SHMEM];    /* shared mem alarm installed */
	u_int32         smCount[MAX_SHMEM];      /* shared mem counters */
	void            *smAddr[MAX_SHMEM];	     /* shared mem addresses */
	u_int32         smSize[MAX_SHMEM];	     /* shared mem sizes */
	/* semaphores */
	OSS_SEM_HANDLE	*semBinHdl;
	OSS_SEM_HANDLE	*semCntHdl;
	/* buffers */
	int32 io_buf;					/* normal i/o buffer */
	int8 blk_buf[MAX_BLOCK_SIZE];	/* block i/o buffer */
	/* spin lock test */
	OSS_SPINL_HANDLE	*slHdl;			/* spin lock handle */
    OSS_ALARM_HANDLE	*slAlmHdl[MAX_SLALARM];	/* alarm handle */
	u_int32				slNoCount;		/* no spin lock counter */
	u_int32				slYesCount;		/* yes spin lock counter */
} LL_HANDLE;

/* include files which need LL_HANDLE */
#include <MEN/ll_entry.h>   /* low level driver jumptable  */
#include <MEN/mt_drv.h>   	/* MT driver header file */

/*-----------------------------------------+
|  PROTOTYPES                              |
+-----------------------------------------*/
static int32 MT_Init(DESC_SPEC *descSpec, OSS_HANDLE *osHdl,
					   MACCESS *ma, OSS_SEM_HANDLE *devSemHdl,
					   OSS_IRQ_HANDLE *irqHdl, LL_HANDLE **llHdlP);
static int32 MT_Exit(LL_HANDLE **llHdlP );
static int32 MT_Read(LL_HANDLE *llHdl, int32 ch, int32 *value);
static int32 MT_Write(LL_HANDLE *llHdl, int32 ch, int32 value);
static int32 MT_SetStat(LL_HANDLE *llHdl,int32 code, int32 ch, INT32_OR_64	value32_or_64);
static int32 MT_GetStat(LL_HANDLE *llHdl, int32 code, int32 ch,INT32_OR_64	*value32_or_64P);
static int32 MT_BlockRead(LL_HANDLE *llHdl, int32 ch, void *buf, int32 size,
							int32 *nbrRdBytesP);
static int32 MT_BlockWrite(LL_HANDLE *llHdl, int32 ch, void *buf, int32 size,
							 int32 *nbrWrBytesP);
static int32 MT_Irq(LL_HANDLE *llHdl );
static int32 MT_Info(int32     infoType, ... );


static char* Ident( void );
static int32 Cleanup(LL_HANDLE *llHdl, int32 retCode);
static void DoSleep(LL_HANDLE *llHdl, u_int32 *msec);
static void AlarmHandler0(void *arg);
static void AlarmHandler1(void *arg);
#ifndef NO_CALLBACK
static void CallbackHandler0(void *arg);
static void CallbackHandler1(void *arg);
#endif
static void SharedMemHandler0(void *arg);
static void SharedMemHandler1(void *arg);
static void SpinLockAlmHdlr0(void *arg);
static void SpinLockAlmHdlr1(void *arg);

/**************************** MT_GetEntryX *********************************
 *
 *  Description:  Initialize drivers jump table
 *
 *                MT_GetEntry0..3  which means
 *						       0 = unknown
 *						       1 = no locking
 *						       2 = call locking
 *						       3 = channel locking
 *
 *---------------------------------------------------------------------------
 *  Input......:  ---
 *  Output.....:  drvP  pointer to the initialized jump table structure
 *  Globals....:  ---
 ****************************************************************************/
void
#ifdef _ONE_NAMESPACE_PER_DRIVER_
    LL_GetEntry
#else
#  if ( LOCKMODE == 0 )
#   ifdef MT_PCI
    MT_GetEntry_Pci
#   else
    MT_GetEntry_LockMode0
#   endif
#  endif
#  if ( LOCKMODE == 1 )
    MT_GetEntry_LockMode1
#  endif
#  if ( LOCKMODE == 2 )
    MT_GetEntry_LockMode2
#  endif
#  if ( LOCKMODE == 3 )
    MT_GetEntry_LockMode3
#  endif
#endif
    ( LL_ENTRY* drvP )
{
    drvP->init        = MT_Init;
    drvP->exit        = MT_Exit;
    drvP->read        = MT_Read;
    drvP->write       = MT_Write;
    drvP->blockRead   = MT_BlockRead;
    drvP->blockWrite  = MT_BlockWrite;
    drvP->setStat     = MT_SetStat;
    drvP->getStat     = MT_GetStat;
    drvP->irq         = MT_Irq;
    drvP->info        = MT_Info;
}

/******************************** MT_Init ***********************************
 *
 *  Description:  Allocate and return ll handle, initialize hardware
 *
 *                The following descriptor keys are used:
 *
 *                Deskriptor key        Default          Range
 *                --------------------  ---------------  -------------
 *                DEBUG_LEVEL           OSS_DBG_DEFAULT  see dbg.h
 *                DEBUG_LEVEL_DESC      DBG_OFF          see dbg.h
 *                ID_CHECK              0                0..1
 *
 *---------------------------------------------------------------------------
 *  Input......:  descSpec   pointer to descriptor data
 *                osHdl      oss handle
 *                maHdl      hw access handle
 *                devSemHdl  device semaphore handle
 *                irqHdl     irq handle
 *  Output.....:  llHdlP     ptr to low level driver handle
 *                return     success (0) or error code
 *  Globals....:  ---
 ****************************************************************************/
int32 MT_Init(
    DESC_SPEC       *descP,
    OSS_HANDLE      *osHdl,
    MACCESS         *ma,
    OSS_SEM_HANDLE  *devSemHdl,
    OSS_IRQ_HANDLE  *irqHdl,
    LL_HANDLE       **llHdlP
)
{
    LL_HANDLE *llHdl = NULL;
    u_int32 gotsize;
    int32 error;
    u_int32 value, n;

    /*------------------------------+
    |  prepare the handle           |
    +------------------------------*/
	/* alloc */
    if ((*llHdlP = llHdl = (LL_HANDLE*)
		 OSS_MemGet(osHdl, sizeof(LL_HANDLE), &gotsize)) == NULL)
       return(ERR_OSS_MEM_ALLOC);

	/* clear */
    OSS_MemFill(osHdl, gotsize, (char*)llHdl, 0x00);

	/* init */
    llHdl->memAlloc   = gotsize;
    llHdl->osHdl      = osHdl;
    llHdl->irqHdl     = irqHdl;
#ifndef NO_HWACCESS
    llHdl->ma		  = *ma;
#endif
    llHdl->devSemHdl  = devSemHdl;

    /*------------------------------+
    |  init id function table       |
    +------------------------------*/
	/* drivers ident function */
	llHdl->idFuncTbl.idCall[0].identCall = Ident;
	/* libraries ident functions */
	llHdl->idFuncTbl.idCall[1].identCall = DESC_Ident;
	llHdl->idFuncTbl.idCall[2].identCall = OSS_Ident;
	/* terminator */
	llHdl->idFuncTbl.idCall[3].identCall = NULL;

    /*------------------------------+
    |  prepare debugging            |
    +------------------------------*/
	DBG_MYLEVEL = OSS_DBG_DEFAULT;	/* set OS specific debug level */
	DBGINIT((NULL,&DBH));

    /*------------------------------+
    |  scan descriptor              |
    +------------------------------*/
	/* prepare access */
    if ((error = DESC_Init(descP, llHdl->osHdl, &llHdl->descHdl)))
		return( Cleanup(llHdl,error) );

    /* DEBUG_LEVEL_DESC */
    if ((error = DESC_GetUInt32(llHdl->descHdl, DBG_OFF, &value,
								"DEBUG_LEVEL_DESC")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		return( Cleanup(llHdl,error) );

	DESC_DbgLevelSet(llHdl->descHdl, value);	/* set level */

    /* DEBUG_LEVEL */
    if ((error = DESC_GetUInt32(llHdl->descHdl, DBG_OFF, &llHdl->dbgLevel,
								"DEBUG_LEVEL")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		return( Cleanup(llHdl,error) );

    DBGWRT_1((DBH, "LL - MT_Init\n"));

    /* ID_ERROR */
    if ((error = DESC_GetUInt32(llHdl->descHdl, FALSE, &llHdl->idError,
								"ID_ERROR")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		return( Cleanup(llHdl,error) );

    /* ENABLE_TRIG */
    if ((error = DESC_GetUInt32(llHdl->descHdl, FALSE, &llHdl->trigEnable,
								"ENABLE_TRIG")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		return( Cleanup(llHdl,error) );

#ifndef NO_CALLBACK
    /* CALLBACK_%d_TRIG */
	for (n=0; n<MAX_CALLBACK; n++) {
		if ((error = DESC_GetUInt32(llHdl->descHdl, FALSE, &llHdl->trigCall[n],
									"CALLBACK_%d_TRIG", n)) &&
			error != ERR_DESC_KEY_NOTFOUND)
			return( Cleanup(llHdl,error) );
	}
#endif

    /* ALARM_%d_TRIG */
	for (n=0; n<MAX_ALARM; n++) {
		if ((error = DESC_GetUInt32(llHdl->descHdl, FALSE, &llHdl->trigAlarm[n],
									"ALARM_%d_TRIG", n)) &&
			error != ERR_DESC_KEY_NOTFOUND)
			return( Cleanup(llHdl,error) );
	}

    /*------------------------------+
    |  produce init error           |
    +------------------------------*/
	if (llHdl->idError)
		return(ERR_LL_ILL_ID);

    /*------------------------------+
    |  init alarms                  |
    +------------------------------*/
    if ((error = OSS_AlarmCreate(llHdl->osHdl, AlarmHandler0, llHdl,
								 &llHdl->alarmHdl[0]))	||
		(error = OSS_AlarmCreate(llHdl->osHdl, AlarmHandler1, llHdl,
								 &llHdl->alarmHdl[1])))
		return( Cleanup(llHdl,error));

#ifndef NO_CALLBACK
    /*------------------------------+
    |  init callbacks               |
    +------------------------------*/
	/* create alarms to trigger callbacks */
    if ((error = OSS_AlarmCreate(llHdl->osHdl, CallbackHandler0, llHdl,
								 &llHdl->cbAlmHdl[0]))	||
		(error = OSS_AlarmCreate(llHdl->osHdl, CallbackHandler1, llHdl,
								 &llHdl->cbAlmHdl[1])))
		return( Cleanup(llHdl,error));

	/* init callbacks */
	if ((error = OSS_CallbackCreate(llHdl->osHdl, MAX_QUEUE,
									&llHdl->cbHdl)))
		return( Cleanup(llHdl,error));
#endif

    /*------------------------------+
    |  init shared memory           |
    +------------------------------*/
	/* create alarms to trigger shared mem access */
    if ((error = OSS_AlarmCreate(llHdl->osHdl, SharedMemHandler0, llHdl,
								 &llHdl->smAlmHdl[0]))	||
		(error = OSS_AlarmCreate(llHdl->osHdl, SharedMemHandler1, llHdl,
								 &llHdl->smAlmHdl[1])))
		return( Cleanup(llHdl,error));

	/* init xhared memory */
#ifndef NO_SHARED_MEM
	if ((error = OSS_SharedMemCreate(llHdl->osHdl, &llHdl->smHdl)))
		return( Cleanup(llHdl,error));
#endif
	/*------------------------------+
    |  init semaphores              |
    +------------------------------*/
	/* create binary semaphore */
	if ( (error = OSS_SemCreate(llHdl->osHdl, OSS_SEM_BIN, 1,
					&llHdl->semBinHdl)) ){
		DBGWRT_ERR((DBH," *** MT_Init: Error creating binary semaphore\n"));
		return(Cleanup(llHdl, error));
	}
	/* create binary semaphore */
	if ( (error = OSS_SemCreate(llHdl->osHdl, OSS_SEM_COUNT, MAX_SEMAPHORE_CNT,
					&llHdl->semCntHdl)) ){
		DBGWRT_ERR((DBH," *** MT_Init: Error creating counter semaphore\n"));
		return(Cleanup(llHdl, error));
	}

    /*------------------------------+
    |  init spin lock test          |
    +------------------------------*/
    if ((error = OSS_AlarmCreate(llHdl->osHdl, SpinLockAlmHdlr0, llHdl,
								 &llHdl->slAlmHdl[0])))
		return( Cleanup(llHdl,error));
	if ((error = OSS_AlarmCreate(llHdl->osHdl, SpinLockAlmHdlr1, llHdl,
								 &llHdl->slAlmHdl[1])))
		return( Cleanup(llHdl,error));
	if ((error = OSS_SpinLockCreate(llHdl->osHdl, &llHdl->slHdl)))
		return( Cleanup(llHdl,error));	

    /*------------------------------+
    |  init hardware                |
    +------------------------------*/
	/* ... */

#ifdef MT_PCI
	/*---------------------------------+
	|  Test access to PCI config regs  |
	+---------------------------------*/
	{
		u_int32 pciBusNbr, pciDevNbr, pciFuncNbr;
		int32 vendorId;

		pciBusNbr = (u_int32)((((U_INT32_OR_64)ma[2]) >> 16) & 0xff);
		pciDevNbr = (u_int32)((((U_INT32_OR_64)ma[2]) >> 11) & 0x1f);
		pciFuncNbr = (u_int32)((((U_INT32_OR_64)ma[2]) >> 8) & 0x7);

		DBGWRT_2((DBH, " PCI bus=%d dev=%d func=%d\n",
				  pciBusNbr, pciDevNbr, pciFuncNbr));

		/*--- Test access to config space ---*/
		error += OSS_PciGetConfig( osHdl, pciBusNbr, pciDevNbr, pciFuncNbr,
								   OSS_PCI_VENDOR_ID, &vendorId );
		DBGWRT_2((DBH, " PCI vendor ID=0x%04x\n", vendorId ));

	}
#endif
	return(ERR_SUCCESS);
}

/****************************** MT_Exit *************************************
 *
 *  Description:  De-initialize hardware and cleanup memory
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdlP  	ptr to low level driver handle
 *  Output.....:  return    success (0) or error code
 *  Globals....:  ---
 ****************************************************************************/
int32 MT_Exit(
   LL_HANDLE    **llHdlP
)
{
    LL_HANDLE *llHdl = *llHdlP;
	int32 error = 0;

    DBGWRT_1((DBH, "LL - MT_Exit\n"));

    /*------------------------------+
    |  de-init hardware             |
    +------------------------------*/
	/* ... */

    /*------------------------------+
    |  cleanup memory               |
    +------------------------------*/
	error = Cleanup(llHdl,error);

	return(error);
}

/****************************** MT_Read *************************************
 *
 *  Description:  Reads value from device
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl    ll handle
 *                ch       current channel
 *  Output.....:  valueP   read value
 *                return   success (0) or error code
 *  Globals....:  ---
 ****************************************************************************/
int32 MT_Read(
    LL_HANDLE *llHdl,
    int32 ch,
    int32 *value
)
{
    DBGWRT_1((DBH, "LL - MT_Read: ch=%d\n",ch));

	/* handle sleep mode */
	if (llHdl->readSleep)
		DoSleep(llHdl, &llHdl->readSleep);

	/* read buffered value */
	*value = llHdl->io_buf;

	return(llHdl->readError ? ERR_LL_READ : ERR_SUCCESS);
}

/****************************** MT_Write ************************************
 *
 *  Description:  Write value to device
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl    ll handle
 *                ch       current channel
 *                value    value to write
 *  Output.....:  return   success (0) or error code
 *  Globals....:  ---
 ****************************************************************************/
int32 MT_Write(
    LL_HANDLE *llHdl,
    int32 ch,
    int32 value
)
{
    DBGWRT_1((DBH, "LL - MT_Write: ch=%d\n",ch));

	/* write value into buffer */
	llHdl->io_buf = value;

	/* handle sleep mode */
	if (llHdl->writeSleep)
		DoSleep(llHdl, &llHdl->writeSleep);

	return(llHdl->writeError ? ERR_LL_WRITE : ERR_SUCCESS);
}

/****************************** MT_SetStat **********************************
 *
 *  Description:  Set driver status
 *
 *                Following status codes are supported:
 *
 *                Code                 Description                Values
 *                -------------------  -------------------------  ----------
 *                M_LL_DEBUG_LEVEL     driver debug level         see oss.h
 *                M_MK_IRQ_ENABLE      interrupt enable           0..1
 *                M_LL_IRQ_COUNT       interrupt counter          0..max
 *                M_LL_BLK_CALLBACK    callback interface         -
 *                M_LL_BLK_SHMEM_x     shared memory interface    -
 *                -------------------  -------------------------  ----------
 *                MT_NOACTION          do nothing                 0..max
 *				  MT_BUSYLOOP		   perform busy loop		  0..max
 *                MT_MDELAY			   test micro delay           0..max
 *                MT_xxx_ERROR         produce call errors        0..1
 *                MT_xxx_SLEEP         let calls sleep [msec]     0..max
 *                MT_ONE_SLEEP         one-shot call sleep mode   0..1
 *                MT_BLK_ALARM_SET     activate    alarm i        see below
 *                MT_ALARM_CLR         de-activate alarm i        0..1
 *                MT_BLK_CALLBACK_SET  activate    callback i     see below
 *                MT_CALLBACK_CLR      de-activate callback i     0..1
 *                MT_BLK_SHMEM_SET     activate    shared mem i   see below
 *                MT_SHMEM_CLR         de-activate shared mem i   0..1
 *                MT_SEM_BIN_TAK       take binary semaphore      0..max
 *                MT_SEM_BIN_REL       release binary semaphore   -
 *                MT_SEM_CNT_TAK       take counter semaphore     0..max
 *                MT_SEM_CNT_REL       release counter semaphore  -
 *                -------------------  -------------------------  ----------
 *
 *                MT_NOACTION does really nothing and returns with success.
 *
 *				  MT_BUSYLOOP performs <value> busy loop to test process
 *				  timeslicing/locking
 *
 *				  MT_MDELAY calls OSS_MikroDelay with the specified value (us)
 *
 *                MT_xxx_ERROR enables call errors. For all subsequent calls
 *                of the corresponding function, an error is returned:
 *
 *                   MT_READ_ERROR	     produce read errors
 *                   MT_WRITE_ERROR	     produce write errors
 *
 *                MT_xxx_SLEEP enables call sleeping. For all subsequent calls
 *                of the corresponding function, function waits in sleep state
 *                for specified millisecons before it returns. To disable call
 *                sleeping define sleep time=0.
 *
 *                   MT_READ_SLEEP       read sleep [msec]
 *                   MT_WRITE_SLEEP      write sleep [msec]
 *                   MT_BLOCKREAD_SLEEP  blockread sleep [msec]
 *                   MT_BLOCKWRITE_SLEEP blockwrite sleep [msec]
 *                   MT_GETSTAT_SLEEP    getstat sleep [msec] *)
 *                   MT_SETSTAT_SLEEP    setstat sleep [msec] *)
 *
 *                   *) sleeps only at MT_NOACTION code
 *
 *                If MT_ONE_SLEEP is enabled (1), sleep mode is automatically
 *                cleared after once waited (one-shot mode).
 *
 *                MT_BLK_ALARM_SET enables generation of alarm/signals.
 *                Cyclic or single alarms with specified period are produced
 *                and the specified signal is send to the application:
 *
 *                   typedef struct {
 *                	    u_int32 nr;		 alarm index (0 or 1)
 *                	    u_int32 msec;	 alarm time [msec]
 *                	    u_int32 cyclic;	 cyclic alarm (0=single, 1=cyclic)
 *                	    u_int32 signal;	 signal to send (0=none)
 *                   } MT_BLK_ALARM;
 *
 *                MT_ALARM_CLR disables generation of specified alarm,
 *                where alarm index is passed as value.
 *
 *                MT_BLK_CALLBACK is passed unmodified to OSS_CallbackSetStat
 *
 *                MT_BLK_SHMEM_SET enables shared memory access.
 *                Shared memory access can be triggered cyclic or single
 *                with specified period:
 *
 *                   typedef struct {
 *                      u_int32 nr;	     shared memory area index (0..1)
 *                   	u_int32 msec;	 access period [msec]
 *                   	u_int32 cyclic;	 cyclic mode (0=single, 1=cyclic)
 *                   } MT_BLK_SHMEM;
 *
 *                MT_SHMEM_CLR disables access of specified shared memory,
 *                where area index is passed as value.
 *
 *                MT_SEM_BIN_TAK takes the binary semaphore with timeout
 *                MT_SEM_BIN_REL releases the token binary semaphore
 *                MT_SEM_CNT_TAK takes the counter semaphore with timeout
 *                MT_SEM_CNT_REL relaeses the token counter semaphore
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl      ll handle
 *                code       status code
 *                ch         current channel
 *                value32_or64  data or ptr to block
 *                          data struct (M_SG_BLOCK)  (*)
 *                (*) = for block status codes
 *  Output.....:  return     success (0) or error code
 *  Globals....:  ---
 ****************************************************************************/
int32 MT_SetStat(
    LL_HANDLE	*llHdl,
    int32		code,
    int32		ch,
    INT32_OR_64	value32_or_64
)
{
	int32		error = ERR_SUCCESS;
	int32		value	= (int32)value32_or_64;	/* 32bit value			       */
	INT32_OR_64	valueP	= value32_or_64;		/* stores 32/64bit pointer     */
	M_SG_BLOCK	*blk = (M_SG_BLOCK*)valueP;		/* stores block struct pointer */

    DBGWRT_1((DBH, "LL - MT_SetStat: ch=%d code=0x%04x value=0x%x\n",
			  ch,code,value));

    switch(code) {

#ifndef NO_CALLBACK
        /*--------------------------+
        |  callback interface       |
        +--------------------------*/
		case M_LL_BLK_CALLBACK:
			error = OSS_CallbackSetStat(llHdl->osHdl,llHdl->cbHdl,
										(OSS_CALLBACK_SETSTAT *)blk->data);
			break;
#endif

#ifndef NO_SHARED_MEM
        /*--------------------------+
        |  shared memory interface  |
        +--------------------------*/
		/* create shared memory area */
		case M_LL_BLK_SHMEM_SET:
		{
			OSS_SHMEM_SET *smSet = (OSS_SHMEM_SET*)blk->data;
			u_int32 smNr, smSize;

			/* check+get index */
			if ((error = OSS_SharedMemCheckNr(llHdl->osHdl, llHdl->smHdl,
											  (OSS_SHMEM_SET*)blk->data,
											  0, MAX_SHMEM-1, &smNr)))
				return(error);

			/* check+get size */
			if ((error = OSS_SharedMemCheckSize(llHdl->osHdl, llHdl->smHdl,
             	                                (OSS_SHMEM_SET*)blk->data,
												1, MAX_SHMEM_SIZE, &smSize)))
				return(error);

			/* create area, get index/address/links */
			if ((error = OSS_SharedMemSet(llHdl->osHdl, llHdl->smHdl,
											(OSS_SHMEM_SET*)blk->data)))
				return(error);

			/* init address/size */
			llHdl->smAddr[smNr]    = smSet->drvAddr;
			llHdl->smSize[smNr]    = smSet->size;
			break;
		}

		/* link to shared memory area */
		case M_LL_BLK_SHMEM_LINK:
		{
			OSS_SHMEM_LINK *smLink = (OSS_SHMEM_LINK*)blk->data;
			u_int32 smNr;

			/* check+get index */
			if ((error = OSS_SharedMemCheckNr(llHdl->osHdl, llHdl->smHdl,
											  (OSS_SHMEM_SET*)blk->data,
											  0, MAX_SHMEM-1, &smNr)))
				return(error);

			/* link area */
			if ((error = OSS_SharedMemLink(llHdl->osHdl, llHdl->smHdl,
											(OSS_SHMEM_SET*)blk->data)))
				return(error);

			/* init address/size */
			llHdl->smAddr[smNr]    = smLink->drvAddr;
			llHdl->smSize[smNr]    = smLink->size;
			break;
		}

		/* remove shared memory area */
		case M_LL_BLK_SHMEM_CLEAR:
		{
			OSS_SHMEM_CLEAR *smClear = (OSS_SHMEM_CLEAR*)blk->data;
			u_int32 smNr;

			/* check+get index */
			if ((error = OSS_SharedMemCheckNr(llHdl->osHdl, llHdl->smHdl,
											  (OSS_SHMEM_SET*)blk->data, 0,
											  MAX_SHMEM-1, &smNr)))
				return(error);

			/* unlink area */
			if ((error = OSS_SharedMemClear(llHdl->osHdl, llHdl->smHdl,
											(OSS_SHMEM_CLEAR*)blk->data)))
				return(error);

			/* no more links ? */
			if (smClear->linkCnt == 0)
				llHdl->smAddr[smNr] = NULL;

			break;
		}
#endif

        /*--------------------------+
        |  debug level              |
        +--------------------------*/
        case M_LL_DEBUG_LEVEL:
            llHdl->dbgLevel = value;
            break;
        /*--------------------------+
        |  enable interrupts        |
        +--------------------------*/
        case M_MK_IRQ_ENABLE:
			error = ERR_LL_UNK_CODE;	/* say: not supported */
            break;
        /*--------------------------+
        |  set irq counter          |
        +--------------------------*/
		case M_MK_IRQ_COUNT:
			error = ERR_LL_UNK_CODE;	/* say: not supported */
            break;
        /*--------------------------+
        |  do nothing               |
        +--------------------------*/
        case MT_NOACTION:
            break;
        /*--------------------------+
        |  perform busy loop        |
        +--------------------------*/
        case MT_BUSYLOOP:
		{
			int32 i;
			volatile u_int32 a=1;
			DBGWRT_2((DBH," busy loop start. count=%d\n", value ));
			for( i=0; i<value; i++ ){
				a = a*10;
			}
			DBGWRT_2((DBH," busy loop end\n" ));
		}
            break;
        /*--------------------------+
        |  test micro delay         |
        +--------------------------*/
        case MT_MDELAY:
			error = OSS_MikroDelay( llHdl->osHdl, value );
            break;
        /*--------------------------+
        |  error modes              |
        +--------------------------*/
        case MT_READ_ERROR:
            llHdl->readError = value;
            break;
        case MT_WRITE_ERROR:
            llHdl->writeError = value;
            break;
        /*--------------------------+
        |  sleep modes              |
        +--------------------------*/
        case MT_READ_SLEEP:
            llHdl->readSleep = value;
            break;
        case MT_WRITE_SLEEP:
            llHdl->writeSleep = value;
            break;
        case MT_BLOCKREAD_SLEEP:
            llHdl->blockReadSleep = value;
            break;
        case MT_BLOCKWRITE_SLEEP:
            llHdl->blockWriteSleep = value;
            break;
        case MT_GETSTAT_SLEEP:
            llHdl->getstatSleep = value;
            break;
        case MT_SETSTAT_SLEEP:
            llHdl->setstatSleep = value;
            break;
        case MT_ONE_SLEEP:
            llHdl->oneSleep = value;
            break;
        /*--------------------------+
        |  alarms                   |
        +--------------------------*/
        case MT_BLK_ALARM_SET:
		{
			MT_BLK_ALARM *alm = (MT_BLK_ALARM*)blk->data;
			u_int32 realMsec;
			int32	sigCreateRet = -1;

			/* check buf size */
			if (blk->size < sizeof(MT_BLK_ALARM))
				return(ERR_LL_USERBUF);

			/* check params */
			if (alm->nr > MAX_ALARM || alm->cyclic > 1)
				return(ERR_LL_ILL_PARAM);

			/* install alarm+signal */
			if (alm->signal){

				/* already installed? */
				if( llHdl->sigHdl[alm->nr] )
					return(ERR_LL_DEV_BUSY);

				if ((sigCreateRet = OSS_SigCreate(llHdl->osHdl,
									   alm->signal,
									   &llHdl->sigHdl[alm->nr])))
					return(sigCreateRet);
			}

            if ((error = OSS_AlarmSet(llHdl->osHdl,
									  llHdl->alarmHdl[alm->nr],
									  alm->msec,
									  alm->cyclic,
									  &realMsec))){

				/* sig was installed, remove it on error */
				if( sigCreateRet == ERR_SUCCESS )
					OSS_SigRemove(llHdl->osHdl, &llHdl->sigHdl[alm->nr]);

				return(error);
			}

            break;
		}
        case MT_ALARM_CLR:
			if (!IN_RANGE(value,0,MAX_ALARM))
				return(ERR_LL_ILL_PARAM);

  			/* remove alarm+signal */
			if ((error = OSS_AlarmClear(llHdl->osHdl, llHdl->alarmHdl[value])))
				return(error);

			if (llHdl->sigHdl[value] ){
				if ((error = OSS_SigRemove(llHdl->osHdl, &llHdl->sigHdl[value])))
					return(error);
				llHdl->sigHdl[value] = 0;
			}
			break;
        /*--------------------------+
        |  semaphores               |
        +--------------------------*/
		case MT_SEM_BIN_TAK:
			if ((error = OSS_SemWait(llHdl->osHdl,llHdl->semBinHdl,value)))
				return(error);
			break;
		case MT_SEM_BIN_REL:
			if ((error = OSS_SemSignal(llHdl->osHdl,llHdl->semBinHdl)))
				return(error);
			break;
		case MT_SEM_CNT_TAK:
			if ((error = OSS_SemWait(llHdl->osHdl,llHdl->semCntHdl,value)))
				return(error);
			break;
		case MT_SEM_CNT_REL:
			if ((error = OSS_SemSignal(llHdl->osHdl,llHdl->semCntHdl)))
				return(error);
			break;
        /*--------------------------+
        |  callback triggering      |
        +--------------------------*/
#ifndef NO_CALLBACK
        case MT_BLK_CALLBACK_SET:
		{
			MT_BLK_CALLBACK *call = (MT_BLK_CALLBACK*)blk->data;
			u_int32 realMsec;

			/* check buf size */
			if (blk->size < sizeof(MT_BLK_CALLBACK))
				return(ERR_LL_USERBUF);

			/* check params */
			if (call->nr >= MAX_CALLBACK || call->cyclic > 1)
				return(ERR_LL_ILL_PARAM);

			/* install alarm */
            if ((error = OSS_AlarmSet(llHdl->osHdl,
									  llHdl->cbAlmHdl[call->nr],
									  call->msec,
									  call->cyclic,
									  &realMsec)))
				return(error);

			llHdl->cbAlmInst[call->nr] = 1;
            break;
		}
        case MT_CALLBACK_CLR:
			if (!IN_RANGE(value,0,MAX_CALLBACK-1))
				return(ERR_LL_ILL_PARAM);

 			/* remove alarm (if installed) */
           if (llHdl->cbAlmInst[value] &&
			   (error = OSS_AlarmClear(llHdl->osHdl, llHdl->cbAlmHdl[value])))
				return(error);

			llHdl->cbAlmInst[value] = 0;
            break;
#endif
		/*--------------------------+
        |  shared memory access     |
        +--------------------------*/
        case MT_BLK_SHMEM_SET:
		{
			MT_BLK_SHMEM *sm = (MT_BLK_SHMEM*)blk->data;
			u_int32 realMsec;

			/* check buf size */
			if (blk->size < sizeof(MT_BLK_SHMEM))
				return(ERR_LL_USERBUF);

			/* check params */
			if (sm->nr >= MAX_SHMEM || sm->cyclic > 1)
				return(ERR_LL_ILL_PARAM);

			/* install alarm */
            if ((error = OSS_AlarmSet(llHdl->osHdl,
									  llHdl->smAlmHdl[sm->nr],
									  sm->msec,
									  sm->cyclic,
									  &realMsec)))
				return(error);

			llHdl->smAlmInst[sm->nr] = 1;
            break;
		}
        case MT_SHMEM_CLR:
			if (!IN_RANGE(value,0,MAX_SHMEM-1))
				return(ERR_LL_ILL_PARAM);

 			/* remove alarm (if installed) */
           if (llHdl->smAlmInst[value] &&
			   (error = OSS_AlarmClear(llHdl->osHdl, llHdl->smAlmHdl[value])))
				return(error);

			llHdl->smAlmInst[value] = 0;
            break;
        /*--------------------------+
        |  (unknown)                |
        +--------------------------*/
        default:
            error = ERR_LL_UNK_CODE;
    }

	/* handle sleep mode (if MT_NOACTION code) */
	if (llHdl->setstatSleep && code==MT_NOACTION)
		DoSleep(llHdl, &llHdl->setstatSleep);

	return(error);
}

/****************************** MT_GetStat **********************************
 *
 *  Description:  Get driver status
 *
 *                Following status codes are supported:
 *
 *                Code                 Description                Values
 *                -------------------  -------------------------  ----------
 *                M_LL_DEBUG_LEVEL     driver debug level         see oss.h
 *                M_LL_CH_NUMBER       number of channels         32
 *                M_LL_CH_DIR          direction of curr chan     M_CH_INOUT
 *                M_LL_CH_LEN          length of curr chan [bits] 1..max
 *                M_LL_CH_TYP          description of curr chan   M_CH_BINARY
 *                M_LL_IRQ_COUNT       interrupt counter          0..max
 *                M_LL_ID_CHECK        eeprom is checked          0..1
 *                M_LL_ID_SIZE         eeprom size [bytes]        128
 *                M_MK_BLK_REV_ID      ident function table ptr   -
 *                M_LL_BLK_CALLBACK_x  callback interface         -
 *                -------------------  -------------------------  ----------
 *                MT_NOACTION          do nothing                 0..max
 *                MT_xxx_ERROR         produce call errors        0..1
 *                MT_xxx_SLEEP         let calls sleep [msec]     0..max
 *                MT_ONE_SLEEP         one-shot call sleep mode   0..1
 *                -------------------  -------------------------  ----------
 *                MT_SPINL_NO          incr&get cntr, start test  0..max
 *                MT_SPINL_YES         increment and get counter  0..max
 *                -------------------  -------------------------  ----------
 *
 *                MT_xxx_ERROR returns if corresponding call errors are
 *                enabled:
 *
 *                   MT_READ_ERROR	     read errors
 *                   MT_WRITE_ERROR	     write errors
 *
 *                MT_xxx_SLEEP returns if corresponding sleep modes are
 *                enabled, i.e. the specified sleep time (0=disabled):
 *
 *                   MT_READ_SLEEP       read sleep [msec]
 *                   MT_WRITE_SLEEP      write sleep [msec]
 *                   MT_BLOCKREAD_SLEEP  blockread sleep [msec]
 *                   MT_BLOCKWRITE_SLEEP blockwrite sleep [msec]
 *                   MT_GETSTAT_SLEEP    getstat sleep [msec]
 *                   MT_SETSTAT_SLEEP    setstat sleep [msec]
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl      ll handle
 *                code       status code
 *                ch         current channel
 *                value32_or64P  ptr to block data struct (M_SG_BLOCK)  (*)
 *                (*) = for block status codes
 *  Output.....:  valueP     data ptr or
 *                           ptr to block data struct (M_SG_BLOCK)  (*)
 *                return     success (0) or error code
 *                (*) = for block status codes
 *  Globals....:  ---
 ****************************************************************************/
int32 MT_GetStat(
    LL_HANDLE	*llHdl,
    int32		code,
    int32		ch,
	INT32_OR_64	*value32_or_64P
)
{
	int32		*valueP   = (int32*)value32_or_64P;	 /* pointer to 32bit value  */
	INT32_OR_64	*value64P = value32_or_64P;			 /* stores 32/64bit pointer */
#ifndef NO_CALLBACK
	M_SG_BLOCK	*blk   = (M_SG_BLOCK*)value32_or_64P; /* stores block struct pointer */
#endif
	int32		error = ERR_SUCCESS;

    DBGWRT_1((DBH, "LL - MT_GetStat: ch=%d code=0x%04x\n",
			  ch,code));

    switch(code)
    {

#ifndef NO_CALLBACK
        /*--------------------------+
        |  callback interface       |
        +--------------------------*/
		case M_LL_BLK_CALLBACK:
			error = OSS_CallbackGetStat(llHdl->osHdl,llHdl->cbHdl,
										(OSS_CALLBACK_GETSTAT *)blk->data);
			break;
#endif

        /*--------------------------+
        |  debug level              |
        +--------------------------*/
        case M_LL_DEBUG_LEVEL:
            *valueP = llHdl->dbgLevel;
            break;
        /*--------------------------+
        |  nr of channels           |
        +--------------------------*/
        case M_LL_CH_NUMBER:
            *valueP = CH_NUMBER;
            break;
        /*--------------------------+
        |  channel direction        |
        +--------------------------*/
        case M_LL_CH_DIR:
            *valueP = M_CH_INOUT;
            break;
        /*--------------------------+
        |  channel length [bits]    |
        +--------------------------*/
        case M_LL_CH_LEN:
            *valueP = 32;
            break;
        /*--------------------------+
        |  channel type info        |
        +--------------------------*/
        case M_LL_CH_TYP:
            *valueP = M_CH_BINARY;
            break;
        /*--------------------------+
        |  irq counter              |
        +--------------------------*/
        case M_LL_IRQ_COUNT:
            *valueP = llHdl->irqCount;
            break;
        /*--------------------------+
        |  id prom check enabled    |
        +--------------------------*/
        case M_LL_ID_CHECK:
            *valueP = FALSE;
            break;
        /*--------------------------+
        |   id prom size            |
        +--------------------------*/
        case M_LL_ID_SIZE:
            *valueP = ID_SIZE;
            break;
        /*--------------------------+
        |   ident table pointer     |
        |   (treat as non-block!)    |
        +--------------------------*/
        case M_MK_BLK_REV_ID:
			*value64P = (INT32_OR_64)&llHdl->idFuncTbl;
			break;
        /*--------------------------+
        |  do nothing               |
        +--------------------------*/
        case MT_NOACTION:
            break;
        /*--------------------------+
        |  error modes              |
        +--------------------------*/
        case MT_READ_ERROR:
            *valueP = llHdl->readError;
            break;
        case MT_WRITE_ERROR:
            *valueP = llHdl->writeError;
            break;
        /*--------------------------+
        |  sleep modes              |
        +--------------------------*/
        case MT_READ_SLEEP:
            *valueP = llHdl->readSleep;
            break;
        case MT_WRITE_SLEEP:
            *valueP = llHdl->writeSleep;
            break;
        case MT_BLOCKREAD_SLEEP:
            *valueP = llHdl->blockReadSleep;
            break;
        case MT_BLOCKWRITE_SLEEP:
            *valueP = llHdl->blockWriteSleep;
            break;
        case MT_GETSTAT_SLEEP:
            *valueP = llHdl->getstatSleep;
            break;
        case MT_SETSTAT_SLEEP:
            *valueP = llHdl->setstatSleep;
            break;
        case MT_ONE_SLEEP:
            *valueP = llHdl->oneSleep;
            break;
        /*--------------------------+
        |  spin lock test           |
        +--------------------------*/
        case MT_SPINL_NO:
		{
			u_int32 realMsec;

			/*
			 * First call: Start alarm handler for spin lock test.
			 * Note: Will be active until last path closed.
			 */
			if( !llHdl->slNoCount ){
				if ((error = OSS_AlarmSet(
								llHdl->osHdl,
								llHdl->slAlmHdl[0],
								10, /* msec */
								1, /* cyclic */
								&realMsec ))){
					return(error);
				}
			}

			llHdl->slNoCount++;
            *valueP = llHdl->slNoCount;
            break;
		}
        case MT_SPINL_YES:
		{
			u_int32 realMsec;

			/*
			 * First call: Start alarm handler for spin lock test.
			 * Note: Will be active until last path closed.
			 */
			if( !llHdl->slYesCount ){
				if ((error = OSS_AlarmSet(
								llHdl->osHdl,
								llHdl->slAlmHdl[1],
								10, /* msec */
								1, /* cyclic */
								&realMsec ))){
					return(error);
				}
			}

			/* acquire spin lock */
			if ((error = OSS_SpinLockAcquire( llHdl->osHdl, llHdl->slHdl )))
				return(error);

			llHdl->slYesCount++;
            *valueP = llHdl->slYesCount;

			/* release spin lock */
			if ((error = OSS_SpinLockRelease( llHdl->osHdl, llHdl->slHdl )))
				return(error);

            break;
		}
		/*--------------------------+
        |  (unknown)                |
        +--------------------------*/
        default:
            error = ERR_LL_UNK_CODE;
    }

	/* handle sleep mode (if MT_NOACTION code) */
	if (llHdl->getstatSleep && code==MT_NOACTION)
		DoSleep(llHdl, &llHdl->getstatSleep);

	return(error);
}

/******************************* MT_BlockRead *******************************
 *
 *  Description:  Read data block from device
 *                Fills the buffer with linear pattern
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl        ll handle
 *                ch           current channel
 *                buf          data buffer
 *                size         data buffer size
 *  Output.....:  nbrRdBytesP  number of read bytes
 *                return       success (0) or error code
 *  Globals....:  ---
 ****************************************************************************/
int32 MT_BlockRead(
     LL_HANDLE *llHdl,
     int32     ch,
     void      *buf,
     int32     size,
     int32     *nbrRdBytesP
)
{
	u_int8 *bufP = (u_int8*) buf;
	int32 n;

    DBGWRT_1((DBH, "LL - MT_BlockRead: ch=%d, size=%d\n",ch,size));

	/* handle sleep mode */
	if (llHdl->blockReadSleep)
		DoSleep(llHdl, &llHdl->blockReadSleep);

	if( size <= MAX_BLOCK_SIZE )
		/* read  buffered block */
		for (n=0; n<size; n++)
			*bufP++ = llHdl->blk_buf[n];

	/* return nr of read bytes */
	*nbrRdBytesP = size;

	return(ERR_SUCCESS);
}

/****************************** MT_BlockWrite *******************************
 *
 *  Description:  Write data block to device
 *                Dump data and return size bytes
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl        ll handle
 *                ch           current channel
 *                buf          data buffer
 *                size         data buffer size
 *  Output.....:  nbrWrBytesP  number of written bytes
 *                return       success (0) or error code
 *  Globals....:  ---
 ****************************************************************************/
int32 MT_BlockWrite(
     LL_HANDLE *llHdl,
     int32     ch,
     void      *buf,
     int32     size,
     int32     *nbrWrBytesP
)
{
	u_int8 *bufP = (u_int8*) buf;
	int32 n;

    DBGWRT_1((DBH, "LL - MT_BlockWrite: ch=%d, size=%d\n",ch,size));

	/* handle sleep mode */
	if (llHdl->blockWriteSleep)
		DoSleep(llHdl, &llHdl->blockWriteSleep);

	/* dump buffer */
	DBGDMP_1((DBH, "block data",(void*)buf,size,1));

	if( size <= MAX_BLOCK_SIZE )
		/* write into block buffer */
		for (n=0; n<size; n++)
			llHdl->blk_buf[n] = *bufP++;

	/* return nr of written bytes */
	*nbrWrBytesP = size;

	return(ERR_SUCCESS);
}


/****************************** MT_Irq *************************************
 *
 *  Description:  Interrupt service routine (not used) LL_IRQ_UNKNOWN
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl    ll handle
 *  Output.....:  return   LL_IRQ_DEV_NOT   => not caused
 *                         LL_IRQ_DEVICE    => device has caused irq
 *                         LL_IRQ_UNKNOWN   => unknown
 *  Globals....:  ---
 ****************************************************************************/
int32 MT_Irq(
   LL_HANDLE *llHdl
)
{
    IDBGWRT_1((DBH, ">>> MT_Irq:\n"));

	/* ... */

    IDBGWRT_1((DBH, "<<< end MT_Irq\n"));
	return( LL_IRQ_UNKNOWN );
}/*MT_Irq*/

/****************************** MT_Info ************************************
 *
 *  Description:  Get driver info
 *
 *                Following info codes are supported:
 *
 *                Code                      Description
 *                ------------------------  -----------------------------
 *                LL_INFO_HW_CHARACTER      hardware characteristics
 *                LL_INFO_ADDRSPACE_COUNT   nr of required address spaces
 *                LL_INFO_ADDRSPACE         address space type
 *                LL_INFO_IRQ               interrupt required
 *                LL_INFO_LOCKMODE          process lock mode required
 *
 *---------------------------------------------------------------------------
 *  Input......:  infoType	   info code
 *                ...          argument(s)
 *  Output.....:  return       success (0) or error code
 *  Globals....:  ---
 ****************************************************************************/
int32 MT_Info(
   int32  infoType,
   ...
)
{
    int32   error = ERR_SUCCESS;
    va_list argptr;

    va_start(argptr, infoType );

    switch(infoType) {
		/*-------------------------------+
        |  hardware characteristics      |
        +-------------------------------*/
        case LL_INFO_HW_CHARACTER:
		{
			u_int32 *addrModeP = va_arg(argptr, u_int32*);
			u_int32 *dataModeP = va_arg(argptr, u_int32*);

			*addrModeP = MDIS_MA08;
			*dataModeP = MDIS_MD08;
			break;
	    }
		/*-------------------------------+
        |  nr of required address spaces |
        +-------------------------------*/
        case LL_INFO_ADDRSPACE_COUNT:
		{
			u_int32 *nbrOfAddrSpaceP = va_arg(argptr, u_int32*);

			*nbrOfAddrSpaceP = ADDRSPACE_COUNT;
			break;
	    }
		/*-------------------------------+
        |   address space type           |
        +-------------------------------*/
        case LL_INFO_ADDRSPACE:
		{
			u_int32 addrSpaceIndex = va_arg(argptr, u_int32);
			u_int32 *addrModeP = va_arg(argptr, u_int32*);
			u_int32 *dataModeP = va_arg(argptr, u_int32*);
			u_int32 *addrSizeP = va_arg(argptr, u_int32*);
			/* Prevent unused variable compiler warning */
			(void)addrSpaceIndex;
			(void)addrModeP;
			(void)dataModeP;
			(void)addrSizeP;

#ifndef MT_PCI
	#ifdef NO_HWACCESS
			error = ERR_LL_ILL_PARAM;
	#else
			if (addrSpaceIndex >= ADDRSPACE_COUNT)
				error = ERR_LL_ILL_PARAM;
			else {
				*addrModeP = MDIS_MA08;
				*dataModeP = MDIS_MD16;
				*addrSizeP = 0x100;
			}
	#endif
#else
			switch( addrSpaceIndex ){
			case 0:
				*addrModeP = MDIS_MA08;
				*dataModeP = MDIS_MD16;
				*addrSizeP = 0x10;
				break;
			case 1:
				*addrModeP = MDIS_MA08;
				*dataModeP = MDIS_MD16;
				*addrSizeP = 0x800;
				break;
			case 2:
				*addrModeP = MDIS_MA_PCICFG;
				*dataModeP = 0;
				*addrSizeP = 1;
				break;
			default:
				error = ERR_LL_ILL_PARAM;
			}
#endif
			break;
	    }
		/*-------------------------------+
        |   interrupt required           |
        +-------------------------------*/
        case LL_INFO_IRQ:
		{
			u_int32 *useIrqP = va_arg(argptr, u_int32*);

			*useIrqP = USE_IRQ;
			break;
	    }
		/*-------------------------------+
        |   process lock mode            |
        +-------------------------------*/
        case LL_INFO_LOCKMODE:
		{
			u_int32 *lockModeP = va_arg(argptr, u_int32*);

			#if( LOCKMODE )
			{
				#if ( LOCKMODE == 1 )
					*lockModeP = LL_LOCK_NONE;
				#endif
				#if ( LOCKMODE == 2 )
					*lockModeP = LL_LOCK_CALL;
				#endif
				#if ( LOCKMODE == 3 )
					*lockModeP = LL_LOCK_CHAN;
				#endif
			}
			#else
			{
				*lockModeP = *lockModeP; /* dummy access to avoid compiler warning */
				error = ERR_LL_ILL_PARAM;
			}
			#endif

			break;
	    }
		/*-------------------------------+
        |   (unknown)                    |
        +-------------------------------*/
        default:
          error = ERR_LL_ILL_PARAM;
    }

    va_end(argptr);
    return(error);
}

/*******************************  Ident  ************************************
 *
 *  Description:  Return ident string
 *
 *---------------------------------------------------------------------------
 *  Input......:  -
 *  Output.....:  return  ptr to ident string
 *  Globals....:  -
 ****************************************************************************/
static char* Ident( void )
{
    return( (char*) RCSid );
}

/********************************* Cleanup **********************************
 *
 *  Description: Close all handles, free memory and return error code
 *		         NOTE: The ll handle is invalid after calling this function
 *
 *---------------------------------------------------------------------------
 *  Input......: llHdl		ll handle
 *               retCode    return value
 *  Output.....: return	    retCode
 *  Globals....: -
 ****************************************************************************/
static int32 Cleanup(
   LL_HANDLE    *llHdl,
   int32        retCode
)
{
	u_int32 n;

    /*------------------------------+
    |  close handles                |
    +------------------------------*/
	/* clean up desc */
	if (llHdl->descHdl)
		DESC_Exit(&llHdl->descHdl);

	/* clean up alarms */
	for (n=0; n<MAX_ALARM; n++) {
		if (llHdl->alarmHdl[n])
			OSS_AlarmRemove(llHdl->osHdl, &llHdl->alarmHdl[n]);

		if (llHdl->sigHdl[n])
			OSS_SigRemove(llHdl->osHdl, &llHdl->sigHdl[n]);
	}

	/* clean up shared mem areas */
	for (n=0; n<MAX_SHMEM; n++)
		if (llHdl->smAlmHdl[n])
			OSS_AlarmRemove(llHdl->osHdl, &llHdl->smAlmHdl[n]);

#ifndef NO_SHARED_MEM
	if (llHdl->smHdl)
		OSS_SharedMemRemove(llHdl->osHdl, &llHdl->smHdl);
#endif
	/* clean up semaphohres */
	if (llHdl->semBinHdl)
	{
		/* binary semaphore */
		OSS_SemRemove(llHdl->osHdl, &llHdl->semBinHdl);
	}
	if (llHdl->semCntHdl) {
		/* counter sempahore */
		OSS_SemRemove(llHdl->osHdl, &llHdl->semCntHdl);
	}

#ifndef NO_CALLBACK
	/* clean up callbacks */
	for (n=0; n<MAX_CALLBACK; n++)
		if (llHdl->cbAlmHdl[n])
			OSS_AlarmRemove(llHdl->osHdl, &llHdl->cbAlmHdl[n]);

	if (llHdl->cbHdl)
		OSS_CallbackRemove(llHdl->osHdl, &llHdl->cbHdl);
#endif

	/* clean up spin lock test */
	for (n=0; n<MAX_SLALARM; n++)
		if (llHdl->slAlmHdl[n])
			OSS_AlarmRemove(llHdl->osHdl, &llHdl->slAlmHdl[n]);
	if (llHdl->slHdl)
		OSS_SpinLockRemove(llHdl->osHdl, &llHdl->slHdl);

	/* cleanup debug */
	DBGEXIT((&DBH));

    /*------------------------------+
    |  free memory                  |
    +------------------------------*/
    /* free my handle */
    OSS_MemFree(llHdl->osHdl, (int8*)llHdl, llHdl->memAlloc);

    /*------------------------------+
    |  return error code            |
    +------------------------------*/
	return(retCode);
}

/********************************* DoSleep **********************************
 *
 *  Description: Release devSem, sleep specified time and lock devSem
 *
 *---------------------------------------------------------------------------
 *  Input......: llHdl		ll handle
 *               msec       time to sleep [msec]
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/
static void DoSleep(
	LL_HANDLE *llHdl,
	u_int32 *msec
)
{
	u_int32 time = *msec;

	/* if one-shot: clear sleep mode before sleeping */
	if (llHdl->oneSleep)
		*msec = 0;

	/* release device (for other processes) */
	if( llHdl->devSemHdl )
		OSS_SemSignal(llHdl->osHdl, llHdl->devSemHdl);

	/* let current process sleep */
	DBGWRT_1((DBH," sleep msec=%d ..\n", time));
	OSS_Delay(llHdl->osHdl, time);
	DBGWRT_1((DBH," end sleep\n"));

	/* lock device */
	if( llHdl->devSemHdl )
		OSS_SemWait(llHdl->osHdl, llHdl->devSemHdl, OSS_SEM_WAITFOREVER);
}

/******************************* AlarmHandler0 ******************************
 *
 *  Description: Handler for alarm 0
 *
 *---------------------------------------------------------------------------
 *  Input......: arg		ll handle
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/
static void AlarmHandler0(void *arg)
{
	LL_HANDLE *llHdl = (LL_HANDLE*)arg;

	DBGWRT_1((DBH,">>> AlarmHandler0:\n"));

#ifndef NO_HWACCESS
	if (llHdl->trigEnable)
		MREAD_D16(llHdl->ma, llHdl->trigAlarm[0]);
#endif

    if (llHdl->sigHdl[0])
		OSS_SigSend(llHdl->osHdl, llHdl->sigHdl[0]);
}

/******************************* AlarmHandler1 ******************************
 *
 *  Description: Handler for alarm 1
 *
 *---------------------------------------------------------------------------
 *  Input......: arg		ll handle
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/
static void AlarmHandler1(void *arg)
{
	LL_HANDLE *llHdl = (LL_HANDLE*)arg;

	DBGWRT_1((DBH,">>> AlarmHandler1:\n"));

#ifndef NO_HWACCESS
	if (llHdl->trigEnable)
		MREAD_D16(llHdl->ma, llHdl->trigAlarm[1]);
#endif

    if (llHdl->sigHdl[1])
		OSS_SigSend(llHdl->osHdl, llHdl->sigHdl[1]);
}

#ifndef NO_CALLBACK
/******************************* CallbackHandler0 ***************************
 *
 *  Description: Handler for callback 0 (alarm)
 *
 *	             Trigger callback and pass callback 0 counter value
 *               as drvArg.
 *
 *---------------------------------------------------------------------------
 *  Input......: arg		ll handle
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/
static void CallbackHandler0(void *arg)
{
	LL_HANDLE *llHdl = (LL_HANDLE*)arg;

	DBGWRT_1((DBH,">>> CallbackHandler0:\n"));

#ifndef NO_HWACCESS
	if (llHdl->trigEnable)
		MREAD_D16(llHdl->ma, llHdl->trigCall[0]);
#endif

	OSS_CallbackExec(llHdl->osHdl,
					 llHdl->cbHdl,
					 0,
					 (void*)llHdl->cbCount[0]);

	llHdl->cbCount[0]++;
}

/******************************* CallbackHandler1 ***************************
 *
 *  Description: Handler for callback 1 (alarm)
 *
 *	             Trigger callback and pass callback 1 counter value
 *               as drvArg.
 *
 *---------------------------------------------------------------------------
 *  Input......: arg		ll handle
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/
static void CallbackHandler1(void *arg)
{
	LL_HANDLE *llHdl = (LL_HANDLE*)arg;

	DBGWRT_1((DBH,">>> CallbackHandler1:\n"));

#ifndef NO_HWACCESS
	if (llHdl->trigEnable)
		MREAD_D16(llHdl->ma, llHdl->trigCall[1]);
#endif

	OSS_CallbackExec(llHdl->osHdl,
					 llHdl->cbHdl,
					 1,
					 (void*)llHdl->cbCount[1]);

	llHdl->cbCount[1]++;
}
#endif

/******************************* SharedMemHandler0 ***************************
 *
 *  Description: Handler for shared memory 0 access (alarm)
 *
 *	             Checks the link counter of shared memory 0. If any links:
 *
 *               Fills the entire shared memory 0 with u_int32 values
 *               representing the nr of calls of this alarm routine.
 *
 *---------------------------------------------------------------------------
 *  Input......: arg		ll handle
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/
static void SharedMemHandler0(void *arg)
{
	LL_HANDLE *llHdl = (LL_HANDLE*)arg;
	u_int32 *p = (u_int32*)llHdl->smAddr[0];
	u_int32 cnt = llHdl->smSize[0] / 4;

	DBGWRT_1((DBH,">>> SharedMemHandler0:\n"));

	/* fill memory, if any links */
	if (p != NULL)
	{
		DBGWRT_2((DBH,"\tfill shmem at %08p with %08x\n", p, llHdl->smCount[0]));

		while(cnt--)
		{
			*p++ = llHdl->smCount[0];
		}/*while*/
	}
	else {
		DBGWRT_2((DBH,"\t(no links)\n"));
	}

	llHdl->smCount[0]++;
}

/******************************* SharedMemHandler1 ***************************
 *
 *  Description: Handler for shared memory access 1 (alarm)
 *
 *	             Checks the link counter of shared memory 1. If any links:
 *
 *               Fills the entire shared memory 1 with u_int32 values
 *               representing the nr of calls of this alarm routine.
 *
 *---------------------------------------------------------------------------
 *  Input......: arg		ll handle
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/
static void SharedMemHandler1(void *arg)
{
	LL_HANDLE *llHdl = (LL_HANDLE*)arg;
	u_int32 *p = (u_int32*)llHdl->smAddr[1];
	u_int32 cnt = llHdl->smSize[1] / 4;

	DBGWRT_1((DBH,">>> SharedMemHandler1: "));

	/* fill memory, if any links */
	if (p != NULL) {
		DBGWRT_2((DBH,"accessing\n"));

		while(cnt--)
			*p++ = llHdl->smCount[1];
	}
	else {
		DBGWRT_2((DBH,"(no links)\n"));
	}

	llHdl->smCount[1]++;
}

/******************************* SpinLockAlmHdlr0 ****************************
 *
 *  Description: Handler for spin lock alarm 0
 *
 *---------------------------------------------------------------------------
 *  Input......: arg		ll handle
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/
static void SpinLockAlmHdlr0(void *arg)
{
	LL_HANDLE *llHdl = (LL_HANDLE*)arg;
	u_int32 tmp;

	/* using no spin lock counter */
	tmp = llHdl->slNoCount;
	tmp++;
	OSS_MikroDelay( llHdl->osHdl, 100 );
	tmp--;
	llHdl->slNoCount = tmp;
}

/******************************* SpinLockAlmHdlr1 ****************************
 *
 *  Description: Handler for spin lock alarm 1
 *
 *---------------------------------------------------------------------------
 *  Input......: arg		ll handle
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/
static void SpinLockAlmHdlr1(void *arg)
{
	LL_HANDLE *llHdl = (LL_HANDLE*)arg;
	u_int32 tmp;

	/* acquire spin lock */
	OSS_SpinLockAcquire( llHdl->osHdl, llHdl->slHdl );
	
	/* using yes spin lock counter */
	tmp = llHdl->slYesCount;
	tmp++;
	OSS_MikroDelay( llHdl->osHdl, 100 );
	tmp--;
	llHdl->slYesCount = tmp;

	/* release spin lock */
	OSS_SpinLockRelease( llHdl->osHdl, llHdl->slHdl );
}







