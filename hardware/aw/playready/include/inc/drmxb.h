/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_XB_H__
#define __DRM_XB_H__

#include <drmtypes.h>
#include <drmpragmas.h>

ENTER_PK_NAMESPACE;

typedef enum
{
    XB_FORMAT_ID_LENGTH_DWORD = sizeof( DRM_DWORD ),
    XB_FORMAT_ID_LENGTH_QWORD = sizeof(DRM_UINT64),
} XB_FORMAT_ID_LENGTH;

#define XB_HEADER_LENGTH( formatIdLength )      ( ( formatIdLength ) + sizeof( DRM_DWORD ) * 2 )    /* XB headers are 1 DWORD/QWORD long plus 2 DWORDs long (id, version, length) */
#define XB_BASE_OBJECT_LENGTH                   ( sizeof( DRM_WORD ) * 2  + sizeof( DRM_DWORD ) )   /* Each object/container always has type (WORD), flags (WORD), and length (DWORD) */
#define XB_MAXIMUM_OBJECT_DEPTH 6
#define DRM_XB_PARSER_MAX_UNKNOWN_CONTAINER_DEPTH 16

/*
** valid flags for containers and objects
*/
enum XB_OBJECT_FLAGS
{
    XB_FLAGS_NONE                         = 0x0000,
    XB_FLAGS_MUST_UNDERSTAND              = 0x0001,
    XB_FLAGS_CONTAINER                    = 0x0002,
    XB_FLAGS_ALLOW_EXTERNAL_PARSE         = 0x0004,
    XB_FLAGS_DO_NOT_SERIALIZE             = 0x0008,
    XB_FLAGS_CONTAINER_HAS_RESERVE_BUFFER = 0x0010, /*
                                                    ** This indicates the container may have reserved memory after the last object with a known byte fill pattern.
                                                    ** This behavior is used for certificate parsing when a data buffer is reserved for adding extended data at a
                                                    ** later time.
                                                    */
};

enum XB_OBJECT_TYPE
{
    XB_OBJECT_TYPE_EXTENDED_HEADER                                  = DRM_MAX_UNSIGNED_TYPE(DRM_WORD)-4, /* 0xFFFB = Extended header information */
    XB_OBJECT_TYPE_ROOT                                             = DRM_MAX_UNSIGNED_TYPE(DRM_WORD)-3, /* 0xFFFC = Format root object - this value is used to enable a RawData weak reference object to be generated during parsing */
    XB_OBJECT_TYPE_UNKNOWN                                          = DRM_MAX_UNSIGNED_TYPE(DRM_WORD)-2, /* 0xFFFD = Unknown object type */
    XB_OBJECT_TYPE_UNKNOWN_CONTAINER                                = DRM_MAX_UNSIGNED_TYPE(DRM_WORD)-1, /* 0xFFFE = Unknown container type */
    XB_OBJECT_TYPE_INVALID                                          = DRM_MAX_UNSIGNED_TYPE(DRM_WORD)  , /* 0xFFFF = Invalid */
    XB_OBJECT_GLOBAL_HEADER                                         = 0                                  /* All formats based on the XB system should start their objects from XB_OBJECT_GLOBAL_HEADER+1 */
};

/* Base struct that all XB structs can be cast to */
typedef struct __tagDRM_XB_EMPTY
{
    DRM_BOOL  fValid;
} DRM_XB_EMPTY;

typedef struct __tagDRM_XB_FORMAT_VERSION
{
    DRM_BOOL  fValid;
    DRM_DWORD dwVersion;
} DRM_XB_FORMAT_VERSION;

/* Base struct for all types that allow duplicates as they are parsed into a linked list */
typedef struct __tagDRM_XB_BASELIST
{
    DRM_BOOL fValid;
DRM_OBFUS_PTR_TOP
    struct __tagDRM_XB_BASELIST *pNext;
DRM_OBFUS_PTR_BTM

} DRM_XB_BASELIST;

/*
** A flat array of GUIDs.  When represented in the binary format it is serialized as
** a DWORD followed by cGUIDs consecutive 16 byte GUID values.
*/
typedef struct __tagDRM_XB_GUIDLIST
{
    DRM_BOOL   fValid;
    DRM_DWORD  cGUIDs;
DRM_OBFUS_PTR_TOP
    DRM_BYTE  *pguidBuffer;
DRM_OBFUS_PTR_BTM
    DRM_DWORD  iGuids;
} DRM_XB_GUIDLIST;


