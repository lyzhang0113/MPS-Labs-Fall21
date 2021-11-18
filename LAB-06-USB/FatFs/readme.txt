2018-11-15 - Kyle Wilt - Originated

FatFs Driver:

This driver is a re-organized version of the driver provided in STM32CubeF7
to allow for easier inclusion into the Microprocessor Systems Lab 6 USB project.
Some files provided from STM32CubeF7 are not included as they are not needed
for the course project.
Most importantly: only the USBH driver is included. 
		uSD and RAM drivers are not included.
		
In ff.h, the FILINFO typedef has been changed from the original to by default
allocate memory to the lfname field.  In the provided version, the lfname field
was simply a pointer which never gets allocated prior to use (unsure why).

ALSO NOTE: This driver is several versions behind the generic non-stm32 
		FatFs Driver found here: http://elm-chan.org/fsw/ff/00index_e.html