/*****************************************************************
|
|   Wasabi - Result Codes
|
|   $Id: WsbResults.h 10793 2014-06-25 12:46:08Z jebaseelir $
|   Original author: Julien Boeuf
|
|   This software is provided to you pursuant to your agreement 
|   with Intertrust Technologies Corporation ("Intertrust"). 
|   This software may be used only in accordance with the terms 
|   of the agreement.
|      
|   Copyright (c) 2005-2014 by Intertrust. All rights reserved. 
|
****************************************************************/
/** 
* @file
* @brief Result codes
*/

#ifndef _WSB_RESULTS_H_
#define _WSB_RESULTS_H_

/*----------------------------------------------------------------------
|   macros
+---------------------------------------------------------------------*/
#define WSB_SUCCESS   0
#define WSB_FAILURE (-1)

#define WSB_FAILED(_result)    ((_result) != WSB_SUCCESS)
#define WSB_SUCCEEDED(_result) ((_result) == WSB_SUCCESS)

/*----------------------------------------------------------------------
|   error code bases    
+---------------------------------------------------------------------*/
#if !defined(WSB_ERROR_BASE)
#define WSB_ERROR_BASE (-100000)
#endif

/* error codes bases for the external API */
#define WSB_ERROR_BASE_GENERAL             (WSB_ERROR_BASE - 0   )
/* general error codes */
#define WSB_ERROR_OUT_OF_MEMORY            (WSB_ERROR_BASE_GENERAL - 0)
#define WSB_ERROR_OUT_OF_RESOURCES         (WSB_ERROR_BASE_GENERAL - 1)
#define WSB_ERROR_INTERNAL                 (WSB_ERROR_BASE_GENERAL - 2)
#define WSB_ERROR_INVALID_PARAMETERS       (WSB_ERROR_BASE_GENERAL - 3)
#define WSB_ERROR_INVALID_STATE            (WSB_ERROR_BASE_GENERAL - 4)
#define WSB_ERROR_NOT_IMPLEMENTED          (WSB_ERROR_BASE_GENERAL - 5)
#define WSB_ERROR_OUT_OF_RANGE             (WSB_ERROR_BASE_GENERAL - 6)
#define WSB_ERROR_ACCESS_DENIED            (WSB_ERROR_BASE_GENERAL - 7)
#define WSB_ERROR_INVALID_SYNTAX           (WSB_ERROR_BASE_GENERAL - 8)
#define WSB_ERROR_NOT_SUPPORTED            (WSB_ERROR_BASE_GENERAL - 9)
#define WSB_ERROR_INVALID_FORMAT           (WSB_ERROR_BASE_GENERAL - 10)
#define WSB_ERROR_NOT_ENOUGH_SPACE         (WSB_ERROR_BASE_GENERAL - 11)
#define WSB_ERROR_NO_SUCH_ITEM             (WSB_ERROR_BASE_GENERAL - 12)
#define WSB_ERROR_NO_SUCH_INTERFACE        (WSB_ERROR_BASE_GENERAL - 13)
#define WSB_ERROR_NOT_AVAILABLE            (WSB_ERROR_BASE_GENERAL - 14)
#define WSB_ERROR_TIMEOUT                  (WSB_ERROR_BASE_GENERAL - 15)
#define WSB_ERROR_RUNTIME_NOT_INITED       (WSB_ERROR_BASE_GENERAL - 16)
#define WSB_ERROR_INVALID_PATH             (WSB_ERROR_BASE_GENERAL - 17)
#define WSB_ERROR_CANNOT_OPEN_FILE         (WSB_ERROR_BASE_GENERAL - 18) 
#define WSB_ERROR_EOS                      (WSB_ERROR_BASE_GENERAL - 19)  
#define WSB_ERROR_INCOMPLETE_MEDIA         (WSB_ERROR_BASE_GENERAL - 20)
#define WSB_ERROR_STREAM_MISSING           (WSB_ERROR_BASE_GENERAL - 21)

#define WSB_ERROR_BASE_NETWORK             (WSB_ERROR_BASE -  60)
#define WSB_ERROR_BASE_DRM_PLUGIN          (WSB_ERROR_BASE - 100)
#define WSB_ERROR_BASE_DOWNLOAD_MANAGER    (WSB_ERROR_BASE - 200)
#define WSB_ERROR_BASE_METADATA_HELPER     (WSB_ERROR_BASE - 300)
#define WSB_ERROR_BASE_LICENSE_STORE       (WSB_ERROR_BASE - 400)
#define WSB_ERROR_BASE_MEDIA_SOURCE_SERVER (WSB_ERROR_BASE - 500)

