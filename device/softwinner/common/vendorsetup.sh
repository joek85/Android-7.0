#
# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# This file is executed by build/envsetup.sh, and can use anything
# defined in envsetup.sh.
#
# In particular, you can add lunch options with the add_lunch_combo
# function: add_lunch_combo generic-eng

#!/bin/bash

export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64
export PATH=$JAVA_HOME/bin:$PATH
export CLASSPATH=.:$JAVA_HOME/lib

function cdevice()
{	
	cd $DEVICE
}

function cout()
{
	cd $OUT	
}

function extract-bsp()
{
#	LICHEE_DIR=$ANDROID_BUILD_TOP/../lichee
	LICHEE_DIR=/joedev/h3-android-7-0-2017-10-13/lichee
    CHIP_VERSION=$(get_build_var SW_CHIP_PLATFORM)
    if [ "$CHIP_VERSION" = "H8" ];then
	    LINUXOUT_DIR=$LICHEE_DIR/out/sun8iw6p1/android/common
	elif [ "$CHIP_VERSION" = "H3" ];then
		LINUXOUT_DIR=$LICHEE_DIR/out/sun8iw7p1/android/common
    elif [ "$CHIP_VERSION" = "A80" ];then
	    LINUXOUT_DIR=$LICHEE_DIR/out/sun9iw1p1/android/common
    elif [ "$CHIP_VERSION" = "H64" ];then
	    LINUXOUT_DIR=$LICHEE_DIR/out/sun50iw1p1/android/common
    elif [ "$CHIP_VERSION" = "H5" ];then
	    LINUXOUT_DIR=$LICHEE_DIR/out/sun50iw2p1/android/common
    elif [ "$CHIP_VERSION" = "H6" ];then
	    LINUXOUT_DIR=$LICHEE_DIR/out/sun50iw6p1/android/common
    else 
        echo "unknow CHIP_VERSION $CHIP_VERSION"
        return
    fi
	LINUXOUT_MODULE_DIR=$LINUXOUT_DIR/lib/modules/*/*
	CURDIR=$PWD

	cd $DEVICE

	#extract kernel
	if [ -f kernel ] ; then
		rm kernel
	fi
	cp $LINUXOUT_DIR/bImage kernel
	echo "$DEVICE/bImage copied!"

	#extract linux modules
	if [ -d modules ] ; then
		rm -rf modules
	fi
	mkdir -p modules/modules
	cp -rf $LINUXOUT_MODULE_DIR modules/modules
	echo "$DEVICE/modules copied!"
	chmod 0755 modules/modules/*

# create modules.mk
(cat << EOF) > ./modules/modules.mk 
# modules.mk generate by extract-files.sh, do not edit it.
PRODUCT_COPY_FILES += \\
	\$(call find-copy-subdir-files,*,\$(LOCAL_PATH)/modules,system/vendor/modules)
EOF

	cd $CURDIR
}

function pack()
{
	T=$(gettop)
	export ANDROID_IMAGE_OUT=$OUT
	export PACKAGE=$T/../lichee/tools/pack

	sh $DEVICE/package.sh $*
}

function get_uboot()
{
    echo "get_uboot paramter: $@"
    local chip=$(get_build_var SW_CHIP_PLATFORM)
    pack -n -s -f $@
    if [ ! -e $OUT/bootloader ] ; then
        mkdir $OUT/bootloader
    fi
    rm -rf $OUT/bootloader/*
    cp -v $PACKAGE/out/bootloader.fex $OUT
    cp -r $PACKAGE/out/boot-resource/* $OUT/bootloader
    echo "\"$PACKAGE/out/boot-resource/* -> $OUT/bootloader/\""
    cp -v $PACKAGE/out/env.fex $OUT
    cp -v $PACKAGE/out/boot0_nand.fex $OUT
    cp -v $PACKAGE/out/boot0_sdcard.fex $OUT

    cp -v $PACKAGE/out/boot_package.fex $OUT/uboot_nand.fex
    cp -v $PACKAGE/out/boot_package.fex $OUT/uboot_sdcard.fex
    cp -v $PACKAGE/out/sys_config.fex $OUT/board_config.fex
    cp -v $PACKAGE/out/sys_config.bin $OUT/board_config.bin

	if [ -e $PACKAGE/out/toc0.fex ]; then
		cp -v $PACKAGE/out/toc0.fex $OUT/toc0.fex
	fi

	if [ -e $PACKAGE/out/toc1.fex ]; then
		cp -v $PACKAGE/out/toc1.fex $OUT/toc1.fex
	fi
}

function get_uboot_unsigned()
{
    pack -s -f -n $@
    if [ ! -e $OUT/bootloader ] ; then
        mkdir $OUT/bootloader
    fi
    rm -rf $OUT/bootloader/*
    cp -v $PACKAGE/out/bootloader.fex $OUT
    cp -r $PACKAGE/out/boot-resource/* $OUT/bootloader
    echo "\"$PACKAGE/out/boot-resource/* -> $OUT/bootloader/\""
    cp -v $PACKAGE/out/env.fex $OUT
    cp -v $PACKAGE/out/boot0_nand.fex $OUT
    cp -v $PACKAGE/out/boot0_sdcard.fex $OUT
    cp -v $PACKAGE/out/boot_package.fex $OUT/uboot_nand.fex
    cp -v $PACKAGE/out/boot_package.fex $OUT/uboot_sdcard.fex
    cp -v $PACKAGE/out/boot_package.fex $OUT/toc1.fex
    cp -v $PACKAGE/out/boot0_nand.fex $OUT/toc0.fex
}

function get_uboot_signed_wrong()
{
    cp ${ANDROID_BUILD_TOP}/build/tools/releasetools/secureboot_sign_tools/WrongTrustkey.pem ${ANDROID_BUILD_TOP}/../lichee/tools/pack/common/keys/Trustkey.pem
    pack -s -f -n $@
    cp ${ANDROID_BUILD_TOP}/build/tools/releasetools/secureboot_sign_tools/Trustkey.pem ${ANDROID_BUILD_TOP}/../lichee/tools/pack/common/keys/Trustkey.pem
    if [ ! -e $OUT/bootloader ] ; then
        mkdir $OUT/bootloader
    fi
    rm -rf $OUT/bootloader/*
    cp -v $PACKAGE/out/bootloader.fex $OUT
    cp -r $PACKAGE/out/boot-resource/* $OUT/bootloader
    echo "\"$PACKAGE/out/boot-resource/* -> $OUT/bootloader/\""
    cp -v $PACKAGE/out/env.fex $OUT
    cp -v $PACKAGE/out/boot0_nand.fex $OUT
    cp -v $PACKAGE/out/boot0_sdcard.fex $OUT
    cp -v $PACKAGE/out/boot_package.fex $OUT/uboot_nand.fex
    cp -v $PACKAGE/out/boot_package.fex $OUT/uboot_sdcard.fex

    if [ -e $PACKAGE/out/toc0.fex ]; then
        cp -v $PACKAGE/out/toc0.fex $OUT/toc0.fex
    fi

    if [ -e $PACKAGE/out/toc1.fex ]; then
        cp -v $PACKAGE/out/toc1.fex $OUT/toc1.fex
    fi

    if [ -e $PACKAGE/out/verity_block.fex ]; then
        cp -v $PACKAGE/out/verity_block.fex $OUT/verity_block.fex
    fi
}

function make_ota_target_file()
{
  get_uboot $@
  echo "rm $OUT/obj/PACKAGING/target_files_intermediates/"
  rm -rf $OUT/obj/PACKAGING/target_files_intermediates/
  echo "---make target-files-package---"
  make target-files-package

}

function make_ota_package()
{
  get_uboot $@
  echo "rm $OUT/obj/PACKAGING/target_files_intermediates/"
  rm -rf $OUT/obj/PACKAGING/target_files_intermediates/
  echo "----make otapackage ----"
  make otapackage -j16
}

function make_ota_package_inc()
{
  mv *.zip old_target_files.zip
  get_uboot $@
  echo "rm $OUT/obj/PACKAGING/target_files_intermediates/"
  rm -rf $OUT/obj/PACKAGING/target_files_intermediates/
  echo "----make otapackage_inc----"
  make otapackage_inc
}

function make_ota_signed_package()
{
    make_ota_package
    OTA_PACKAGE=$(ls -Art $OUT | grep '[0-9]\{8\}.zip' |tail -n 1)
    ./build/tools/releasetools/secureboot_sign_tools/sign_ota.sh -i "$OUT/$OTA_PACKAGE" -k build/tools/releasetools/secureboot_sign_tools/Trustkey.pem -o /tmp/ota.zip
    OTA_SIGNED=${OTA_PACKAGE%-*}"_signed.zip"
    mv /tmp/ota.zip "${OUT}/${OTA_SIGNED}"
    rm ${OUT}/${OTA_PACKAGE}
}

function make_ota_signed_wrong_package()
{
    make_ota_package
    OTA_PACKAGE=$(ls -Art $OUT | grep '.zip' |tail -n 1)
    echo $OTA_PACKAGE
    ./build/tools/releasetools/secureboot_sign_tools/sign_ota.sh -i "$OUT/$OTA_PACKAGE" -k build/tools/releasetools/secureboot_sign_tools/WrongTrustkey.pem -o /tmp/ota.zip
    OTA_SIGNED_WRONG=${OTA_PACKAGE%-*}"_signed_wrong.zip"
    mv /tmp/ota.zip "${OUT}/${OTA_SIGNED_WRONG}"
    rm ${OUT}/${OTA_PACKAGE}
}

function make_ota_unsigned_package()
{
    make_ota_package
    OTA_PACKAGE=$(ls -Art $OUT | grep '.zip' |tail -n 1)
    OTA_SIGNED_WRONG=${OTA_PACKAGE%-*}"_unsigned.zip"
    mv ${OUT}/${OTA_PACKAGE} "${OUT}/${OTA_SIGNED_WRONG}"
}

function make_ota_signed_uboot_package()
{
    make_ota_uboot_package
    OTA_PACKAGE=$(ls -Art $OUT | grep 'boottest.zip' |tail -n 1)
    ./build/tools/releasetools/secureboot_sign_tools/sign_ota_testboot.sh -i "$OUT/$OTA_PACKAGE" -k build/tools/releasetools/secureboot_sign_tools/Trustkey.pem -o /tmp/ota.zip
    OTA_SIGNED=${OTA_PACKAGE%-*}"_signed_uboot.zip"
    mv /tmp/ota.zip "${OUT}/${OTA_SIGNED}"
    rm ${OUT}/${OTA_PACKAGE}
}

function make_ota_unsigned_uboot_package()
{
    get_uboot_unsigned
    echo "rm $OUT/obj/PACKAGING/target_files_intermediates/"
    rm -rf $OUT/obj/PACKAGING/target_files_intermediates/
    echo "----make unsigned otapackage ----"
    make otapackage_boottest -j16

    OTA_PACKAGE=$(ls -Art $OUT | grep 'boottest.zip' |tail -n 1)
    OTA_UNSIGNED=${OTA_PACKAGE%-*}"_unsigned_uboot.zip"
    mv ${OUT}/${OTA_PACKAGE} "${OUT}/${OTA_UNSIGNED}"
}

function make_ota_signed_wrong_uboot_package()
{
    make_ota_uboot_package
    OTA_PACKAGE=$(ls -Art $OUT | grep 'boottest.zip' |tail -n 1)
    ./build/tools/releasetools/secureboot_sign_tools/sign_ota_testboot.sh -i "$OUT/$OTA_PACKAGE" -k build/tools/releasetools/secureboot_sign_tools/WrongTrustkey.pem -o /tmp/ota.zip
    OTA_SIGNED=${OTA_PACKAGE%-*}"_signed_wrong_uboot.zip"
    mv /tmp/ota.zip "${OUT}/${OTA_SIGNED}"
    rm ${OUT}/${OTA_PACKAGE}
}

function make_ota_signed_kernel_package()
{
    make_ota_kernel_package
    OTA_PACKAGE=$(ls -Art $OUT | grep 'kerneltest.zip' |tail -n 1)
    ./build/tools/releasetools/secureboot_sign_tools/sign_ota_testkernel.sh -i "$OUT/$OTA_PACKAGE" -k build/tools/releasetools/secureboot_sign_tools/Trustkey.pem -o /tmp/ota.zip -f 0
    OTA_SIGNED=${OTA_PACKAGE%-*}"_signed_kernel.zip"
    rm ${OUT}/${OTA_PACKAGE}
    mv /tmp/ota.zip "${OUT}/${OTA_SIGNED}"
}

function make_ota_unsigned_kernel_package()
{
    make_ota_kernel_package
    OTA_PACKAGE=$(ls -Art $OUT | grep 'kerneltest.zip' |tail -n 1)
    OTA_UNSIGNED=${OTA_PACKAGE%-*}"_unsigned_kernel.zip"
    mv ${OUT}/${OTA_PACKAGE} "${OUT}/${OTA_UNSIGNED}"
}

function make_ota_signed_wrong_kernel_package()
{
    make_ota_kernel_package
    OTA_PACKAGE=$(ls -Art $OUT | grep 'kerneltest.zip' |tail -n 1)
    ./build/tools/releasetools/secureboot_sign_tools/sign_ota_testkernel.sh -i "$OUT/$OTA_PACKAGE" -k build/tools/releasetools/secureboot_sign_tools/WrongTrustkey.pem -o /tmp/ota.zip -f 1
    OTA_SIGNED_WRONG=${OTA_PACKAGE%-*}"_signed_wrong_kernel.zip"
    rm ${OUT}/${OTA_PACKAGE}
    mv /tmp/ota.zip "${OUT}/${OTA_SIGNED_WRONG}"
}

function make_ota_kernel_package()
{
    get_uboot
    echo "rm $OUT/obj/PACKAGING/target_files_intermediates/"
    rm -rf $OUT/obj/PACKAGING/target_files_intermediates/
    echo "----make kernel otapackage ----"
    make otapackage_kerneltest -j16
}

function make_ota_uboot_package()
{
    get_uboot
    echo "rm $OUT/obj/PACKAGING/target_files_intermediates/"
    rm -rf $OUT/obj/PACKAGING/target_files_intermediates/
    echo "----make otapackage ----"
    make otapackage_boottest -j16
}

function make_ota_uboot_unsigned_package()
{
    get_uboot_unsigned
    echo "rm $OUT/obj/PACKAGING/target_files_intermediates/"
    rm -rf $OUT/obj/PACKAGING/target_files_intermediates/
    echo "----make unsigned otapackage ----"
    make otapackage_boottest -j16
}

function verity_key_init()
{
	cd build/tools/verity;./gen_dm_verity_key.sh;croot
}

function verity_data_init()
{
	echo "----verity_data_init----"
	device/softwinner/common/verity/gen_dm_verity_data.sh ${OUT}/system.img ${OUT}/verity_block
	cp -vf ${OUT}/verity_block ${OUT}/verity_block.img
}

function make_toc_package()
{
	$DEVICE/package_toc.sh
}

