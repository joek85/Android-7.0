/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __OEMNDT_H__
#define __OEMNDT_H__

#include <oemcommon.h>
#include <drmprndprotocoltypes.h>

ENTER_PK_NAMESPACE;

/*
** OEM_MANDATORY:
** If the device supports PlayReady-ND-Tx (PRND-Tx), this function MUST be implemented by the OEM.
*/
DRM_API DRM_RESULT DRM_CALL Oem_Ndt_AllowNewDeviceRegistration(
    __in     DRM_BYTEBLOB f_DeviceCertificate,
    __in     DRM_BYTEBLOB f_DeviceSerialNumber );

/*
** OEM_MANDATORY:
** If the device supports PlayReady-ND-Tx (PRND-Tx), this function MUST be implemented by the OEM.
*/
DRM_API DRM_RESULT DRM_CALL Oem_Ndt_GetProximityDetectionType(
    __out_ecount(1) DRM_PRND_PROXIMITY_DETECTION_TYPE   *f_peProximityDetectionType );

/*
** OEM_MANDATORY:
** If the device supports PlayReady-ND-Tx (PRND-Tx), this function MUST be implemented by the OEM.
*/
DRM_API DRM_RESULT DRM_CALL Oem_Ndt_GetProximityDetectionChannel(
    __out_bcount_opt(*f_pcbChannel) DRM_BYTE       *f_pbChannel,
    __inout                         DRM_WORD       *f_pcbChannel );

/*
** If the device supports PlayReady-ND-Tx (PRND-Tx), this function is optional.
*/
DRM_API DRM_RESULT DRM_CALL Oem_Ndt_SetProximityDetectionType(
    __in DRM_PRND_PROXIMITY_DETECTION_TYPE  f_eProximityDetectionType );

/*
** If the device supports PlayReady-ND-Tx (PRND-Tx), this function is optional.
*/
DRM_API DRM_RESULT DRM_CALL Oem_Ndt_SetProximityDetectionChannel(
    __in_ecount(f_cbChannel)  const DRM_BYTE       *f_pbChannel,
    __in                            DRM_WORD        f_cbChannel );

EXIT_PK_NAMESPACE;

#endif /*  __OEMNDT_H__ */