/*
** A flat array of WORDs.  When represented in the binary format it is serialized as
** a DWORD followed by cWORDs consecutive WORD values.
*/
typedef struct __tagDRM_XB_WORDLIST
{
    DRM_BOOL   fValid;
    DRM_DWORD  cWORDs;
DRM_OBFUS_PTR_TOP
    DRM_BYTE  *pwordBuffer;
DRM_OBFUS_PTR_BTM
    DRM_DWORD  iWords;
DRM_OBFUS_FILL_BYTES(4)
} DRM_XB_WORDLIST;

/*
** A flat array of DWORDs.  When represented in the binary format it is serialized as
** a DWORD followed by cDWORDs consecutive DWORD values.
*/
typedef struct __tagDRM_XB_DWORDLIST
{
    DRM_BOOL   fValid;
    DRM_DWORD  cDWORDs;
DRM_OBFUS_PTR_TOP
    DRM_BYTE  *pdwordBuffer;
DRM_OBFUS_PTR_BTM
    DRM_DWORD  iDwords;
DRM_OBFUS_FILL_BYTES(4)
} DRM_XB_DWORDLIST;

#define XB_DWORD_LIST_TO_PDWORD( dwordlist ) ( ( (dwordlist).cDWORDs != 0) ? ( (DRM_DWORD*)( (dwordlist).pdwordBuffer + (dwordlist).iDwords) ) : NULL )

/*
** A flat array of QWORDs.  When represented in the binary format it is serialized as
** a DWORD followed by cQWORDs consecutive QWORD values.
*/
typedef struct __tagDRM_XB_QWORDLIST
{
    DRM_BOOL   fValid;
    DRM_DWORD  cQWORDs;
DRM_OBFUS_PTR_TOP
    DRM_BYTE  *pqwordBuffer;
DRM_OBFUS_PTR_BTM
    DRM_DWORD  iQwords;
DRM_OBFUS_FILL_BYTES(4)
} DRM_XB_QWORDLIST;

/*
**  An array of byte.  When serialized it may be serialized as
**  a DWORD followed by cbData consecutive bytes or just cbData bytes if the
**  defining format specifies the data as a fixed size.
*/
typedef struct __tagDRM_XB_BYTEARRAY
{
    DRM_BOOL   fValid;
    DRM_DWORD  cbData;
DRM_OBFUS_PTR_TOP
    DRM_BYTE  *pbDataBuffer;
DRM_OBFUS_PTR_BTM
    DRM_DWORD  iData;
DRM_OBFUS_FILL_BYTES(4)
} DRM_XB_BYTEARRAY;

#define XBBA_TO_PB( xbba ) ( ( (xbba).cbData != 0 ) ? ( (xbba).pbDataBuffer + (xbba).iData ) : NULL )
#define XBBA_TO_CB( xbba ) ( (xbba).cbData )

typedef struct __tagDRM_XB_OBJECT
{
    DRM_BOOL                            fValid;
    DRM_WORD                            wType;
    DRM_WORD                            wFlags;
DRM_OBFUS_PTR_TOP
    DRM_BYTE                           *pbBuffer;
DRM_OBFUS_PTR_BTM
    DRM_DWORD                           ibData;
    DRM_DWORD                           cbData;
    struct __tagDRM_XB_OBJECT *pNext; /* Linked list */
} DRM_XB_OBJECT;

typedef struct __tagDRM_XB_UNKNOWN_OBJECT
{
    DRM_BOOL                            fValid;
    DRM_WORD                            wType;
    DRM_WORD                            wFlags;
DRM_OBFUS_PTR_TOP
    DRM_BYTE                           *pbBuffer;
DRM_OBFUS_PTR_BTM
    DRM_DWORD                           ibData;
    DRM_DWORD                           cbData;
DRM_OBFUS_PTR_TOP
    struct __tagDRM_XB_UNKNOWN_OBJECT  *pNext; /* Linked list */
DRM_OBFUS_PTR_BTM
} DRM_XB_UNKNOWN_OBJECT;

