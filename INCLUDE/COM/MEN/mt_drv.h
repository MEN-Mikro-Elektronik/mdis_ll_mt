/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: mt_drv.h
 *
 *       Author: see
 *        $Date: 2010/12/01 12:20:21 $
 *    $Revision: 2.7 $
 *
 *  Description: Header file for MT driver
 *               - MT specific status codes
 *               - MT function prototypes
 *
 *     Switches: _ONE_NAMESPACE_PER_DRIVER_
 *               _LL_DRV_
 *
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

#ifndef _MT_LLDRV_H
#define _MT_LLDRV_H

#ifdef __cplusplus
      extern "C" {
#endif

/*-----------------------------------------+
|  TYPEDEFS                                |
+-----------------------------------------*/
/* structure for the MT_BLK_ALARM_SET setstats */
typedef struct {
	u_int32 nr;				/* alarm index (0..1) */
	u_int32 msec;			/* alarm period [msec] */
	u_int32 cyclic;			/* cyclic mode (0=single, 1=cyclic) */
	u_int32 signal;			/* signal to send */
} MT_BLK_ALARM;

/* structure for the MT_BLK_CALLBACK_SET setstats */
typedef struct {
    u_int32 nr;				/* callback function index (0..1) */
	u_int32 msec;			/* callback period [msec] */
	u_int32 cyclic;			/* cyclic mode (0=single, 1=cyclic) */
} MT_BLK_CALLBACK;

/* structure for the MT_BLK_SHMEM_SET setstats */
typedef struct {
    u_int32 nr;				/* shared memory area index (0..1) */
	u_int32 msec;			/* access period [msec] */
	u_int32 cyclic;			/* cyclic mode (0=single, 1=cyclic) */
} MT_BLK_SHMEM;

/*-----------------------------------------+
|  DEFINES                                 |
+-----------------------------------------*/
/* MT specific status codes (STD) */           /* S,G: S=setstat, G=getstat */
#define MT_IRQCOUNT         M_DEV_OF+0x00      /* G,S: irq counter */
#define MT_NOACTION         M_DEV_OF+0x01      /* G,S: do nothing */
#define MT_BUSYLOOP			M_DEV_OF+0x02 	   /*   S: busy loop */
#define MT_MDELAY			M_DEV_OF+0x03 	   /*   S: test microdelay */

#define MT_READ_ERROR       M_DEV_OF+0x10      /* G,S: produce read errors */
#define MT_WRITE_ERROR      M_DEV_OF+0x11      /* G,S: produce write errors */

#define MT_READ_SLEEP       M_DEV_OF+0x20      /* G,S: read sleep [msec] */
#define MT_WRITE_SLEEP      M_DEV_OF+0x21      /* G,S: write sleep [msec] */
#define MT_BLOCKREAD_SLEEP  M_DEV_OF+0x22      /* G,S: blockread sleep [msec] */
#define MT_BLOCKWRITE_SLEEP M_DEV_OF+0x23      /* G,S: blockwrite sleep [msec] */
#define MT_GETSTAT_SLEEP    M_DEV_OF+0x24      /* G,S: getstat sleep [msec] */
#define MT_SETSTAT_SLEEP    M_DEV_OF+0x25      /* G,S: setstat sleep [msec] */
#define MT_ONE_SLEEP        M_DEV_OF+0x26      /* G,S: one-shot sleep mode */

#define MT_ALARM_CLR        M_DEV_OF+0x30      /*   S: de-activate alarm */
#define MT_CALLBACK_CLR     M_DEV_OF+0x31      /*   S: de-activate callback */
#define MT_SHMEM_CLR     	M_DEV_OF+0x32      /*   S: de-activate shmem access */

#define MT_SEM_BIN_TAK		M_DEV_OF+0x40	   /*   S: take binary semphore */
#define MT_SEM_BIN_REL		M_DEV_OF+0x41	   /*   S: release binary semphore */
#define MT_SEM_CNT_TAK		M_DEV_OF+0x42	   /*   S: take counter semphore */
#define MT_SEM_CNT_REL		M_DEV_OF+0x43	   /*   S: release counter semphore */

#define MT_SPINL_NO			M_DEV_OF+0x50	   /* G  : increment and get counter
											           and start cyclic alarm */
#define MT_SPINL_YES		M_DEV_OF+0x51	   /* G  : increment and get counter
											           with spin lock */

/* MT specific status codes (BLK) */           /* S,G: S=setstat, G=getstat */
#define MT_BLK_ALARM_SET    M_DEV_BLK_OF+0x00  /*   S: activate alarm */
#define MT_BLK_CALLBACK_SET M_DEV_BLK_OF+0x01  /*   S: activate callback */
#define MT_BLK_SHMEM_SET    M_DEV_BLK_OF+0x02  /*   S: activate shmem access */

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
#ifdef _LL_DRV_

#ifndef _ONE_NAMESPACE_PER_DRIVER_
	extern void MT_GetEntry_LockMode0(LL_ENTRY* drvP); /* unkown  locking */
	extern void MT_GetEntry_LockMode1(LL_ENTRY* drvP); /* no      locking */
	extern void MT_GetEntry_LockMode2(LL_ENTRY* drvP); /* call    locking */
	extern void MT_GetEntry_LockMode3(LL_ENTRY* drvP); /* channel locking */
#endif


#endif /* _LL_DRV_ */

/*-----------------------------------------+
|  BACKWARD COMPATIBILITY TO MDIS4         |
+-----------------------------------------*/
#ifndef U_INT32_OR_64
 /* we have an MDIS4 men_types.h and mdis_api.h included */
 /* only 32bit compatibility needed!                     */
 #define INT32_OR_64  int32
 #define U_INT32_OR_64 u_int32
 typedef INT32_OR_64  MDIS_PATH;
#endif /* U_INT32_OR_64 */

#ifdef __cplusplus
      }
#endif

#endif /* _MT_LLDRV_H */



