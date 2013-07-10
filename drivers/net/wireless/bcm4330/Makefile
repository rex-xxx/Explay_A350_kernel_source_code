EXTRA_CFLAGS += -Dlinux -DLINUX -DBDC -DTOE -DBCMDRIVER -DBCMDONGLEHOST -DUNRELEASEDCHIP -DBCMDMA32 -DBCMFILEIMAGE -DDHDTHREAD \
    -DDHD_GPL -DDHD_SCHED -DDONGLEOVERLAYS -DSDTEST -DBCMSDIO -DBCMLXSDMMC -DBCMPLATFORM_BUS -DDHD_BCMEVENTS -DWIFI_ACT_FRAME \
    -DSHOW_EVENTS

################ OPTIONAL FEATURES #######################
# For AW70
EXTRA_CFLAGS += -DCUSTOMER_HW3

# For MAC address read
 EXTRA_CFLAGS += -DREAD_MACADDR

# SDIO_ISR_THREAD exclusive with OOB_INTR_ONLY
#EXTRA_CFLAGS += -DSDIO_ISR_THREAD


# For OOB only
EXTRA_CFLAGS += -DEMBEDDED_PLATFORM
EXTRA_CFLAGS += -DOOB_INTR_ONLY 
#EXTRA_CFLAGS += -DMMC_SDIO_ABORT

# For HW_OOB
#EXTRA_CFLAGS += -DHW_OOB  

# For SOFTAP
EXTRA_CFLAGS += -DSOFTAP -DWLBTAMP -DCSCAN  -DCUSTOM_OOB_GPIO_NUM=19 -DWIFI_ACT_FRAME

# Packet Filter
EXTRA_CFLAGS += -DPKT_FILTER_SUPPORT

# For Debug
EXTRA_CFLAGS += -DDHD_DEBUG -DSRCBASE=\"$(src)\"

# For WAPI
EXTRA_CFLAGS += -DBCMWAPI_WAI -DBCMWAPI_WPI

#include directorys
EXTRA_CFLAGS += -I$(src)/src/include/
EXTRA_CFLAGS += -I$(src)/src/dhd/sys/
EXTRA_CFLAGS += -I$(src)/src/dongle/
EXTRA_CFLAGS += -I$(src)/src/bcmsdio/sys/
EXTRA_CFLAGS += -I$(src)/src/wl/sys/
EXTRA_CFLAGS += -I$(src)/src/shared/

DHDOFILES =	src/dhd/sys/dhd_linux.o \
		src/dhd/sys/dhd_common.o \
		src/dhd/sys/dhd_cdc.o \
		src/dhd/sys/dhd_bta.o \
		src/dhd/sys/dhd_linux_sched.o\
		src/dhd/sys/dhd_sdio.o \
		src/dhd/sys/dhd_custom_gpio.o \
		src/shared/aiutils.o \
		src/shared/bcmutils.o \
		src/shared/bcmwifi.o \
		src/shared/hndpmu.o \
		src/shared/linux_osl.o \
		src/shared/sbutils.o \
		src/shared/siutils.o \
		src/shared/bcmevent.o \
		src/bcmsdio/sys/bcmsdh_sdmmc.o \
		src/bcmsdio/sys/bcmsdh.o \
		src/bcmsdio/sys/bcmsdh_linux.o \
		src/bcmsdio/sys/bcmsdh_sdmmc_linux.o \
		src/wl/sys/wl_iw.o \
		src/wl/sys/wl_android.o	\
		src/wl/sys/wldev_common.o	\
		src/dhd/sys/dhd_custom_lct.o	

obj-$(CONFIG_BCM4330) += dhd.o
dhd-objs += $(DHDOFILES)
EXTRA_LDFLAGS += --strip-debug