/* network error codes */
#define WSB_ERROR_NETWORK_FAILURE                    (WSB_ERROR_BASE_NETWORK -  0)
#define WSB_ERROR_NETWORK_RESOURCE_NOT_FOUND         (WSB_ERROR_BASE_NETWORK -  1)
#define WSB_ERROR_NETWORK_CONNECTION_REFUSED         (WSB_ERROR_BASE_NETWORK -  2)
#define WSB_ERROR_NETWORK_DISCONNECTED               (WSB_ERROR_BASE_NETWORK -  3)
#define WSB_ERROR_NETWORK_HOST_UNKNOWN               (WSB_ERROR_BASE_NETWORK -  4)
#define WSB_ERROR_NETWORK_PERMISSION_DENIED          (WSB_ERROR_BASE_NETWORK -  5)
#define WSB_ERROR_NETWORK_SERVER_FAILURE             (WSB_ERROR_BASE_NETWORK -  6)
#define WSB_ERROR_NETWORK_PROTOCOL_FAILURE           (WSB_ERROR_BASE_NETWORK -  7)

/* drm error codes */
#define WSB_ERROR_BASE_DRM                 (WSB_ERROR_BASE - 600)
#define WSB_ERROR_DRM_INIT                 (WSB_ERROR_BASE_DRM - 0)
#define WSB_ERROR_DRM_NO_LICENSE           (WSB_ERROR_BASE_DRM - 1)
#define WSB_ERROR_DRM_NO_SILENT_HEADER     (WSB_ERROR_BASE_DRM - 2)
#define WSB_ERROR_DRM_NO_RENEWAL_URL       (WSB_ERROR_BASE_DRM - 3)
#define WSB_ERROR_DRM_ACQUIRE_TOKEN        (WSB_ERROR_BASE_DRM - 4)
#define WSB_ERROR_DRM_PROCESS_TOKEN        (WSB_ERROR_BASE_DRM - 5)
#define WSB_ERROR_DRM_EVAL_LICENSE         (WSB_ERROR_BASE_DRM - 6)
#define WSB_ERROR_DRM_DENY_RIGHTS          (WSB_ERROR_BASE_DRM - 7)
#define WSB_ERROR_DRM_GET_KEY              (WSB_ERROR_BASE_DRM - 8)
#define WSB_ERROR_DRM_UNHANDLED_CALLBACK   (WSB_ERROR_BASE_DRM - 9)
#define WSB_ERROR_DRM_UNHANDLED_OBLIGATION (WSB_ERROR_BASE_DRM - 10)
#define WSB_ERROR_DRM_CLIENT_SYSTEM_ERROR  (WSB_ERROR_BASE_DRM - 11)
#define WSB_ERROR_DRM_INVALID_URL          (WSB_ERROR_BASE_DRM - 12)
#define WSB_ERROR_DRM_SERVER_UNREACHABLE   (WSB_ERROR_BASE_DRM - 13)
#define WSB_ERROR_DRM_DENIED_BY_SERVER     (WSB_ERROR_BASE_DRM - 14)
#define WSB_ERROR_DRM_BAD_SERVER_RESPONSE  (WSB_ERROR_BASE_DRM - 15)
#define WSB_ERROR_DRM_BAD_CONTENT_FORMAT   (WSB_ERROR_BASE_DRM - 16)
#define WSB_ERROR_DRM_LICENSE_UNSUPPORTED  (WSB_ERROR_BASE_DRM - 17)
#define WSB_ERROR_DRM_BAD_LICENSE_FORMAT   (WSB_ERROR_BASE_DRM - 18)
#define WSB_ERROR_DRM_MISSING_CREDENTIALS  (WSB_ERROR_BASE_DRM - 19)
#define WSB_ERROR_DRM_LICENSE_EXPIRED      (WSB_ERROR_BASE_DRM - 20)
#define WSB_ERROR_DRM_PLAY_COUNT_EXCEEDED  (WSB_ERROR_BASE_DRM - 21)
#define WSB_ERROR_DRM_SUBSCRIPTION_EXPIRED (WSB_ERROR_BASE_DRM - 22)
#define WSB_ERROR_DRM_UNKNOWN              (WSB_ERROR_BASE_DRM - 23)
 

