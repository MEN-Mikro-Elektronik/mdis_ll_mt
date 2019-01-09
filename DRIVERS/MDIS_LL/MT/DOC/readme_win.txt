Windows specifics for MT MDIS5 driver                              10/14/2009 dp
=====================================


MT driver
---------
The Windows MT device driver (men_mt.sys) can be installed for an M-Module
(e.g. M66) instead of any other M-Module driver (e.g. men_m66.sys) and
requires an installed MDIS5 BBIS instance (e.g. men_d201).

  
MT_NOHW driver
--------------  
The Windows MT_NOHW driver (men_mt_nohw.sys) can be installed without any
MDIS5 BBIS instance.
To install the Windows MT_NOHW driver, proceed as follows:

* Use the Add Hardware Wizard accessible via the Control Panel to install the
  device driver.

* Choose the proper wizard options to install a new device manually and specify
  the location of the mt.inf file belonging to the MT_NOHW driver.
  Then select the MT_NOHW model and continue the installation with the wizard.


Please refer to the MDIS5 under Windows user manual for further information about
the installation of MEN's Windows MDIS5 drivers.