typedef struct __tagDRM_XB_UNKNOWN_CONTAINER
{
    DRM_BOOL                             fValid;
    DRM_WORD                             wType;
    DRM_WORD                             wFlags;
DRM_OBFUS_PTR_TOP
    DRM_XB_UNKNOWN_OBJECT                *pObject;
DRM_OBFUS_PTR_BTM
DRM_OBFUS_PTR_TOP
    struct __tagDRM_XB_UNKNOWN_CONTAINER *pNext; /* Linked list */
DRM_OBFUS_PTR_BTM
DRM_OBFUS_PTR_TOP
    struct __tagDRM_XB_UNKNOWN_CONTAINER *pUnkChildcontainer; /* Linked list */
DRM_OBFUS_PTR_BTM
} DRM_XB_UNKNOWN_CONTAINER;

/*
**  Each element in an object can be one of these base types
*/
typedef enum
{
    XB_ELEMENT_TYPE_INVALID,
    XB_ELEMENT_TYPE_WORD,
    XB_ELEMENT_TYPE_DWORD,
    XB_ELEMENT_TYPE_QWORD,
    XB_ELEMENT_TYPE_GUID,
    XB_ELEMENT_TYPE_GUIDLIST,
    XB_ELEMENT_TYPE_WORDLIST,
    XB_ELEMENT_TYPE_DWORDLIST,
    XB_ELEMENT_TYPE_STRUCTLIST,
    XB_ELEMENT_TYPE_BYTEARRAY,
    XB_ELEMENT_TYPE_QWORDLIST,
    XB_ELEMENT_TYPE_EMPTY,
    XB_ELEMENT_TYPE_UNKNOWN_OBJECT,
    XB_ELEMENT_TYPE_UNKNOWN_CONTAINER,
    XB_ELEMENT_TYPE_ELEMENT_COUNT,
} XB_ELEMENT_BASE_TYPE;

typedef enum _XB_SIZE_TYPE
{
    XB_SIZE_IS_ABSOLUTE,                            /* Absolute means that the size value is an exact value as specified by the accompanying DWORD; absolute == fixed size */
    XB_SIZE_IS_RELATIVE_WORD,                       /* Relative means that there is an adjoining WORD in the binary format that specifies the size; relative == dynamic size */
    XB_SIZE_IS_RELATIVE_DWORD,                      /* Relative means that there is an adjoining DWORD in the binary format that specifies the size */
    XB_SIZE_IS_RELATIVE_DWORD_IN_BITS,              /* Relative means that there is an adjoining DWORD in the binary format that specifies the size (in BITS not BYTES)*/
    XB_SIZE_IS_WORD_IN_BITS_ANOTHER_STRUCT_MEMBER,  /* Relative means that there is an adjoining WORD in the binary format that specifies the size (in BITS not BYTES)*/
    XB_SIZE_IS_REMAINING_DATA_IN_OBJECT,            /* Remaining in data object means that size of the byte array is the remaining bytes in the object. WARNING!!!! THIS MUST BE THE LAST ELEMENT IN THE OBJECT! */
    XB_SIZE_IS_DATA_IN_OBJECT_FOR_WEAKREF,          /* The entire size of the data object.  The XBinary byte array will be weak reference to the raw bytes for this object.
                                                     * The serialization data index should not change when parsing this object. */
    XB_SIZE_IS_WORD_IN_ANOTHER_STRUCT_MEMBER,       /* Means that there is an adjoining WORD in the binary format that specifies the size (in BYTES).  For this type the DRM_XB_SIZE.wOffset element is used
                                                     * as the offset for the previous value and the wMinSize field contains the sturcture size.  For DWORD lists, the wMinSize and wMaxSize are used to determine
                                                     * the min/max expected element count.  The parser/builder will fail if the count falls outside this range.  A range of {0,0} will be ignored. */
    XB_SIZE_IS_DWORD_IN_ANOTHER_STRUCT_MEMBER,      /* Means that there is an adjoining DWORD in the binary format that specifies the size (in BYTES).  For this type the DRM_XB_SIZE.wOffset element is used
                                                     * as the offset for the previous value and the wMinSize field contains the sturcture size. */
    XB_SIZE_IS_ELEMENT_COUNT,                       /* This is NOT used in reference to a XBinary byte array.  This value is used by a container to indicate that the cEntries object in the container should
                                                     * contain a count of specific elements.  The element type is indicated by the DRM_XB_SIZE.wMinSize member. */
    XB_SIZE_IS_CONTAINER_FILL_PATTERN,              /* Means that the variable is not parsed and is only used on containers to indicate the byte fill pattern that may complete the end of the container length */
    XB_SIZE_IS_MAX_MIN_VALUES,                      /* Means the XBSize parameter contains the maximum and minimum values for the associated XBinary element. */
    XB_SIZE_IS_IGNORED = 0xBAAD                     /* Relative means that there is an adjoining DWORD in the binary format that specifies the size */
} XB_SIZE_TYPE;

