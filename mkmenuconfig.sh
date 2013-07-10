#!/bin/bash

KERNEL_OUT=../out/target/product/msm7627a/obj/KERNEL_OBJ
KERNEL_DEFCONFIG=msm7627a-perf_aw551_yl_defconfig

cp  arch/arm/configs/$KERNEL_DEFCONFIG $KERNEL_OUT/.config
env KCONFIG_NOTIMESTAMP=true \
     make O=$KERNEL_OUT ARCH=arm CROSS_COMPILE=arm-eabi- menuconfig
cp $KERNEL_OUT/.config arch/arm/configs/$KERNEL_DEFCONFIG
