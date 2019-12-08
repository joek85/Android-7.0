#ifndef _SBC_H_
#define _SBC_H_

/* defines */
#define SBC_SUCCESS            0
#define SBC_NOT_ENOUGH_DATA   -1
#define SBC_BUFFER_TOO_SMALL  -2
#define SBC_NO_SYNCBYTE       -3
#define SBC_CHECKSUM_ERROR    -4

#define SBC_SYNCBYTE           0x9c
#define SBC_MAX_SUBBANDS       8
#define SBC_MAX_CHANNELS       2
#define SBC_TERMINATOR        -1

/* structs, typedefs */
#define SBC_FREQU16000   0x0
#define SBC_FREQU32000   0x1
#define SBC_FREQU44100   0x2
#define SBC_FREQU48000   0x3
typedef unsigned short TSBCSamplingFrequency;

#define SBC_BLOCKS4    0x0
#define SBC_BLOCKS8    0x1
#define SBC_BLOCKS12   0x2
#define SBC_BLOCKS16   0x3
typedef unsigned short TSBCBlockNumber;

#define SBC_MODE_MONO     0x0
#define SBC_MODE_DUAL     0x1
#define SBC_MODE_STEREO   0x2
#define SBC_MODE_JOINT    0x3
typedef unsigned short TSBCChannelMode;

#define SBC_ALLOCLOUDNESS  0x0
#define SBC_ALLOCSNR       0x1
typedef unsigned short TSBCAllocMethod;

#define SBC_SUBBANDS4  0x0
#define SBC_SUBBANDS8  0x1
typedef unsigned short TSBCSubbandNumber;

typedef struct tagSBCParams
{
	TSBCSamplingFrequency samplingFrequency;
	TSBCBlockNumber blockNumber;
	TSBCChannelMode channelMode;
	TSBCAllocMethod allocMethod;
	TSBCSubbandNumber subbandNumber;
	unsigned char bitpool;
} TSBCParams;

typedef struct taglist TList;

struct taglist
 {
  TList  *pNext;          /* next/first element    */
  TList  *pPrevious;      /* previous/last element */
 };

typedef TList * PList;
#define LIST_IS_EMPTY(pList)    ((pList)->pNext == pList)
#define LIST_QUEUE_OUT(pList)   (pList)->pNext; listRemove((pList)->pNext)
 
void listInit  ( PList pList );
void listInsert( PList pElement, PList pDestinationList );
void listRemove( PList pElement );
void listMove  ( PList pElement, PList pDestinationList );
int  listElementCount( PList pList );

 
/* public sbc interface */
void sbcInitEncoder(void); /* initializes internal buffers */
int sbcEncode(unsigned char* inputBuffer, int inputSize, TSBCParams* params, unsigned char* outputBuffer, int* pOutputSize);
/* returns number of processed input bytes; *pOutputSize contains number of output bytes */
/* or required output buffer size */

void sbcInitDecoder(void); /* initializes internal buffers */
int sbcDecode(unsigned char* pInputBuffer, int inputSize, unsigned char* pOutputBuffer, int* pOutputSize);
/* returns number of processed input bytes; *pOutputSize contains number of output bytes */
/* or required output buffer size */
int sbcGetParams(unsigned char* pInputBuffer, int inputSize, TSBCParams* pParams);
/* fill TSBCParams with data or first SBC frame in pInputBuffer */

#endif