/* directshow filter error code base */
#define WSB_ERROR_BASE_DS_FILTER           (WSB_ERROR_BASE - 700)

/* gstreamer element error code base */
#define WSB_ERROR_BASE_GST_ELEMENT         (WSB_ERROR_BASE - 800)

/* DMZ error codes */
#if 0
#define WSB_ERROR_BASE_DMZ                 (WSB_ERROR_BASE - 900)

#define WSB_ERROR_DMZ_NO_DMZ_INFO                  (WSB_ERROR_BASE_DMZ - 0)
#define WSB_ERROR_DMZ_INVALID_TRACK_TYPE           (WSB_ERROR_BASE_DMZ - 1)
#define WSB_ERROR_DMZ_INTERNAL_ERROR               (WSB_ERROR_BASE_DMZ - 2)
#define WSB_ERROR_DMZ_NO_MP4_FILE                  (WSB_ERROR_BASE_DMZ - 3)
#define WSB_ERROR_DMZ_NO_MOVIE                     (WSB_ERROR_BASE_DMZ - 4)
#define WSB_ERROR_DMZ_NO_AUDIO_TRACK               (WSB_ERROR_BASE_DMZ - 5)
#define WSB_ERROR_DMZ_NO_VIDEO_TRACK               (WSB_ERROR_BASE_DMZ - 6)
#define WSB_ERROR_DMZ_MEDIA_SPAN_NOT_FOUND         (WSB_ERROR_BASE_DMZ - 7)
#define WSB_ERROR_DMZ_MEDIA_ENTRY_NOT_FOUND        (WSB_ERROR_BASE_DMZ - 8)
#define WSB_ERROR_DMZ_DIGEST_VERIFICATION_FAILURE  (WSB_ERROR_BASE_DMZ - 9)
#define WSB_ERROR_DMZ_CANNOT_RESOLVE_EXTERNAL_ZONE (WSB_ERROR_BASE_DMZ - 10)
#define WSB_ERROR_DMZ_ZONE_RESOLVER_NOT_FOUND      (WSB_ERROR_BASE_DMZ - 11)
#define WSB_ERROR_DMZ_STORE_OPEN_DB                (WSB_ERROR_BASE_DMZ - 12)
#define WSB_ERROR_DMZ_STORE_ADD_ENTRY              (WSB_ERROR_BASE_DMZ - 13)
#define WSB_ERROR_DMZ_STORE_ENTRY_NOT_FOUND        (WSB_ERROR_BASE_DMZ - 14)
#define WSB_ERROR_DMZ_INVALID_AD_PATH              (WSB_ERROR_BASE_DMZ - 15)
#define WSB_ERROR_DMZ_ZONE_NOT_FOUND               (WSB_ERROR_BASE_DMZ - 16)
#define WSB_ERROR_DMZ_MULTIPLE_ZONES               (WSB_ERROR_BASE_DMZ - 17)
#define WSB_ERROR_DMZ_LICENSE_INVALID              (WSB_ERROR_BASE_DMZ - 18)
#define WSB_ERROR_DMZ_CONTENT_KEY_LENGTH_INVALID   (WSB_ERROR_BASE_DMZ - 19)
#define WSB_ERROR_DMZ_AD_FILE_NOT_FOUND            (WSB_ERROR_BASE_DMZ - 20)
#endif

/* Adaptive Streaming (Smth) error codes */
#define WSB_ERROR_BASE_SMTH                (WSB_ERROR_BASE - 1000)

/* Action Result Info error codes */
#define WSB_ERROR_BASE_ARI                 (WSB_ERROR_BASE - 1100)

/* Http Live Streaming (Hls) error codes */
#define WSB_ERROR_BASE_HLS                 (WSB_ERROR_BASE - 1200) 

/* MPEG-Dynamic Adaptive Streaming over HTTP (Dash) error codes */
#define WSB_ERROR_BASE_DASH                (WSB_ERROR_BASE - 1300) 


/* standard error codes                                  */
/* these are special codes to convey an errno            */
/* the error code is (WSB_ERROR_BASE_ERRNO - errno)      */
/* where errno is the positive integer from errno.h      */
#define WSB_ERROR_BASE_ERRNO               (WSB_ERROR_BASE - 2000)
#define WSB_ERROR_ERRNO(e)                 (WSB_ERROR_BASE_ERRNO - (e))

#endif /* _WSB_RESULTS_H_ */