typedef struct __tagDRM_XB_SIZE
{
    XB_SIZE_TYPE eSizeType;
    DRM_WORD     wMinSize;
    DRM_WORD     wMaxSize;  /* Max size is ignored if wMaxSize is zero */
    DRM_WORD     wOffset;   /* Offset into the parent structure where the size/count field is located */
    DRM_WORD     wPadding;
} DRM_XB_SIZE;

/* Unless you are using a BYTEARRAY use XB_SIZE_IGNORED for the xbSize field  */
#define XB_SIZE_IGNORED                   { XB_SIZE_IS_IGNORED       , 0        , 0        , 0, 0xBAAD          }
#define XB_SIZE_MAX_MIN(minSize, maxSize) { XB_SIZE_IS_MAX_MIN_VALUES, (minSize), (maxSize), 0, XB_PADDING_NONE }

#define XB_PADDING_NONE 0

#define XB_SIZE_ABSOLUTE(x)                             XB_SIZE_ABSOLUTE_WITH_PADDING(x, XB_PADDING_NONE)
#define XB_SIZE_RELATIVE_DWORD(minSize,maxSize)         XB_SIZE_RELATIVE_DWORD_WITH_PADDING(minSize, maxSize, XB_PADDING_NONE)
#define XB_SIZE_RELATIVE_WORD(minSize,maxSize)          XB_SIZE_RELATIVE_WORD_WITH_PADDING(minSize, maxSize, XB_PADDING_NONE)
/*
** For STRUCTLIST objects we use the DRM_XB_SIZE object to represent the size of the struct and the offset of the element count value in a previous structure.
** The struct size is stored in the DRM_XB_SIZE.wSize member and the offset to the element count (in a previous structure) is in the DRM_XB_SIZE.dwPadding
** member.
*/
#define XB_COUNT_WORD_FROM_STRUCT_MEMBER(minCount, maxCount, struc, member) { XB_SIZE_IS_WORD_IN_ANOTHER_STRUCT_MEMBER     , (minCount), (maxCount), DRM_OFFSET_OF(struc,member), XB_PADDING_NONE }
#define XB_COUNT_DWORD_FROM_STRUCT_MEMBER(minCount, maxCount, struc, member){ XB_SIZE_IS_DWORD_IN_ANOTHER_STRUCT_MEMBER    , (minCount), (maxCount), DRM_OFFSET_OF(struc,member), XB_PADDING_NONE }
#define XB_SIZE_IS_COUNT_OF_ELEMENTS(reftype)                               { XB_SIZE_IS_ELEMENT_COUNT                     , (reftype) , (reftype) , 0                          , XB_PADDING_NONE }
#define XB_SIZE_IN_OBJECT_FOR_WEAKREF                                       { XB_SIZE_IS_DATA_IN_OBJECT_FOR_WEAKREF        , 0         , 0         , 0                          , XB_PADDING_NONE }
#define XB_SIZE_REMAINING_DATA_IN_OBJECT(minSize,maxSize)                   { XB_SIZE_IS_REMAINING_DATA_IN_OBJECT          , (minSize) , (maxSize) , 0                          , XB_PADDING_NONE }
#define XB_SIZE_FROM_STRUCT_MEMBER_IN_BITS(struc, member, minSize, maxSize) { XB_SIZE_IS_WORD_IN_BITS_ANOTHER_STRUCT_MEMBER, (minSize) , (maxSize) , DRM_OFFSET_OF(struc,member), XB_PADDING_NONE }
#define XB_SIZE_CONTAINER_FILL_PATTERN(x)                                   { XB_SIZE_IS_CONTAINER_FILL_PATTERN            , 0         , 0         , (x)                        , XB_PADDING_NONE }

