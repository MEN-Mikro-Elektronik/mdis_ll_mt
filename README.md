# MDIS5 MT Low-Level Driver Package

## Overview

- MT driver development 1998..2010
- Test driver and tools for MDIS testing
- Used for Verification of the MDIS System Package for Windows
- Windows: Possibility, to install the mt_nohw driver without BBIS and real HW
- Useful for debugging of MDIS functionality



## MT Driver

### Driver Variants

| Driver Name | mak File | Usage | Lock Mode | HW Access | Addr Spaces |
| ----------- | -------- | ----- | --------- | --------- | ----------- |
| mt | driver.mak | General Testing (see next slide) | undefined | x | 1 |
| mt_lm1 | driver_lm1.mak | Locking mode testing | No Locking LL_LOCK_NONE | x | 1 |
| mt_lm2 | driver_lm2.mak | Locking mode testing | Call Locking LL_LOCK_CALL | x | 1 |
| mt_lm3 | driver_lm3.mak | Locking mode testing | Channel Locking LL_LOCK_CHAN | x | 1 |
| mt_pci | driver_pci.mak | Test MK's PCI functionality, PCI Config Space access (OSS_PciGetConfig) | undefined | x | 3 |
| mt_nohw | driver_nohw.mak | Test without any HW access | undefined |  | 0 |


### Driver Functionality

- Simple Low-Level Driver for MDIS functionality testing
- Signals (OSS_Sig*)
- Callbacks (OSS_Callback*)
- Shared Memory access (OSS_SharedMem*)
- Semaphores (OSS_Sem*)
- Spin Locks (OSS_SpinLock*)
- Alarms (OSS_Alarm_*)
- Micro Delay (OSS_MikroDelay)
- Error simulation


[MT driver documentation (auto generated)](DRIVERS/MDIS_LL/MT/DOC/mt_drv.html)



## MT Tools

### Tools Overview
The MT driver package contains several tools for MDIS testing.

[Overview of included tools (auto generated)](DRIVERS/MDIS_LL/MT/DOC/tools.txt)

The  mt_irqmax and mt_irqrate tools require a M99 M-Module with M99 MDIS Low-Level driver (instead of one of the MT driver variants). Both tools require the  m99_drv.h header file, that is included in the M99 MDIS Low-Level Driver Package.

### Tools Usage Examples

#### Path handling testing with mt_nohw driver
Use mt_pathes tool to open/close MDIS paths:
```mt_pathes mt_nohw_1 -n=30 -r=15 -v```

#### I/O Mode testing with mt_nohw driver
Use mt_iomode tool to switch MDIS channels and read/write data:
```mt_iomode mt_nohw_1 -n=5 -v```

#### Benchmark testing with mt_nohw driver
Use mt_bench tool to get performance results of MDIS calls:
```mt_bench mt_nohw_1 -p=20 -c=30```

#### Alarms testing with mt_nohw driver
Use mt_alarm tool to test the alarm functions of the OSS library:
```mt_alarm mt_nohw_1 -0=2000 -1=3000 -m=1 -n=20```

#### Spin Locks testing with mt_nohw driver
Start mt_spinltest tool to test the spin lock functions of the OSS library:
```mt_spinltest mt_nohw_1 -d=1```
Wait about 5 minutes then press any key to end the test.

#### Interrupt testing with M99 driver/M-Module

Use mt_irqmax tool to get the maximum interrupt frequency:
```mt_irqmax m99_1 -d=10```

Start mt_irqrate tool, then wait about five minutes to show the interrupt rate:
```mt_irqrate m99_1 -r=10```