#define XB_SIZE_ABSOLUTE_WITH_PADDING(x, padding)                                                { XB_SIZE_IS_ABSOLUTE                          , (x)      , (x)      , 0                          , (padding) }
#define XB_SIZE_RELATIVE_DWORD_IN_BITS_WITH_PADDING(minSize, maxSize, padding)                   { XB_SIZE_IS_RELATIVE_DWORD_IN_BITS            , (minSize), (maxSize), 0                          , (padding) }
#define XB_SIZE_RELATIVE_DWORD_WITH_PADDING(minSize, maxSize, padding)                           { XB_SIZE_IS_RELATIVE_DWORD                    , (minSize), (maxSize), 0                          , (padding) }
#define XB_SIZE_RELATIVE_WORD_WITH_PADDING(minSize, maxSize, padding)                            { XB_SIZE_IS_RELATIVE_WORD                     , (minSize), (maxSize), 0                          , (padding) }
#define XB_SIZE_REMAINING_DATA_IN_OBJECT_WITH_PADDING(member, minSize, maxSize, padding)         { XB_SIZE_IS_REMAINING_DATA_IN_OBJECT          , (minSize), (maxSize), 0                          , (padding) }
#define XB_SIZE_IN_OBJECT_FOR_WEAKREF_WITH_PADDING(member, minSize, maxSize, padding)            { XB_SIZE_IS_DATA_IN_OBJECT_FOR_WEAKREF        , (minSize), (maxSize), 0                          , (padding) }
#define XB_SIZE_FROM_STRUCT_MEMBER_IN_BITS_WITH_PADDING(struc, member, minSize, maxSize, padding){ XB_SIZE_IS_WORD_IN_BITS_ANOTHER_STRUCT_MEMBER, (minSize), (maxSize), DRM_OFFSET_OF(struc,member), (padding) }

/*
** Each element in an object is described be the following structure.
** Constructing an array of such objects will describe an entire object
*/
typedef struct __tagDRM_XB_ELEMENT_DESCRIPTION
{
    XB_ELEMENT_BASE_TYPE eElementType;              /* Type of element */
    DRM_WORD             wOffsetInCurrentStruct;    /* Where in the containing struct the element should be serialized to/from */
    DRM_XB_SIZE          xbSize;                    /* If the object is a BYTEARRAY this is the size relative or absolute */
                                                    /* If the object is a STRUCTLIST this is the size of the C-struct to allocate for each entry in the list */
    const DRM_VOID  *pElementDescription; /* Only used if the object is a STRUCTLIST */
    DRM_DWORD        cElementDescription; /* Only used if the object is a STRUCTLIST */
} DRM_XB_ELEMENT_DESCRIPTION;

typedef enum __tagDRM_XB_ALIGNMENT
{
    XB_ALIGN_1_BYTE  =  1,
    XB_ALIGN_8_BYTE  =  8,
    XB_ALIGN_16_BYTE = 16,
} DRM_XB_ALIGNMENT;

/*
**  Each entry(container or object) is represented by the following structure.
*/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_POOR_DATAALIGNMENT, "Structure sequence maintained to avoid manually modifying large structure arrays.")
typedef struct __tagDRM_XB_ENTRY_DESCRIPTION
{
    DRM_WORD  wType;                                /* Type identifier -- should be the same value as the entries order in the entry array in the format description */
    DRM_WORD  wFlags;                               /* Flags for this type */
    DRM_WORD  wParent;                              /* The container that can hold this object/container */
    DRM_BOOL  fDuplicateAllowed;                    /* Are multiple objects allowed */
    DRM_BOOL  fOptional;                            /* Is this a required object for the binary format to be valid? */
    DRM_WORD  wOffsetInCurrentStruct;               /* Where in the containing structure should the data be serialized from/to */
    DRM_DWORD dwStructureSize;                      /* Size of the containing structure.  This is necessary when duplicates are allocated in a linked list */
    DRM_WORD  wBuilderSortOrder;                    /* Order in which elements should be serialized during building.  Has no affect during parsing */
    const DRM_VOID *pContainerOrObjectDescriptions; /* This pointer is either an array of DRM_XB_ELEMENT_DESCRIPTION if the entry is an object or NULL if this is a container . */
    DRM_WORD  cContainerOrObjectDescriptions;
} DRM_XB_ENTRY_DESCRIPTION;
PREFAST_POP

/*
** Indicates the size element is not in the extended header
*/
#define DRM_XB_EXTENDED_HEADER_SIZE_OFFSET_INVALID ((DRM_WORD)0xFFFF)

typedef struct __tagDRM_XB_HEADER_DESCRIPTION
{
    XB_FORMAT_ID_LENGTH             eFormatIdLength;
    DRM_UINT64                      qwFormatIdentifier;             /* 4 or 8 byte string identifier.  Use XB_DEFINE_DWORD_FORMAT_ID or XB_DEFINE_QWORD_FORMAT_ID to create this value */
    DRM_DWORD                       dwFormatVersion;                /* Version supported by this description */
    DRM_XB_ALIGNMENT                eAlign;                         /* Byte alignment identifier.  Must be a non-zero, 4^x value (e.g. 1, 4, 8, 16, etc). */
    DRM_WORD                        wOffsetOfSizeInHeaderStruct;    /* Where in the containing structure should the data be serialized from/to */
    const DRM_XB_ENTRY_DESCRIPTION *pEntryDescription;              /* The header can have optional extra data -- but only 1 */
} DRM_XB_HEADER_DESCRIPTION;

/*
**  Single descrption point for a format.  This format is composed of arrays of other objects as noted below.
*/
typedef struct __tagDRM_XB_FORMAT_DESCRIPTION
{
    const DRM_XB_HEADER_DESCRIPTION *pHeaderDescription;
    const DRM_XB_ENTRY_DESCRIPTION  *pEntryDescriptions; /* An array of entry descriptions.  Each entry description is either a container or an object */
    DRM_WORD                         cEntryDescriptions;
    DRM_WORD                         cContainerDepth;
} DRM_XB_FORMAT_DESCRIPTION;


#define XB_DEFINE_FOUR_BYTE_FORMAT_ID( ch0, ch1, ch2, ch3 )                             \
        ( (DRM_DWORD)(DRM_BYTE)(ch3)         | ( (DRM_DWORD)(DRM_BYTE)(ch2) << 8 ) |    \
        ( (DRM_DWORD)(DRM_BYTE)(ch1) << 16 ) | ( (DRM_DWORD)(DRM_BYTE)(ch0) << 24 ) )

#define XB_DEFINE_DWORD_FORMAT_ID( ch0, ch1, ch2, ch3 )                                 \
        DRM_UI64LITERAL( 0, XB_DEFINE_FOUR_BYTE_FORMAT_ID( ch0, ch1, ch2, ch3 ) )

#define XB_DEFINE_QWORD_FORMAT_ID( ch0, ch1, ch2, ch3, ch4, ch5, ch6, ch7 )             \
        DRM_UI64LITERAL( XB_DEFINE_FOUR_BYTE_FORMAT_ID( ch0, ch1, ch2, ch3 ), XB_DEFINE_FOUR_BYTE_FORMAT_ID( ch4, ch5, ch6, ch7 ) )

#define XB_NULL_DESCRIPTOR NULL, 0
#define XB_USE_DESCRIPTOR( descriptor ) (DRM_VOID*)(descriptor), DRM_NO_OF( (descriptor) )

#define XB_DECLARE_OFFSET_AND_SIZE_DEREF(struc, member) DRM_OFFSET_OF(struc,member), DRM_SIZEOF_MEMBER_DEREF(struc,member)
#define XB_DECLARE_OFFSET_AND_SIZE( struc, member ) DRM_OFFSET_OF(struc,member), DRM_SIZEOF_MEMBER(struc,member)

/*
** Internal helper functions
*/
DRM_NO_INLINE DRM_WORD DRM_CALL _XB_MapObjectTypeToEntryDescriptionIndex(
    __in                      const DRM_XB_FORMAT_DESCRIPTION  *f_pFormat,
    __in                            DRM_WORD                    f_wType ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_BOOL DRM_CALL _XB_IsKnownObjectType(
    __in                      const DRM_XB_FORMAT_DESCRIPTION  *f_pFormat,
    __in                            DRM_WORD                    f_wType ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_BOOL DRM_CALL _XB_IsObjectTypeAChildOf(
    __in                      const DRM_XB_FORMAT_DESCRIPTION  *f_pFormat,
    __in                            DRM_WORD                    f_wChildType,
    __in                            DRM_WORD                    f_wParentType,
    __out_opt                       DRM_XB_ENTRY_DESCRIPTION  **f_ppEntry ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_RESULT DRM_CALL _DRM_XB_AlignData(
    __in                            DRM_XB_ALIGNMENT            f_eAlign,
    __in                            DRM_DWORD                   f_cbBuffer,
    __inout_ecount( 1 )             DRM_DWORD                  *f_piBuffer,
    __out_opt                       DRM_DWORD                  *f_pcbPadding ) DRM_NO_INLINE_ATTRIBUTE;


EXIT_PK_NAMESPACE;

#endif  /* __DRM_XB_H__ */

