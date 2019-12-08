/******************************************************
* Project      : SBC
* FileName     : $RCSfile: sbcenc.c,v $
* Created      : April 2005
* Version      : $Revision: 1.10 $
* Last access  : $Date: 2005/10/27 06:31:28 $
* Archive      : $Source: /var/lib/cvs/sw/src/codec/sbc/sbcenc.c,v $
* Copyright    : (C) Stollmann E+V GmbH 2005
* Author       : Lutz Vogelsang (lv@stollmann.de)
* Compiler     : ANSI C compatible
* Environment  : Windows NT 2000 user mode
* Contents     : implementation of SBC encoder
* Note         : 
* History      : 
*              : $Log: sbcenc.c,v $
*              : Revision 1.10  2005/10/27 06:31:28  lv
*              : corrected sample buffer size
*              :
*              : Revision 1.9  2005/10/27 06:06:34  lv
*              : fixed high level distortion problems
*              : various optimizations
*              :
*              : Revision 1.8  2005/10/26 08:06:40  jz
*              : moved constant data to "const" memory
*              :
*              : Revision 1.7  2005/07/07 08:37:34  jz
*              : Cosmetics
*              :
*              : Revision 1.6  2005/06/01 06:00:28  lv
*              : integrated internal headers
*              : fixed joint decoding bug
*              :
*              : Revision 1.5  2005/04/27 07:48:41  lv
*              : tiny optimizations
*              :
*              : Revision 1.4  2005/04/26 08:37:11  lv
*              : formatting
*              :
*              : Revision 1.3  2005/04/26 08:20:47  lv
*              : added 4 subband implementation, crc creation,
*              : joint mode
*              :
*              : Revision 1.2  2005/04/21 12:07:14  lv
*              : 4 subband encoding implemented
*              :
*              : Revision 1.1  2005/04/21 09:52:54  lv
*              : first checkin; 4 subbands not yet functional
*              :
*******************************************************
* sbcenc.c
******************************************************/

#include <flags.h>
#if 1

#include <string.h>
#include <stdio.h>
#include <sbc.h>



/* arm compiler */
#ifdef __arm
# define INLINE __inline
# define VALUE_IN_REGS __value_in_regs
# define PURE
#else
# define INLINE
# define VALUE_IN_REGS
# define PURE
#endif

/* macros and types */
#if !defined min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

#define limit(a) \
  if (a > 32767)\
  a = 32767;\
  if (a < -32768)\
  a = -32768;

/* sbc defines */
#define GUARD_QUANT          16
#define GUARD_QUANT_HALFBIT  32768   /* must equal (1 << (GUARD_QUANT - 1)) */ 
#define GUARD_PAR            17
#define GUARD_PAR_HALFBIT    65536   /* must equal (1 << (GUARD_QUANT - 1)) */ 
#define GUARD_SUB            15
#define GUARD_SUB_HALFBIT    16384   /* must equal (1 << (GUARD_QUANT - 1)) */ 

/*#define F_SBC_ENC_USE_FLOAT*/
/* forces floating point arithmetic for debugging purposes */

#if defined F_SBC_ENC_USE_FLOAT
#if defined F_SBC_ENC_REUSE_INPUT_BUFFER
#undef F_SBC_ENC_REUSE_INPUT_BUFFER
#endif
#endif


/* structs, typedefs */
typedef struct tagSBCData  
{
  unsigned channel_mode;
  unsigned allocation_method;
  unsigned nrof_subbands;
  unsigned nrof_blocks;
  unsigned nrof_channels;
  unsigned sampling_frequency; /* 0..3 */
  int bitpool;
} TSBCData;

typedef struct tagSampleData
{
  int bits;
  int scale_factor;
  int calc1;
  int calc2;
#if defined F_SBC_DEBUG_FILE
  int sb_sample;
  unsigned qs;
#endif
} TSampleData;

/* global variables */
static TSBCData SBCData;
static TSampleData sampleData[SBC_MAX_CHANNELS + 1][SBC_MAX_SUBBANDS]; /* one additional line for terminator  */
static TSampleData jointSampleData[SBC_MAX_CHANNELS + 1][SBC_MAX_SUBBANDS];
static unsigned x[SBC_MAX_CHANNELS][(SBC_MAX_SUBBANDS + 1) * 11];
static unsigned y[SBC_MAX_CHANNELS][SBC_MAX_SUBBANDS * 2];

#if !defined F_SBC_ENC_REUSE_INPUT_BUFFER
static int samples[256];
#endif

#if defined F_SBC_ENC_USE_FLOAT
static int X[2][80] = {0};
static double Z[2][80] = {0.0};
static double Y[2][16] = {0.0};
static double S[2][16] = {0.0};
#endif

/* global constants */
static const unsigned blockNumbers[4] = {4, 8, 12, 16};
static const unsigned subbandNumbers[2] = {4, 8};
static const char matrixingTable[11] = {18, 27, 36, 45, 54, 63, 72, 81, 0, 9, 18};
static const int offset4[4][4] = {{-1,0,0,0},{-2,0,0,1},{-2,0,0,1},{-2,0,0,1}};
static const int offset8[4][8] = {{-2,0,0,0,0,0,0,1},{-3,0,0,0,0,0,1,2},{-4,0,0,0,0,0,1,2},{-4,0,0,0,0,0,1,2}};

static const int c4[40] = 
{
  /* ik iteration 2 * 5 groups @ 4 */
  0, 70, 196, 358, 1430, 2679, 3785, 4220,
  17772, 25557, 32327, 36940, -17772, -10177, -3778, 804,
  -1430, -401, 245, 510, 503, 510, 245, -401,
  3392, 804, -3778, -10177, 38576, 36940, 32327, 25557,
  3392, 4220, 3785, 2679, 503, 358, 196, 70,
};

static const int c8[80] =
{
  /* ik iteration 2 * 5 groups @ 8 */
  0, 21, 45, 73, 108, 149, 194, 234,
  742, 1052, 1371, 1671, 1921, 2085, 2126, 2008,
  8913, 10877, 12789, 14575, 16157, 17467, 18449, 19057,
  -8913, -6971, -5122, -3422, -1919, -644, 383, 1161,
  -742, -458, -216, -23, 118, 212, 261, 276,
  264, 276, 261, 212, 118, -23, -216, -458,
  1696, 1161, 383, -644, -1919, -3422, -5122, -6971,
  19262, 19057, 18449, 17467, 16157, 14575, 12789, 10877,
  1696, 2008, 2126, 2085, 1921, 1671, 1371, 1052,
  264, 234, 194, 149, 108, 73, 45, 21,
};

static const int mik4[4][8] =
{
  23170, 30274, 32768, 30274, 23170, 12540, 0, -12540,
  -23170, 12540, 32768, 12540, -23170, -30274, 0, 30274,
  -23170, -12540, 32768, -12540, -23170, 30274, 0, -30274,
  23170, -30274, 32768, -30274, 23170, -12540, 0, 12540,
};

static const int mik8[8][16] =
{
  23170, 27246, 30274, 32138, 32768, 32138, 30274, 27246, 23170, 18205, 12540, 6393, 0, -6393, -12540, -18205,
  -23170, -6393, 12540, 27246, 32768, 27246, 12540, -6393, -23170, -32138, -30274, -18205, 0, 18205, 30274, 32138,
  -23170, -32138, -12540, 18205, 32768, 18205, -12540, -32138, -23170, 6393, 30274, 27246, 0, -27246, -30274, -6393,
  23170, -18205, -30274, 6393, 32768, 6393, -30274, -18205, 23170, 27246, -12540, -32138, 0, 32138, 12540, -27246,
  23170, 18205, -30274, -6393, 32768, -6393, -30274, 18205, 23170, -27246, -12540, 32138, 0, -32138, 12540, 27246,
  -23170, 32138, -12540, -18205, 32768, -18205, -12540, 32138, -23170, -6393, 30274, -27246, 0, 27246, -30274, 6393,
  -23170, 6393, 12540, -27246, 32768, -27246, 12540, 6393, -23170, 32138, -30274, 18205, 0, -18205, 30274, -32138,
  23170, -27246, 30274, -32138, 32768, -32138, 30274, -27246, 23170, -18205, 12540, -6393, 0, 6393, -12540, 18205,
};

static const unsigned char crcTable[256] = 
{
  0x00, 0x1D, 0x3A, 0x27, 0x74, 0x69, 0x4E, 0x53,
  0xE8, 0xF5, 0xD2, 0xCF, 0x9C, 0x81, 0xA6, 0xBB,
  0xCD, 0xD0, 0xF7, 0xEA, 0xB9, 0xA4, 0x83, 0x9E,
  0x25, 0x38, 0x1F, 0x02, 0x51, 0x4C, 0x6B, 0x76,
  0x87, 0x9A, 0xBD, 0xA0, 0xF3, 0xEE, 0xC9, 0xD4,
  0x6F, 0x72, 0x55, 0x48, 0x1B, 0x06, 0x21, 0x3C,
  0x4A, 0x57, 0x70, 0x6D, 0x3E, 0x23, 0x04, 0x19,
  0xA2, 0xBF, 0x98, 0x85, 0xD6, 0xCB, 0xEC, 0xF1,
  0x13, 0x0E, 0x29, 0x34, 0x67, 0x7A, 0x5D, 0x40,
  0xFB, 0xE6, 0xC1, 0xDC, 0x8F, 0x92, 0xB5, 0xA8,
  0xDE, 0xC3, 0xE4, 0xF9, 0xAA, 0xB7, 0x90, 0x8D,
  0x36, 0x2B, 0x0C, 0x11, 0x42, 0x5F, 0x78, 0x65,
  0x94, 0x89, 0xAE, 0xB3, 0xE0, 0xFD, 0xDA, 0xC7,
  0x7C, 0x61, 0x46, 0x5B, 0x08, 0x15, 0x32, 0x2F,
  0x59, 0x44, 0x63, 0x7E, 0x2D, 0x30, 0x17, 0x0A,
  0xB1, 0xAC, 0x8B, 0x96, 0xC5, 0xD8, 0xFF, 0xE2,
  0x26, 0x3B, 0x1C, 0x01, 0x52, 0x4F, 0x68, 0x75,
  0xCE, 0xD3, 0xF4, 0xE9, 0xBA, 0xA7, 0x80, 0x9D,
  0xEB, 0xF6, 0xD1, 0xCC, 0x9F, 0x82, 0xA5, 0xB8,
  0x03, 0x1E, 0x39, 0x24, 0x77, 0x6A, 0x4D, 0x50,
  0xA1, 0xBC, 0x9B, 0x86, 0xD5, 0xC8, 0xEF, 0xF2,
  0x49, 0x54, 0x73, 0x6E, 0x3D, 0x20, 0x07, 0x1A,
  0x6C, 0x71, 0x56, 0x4B, 0x18, 0x05, 0x22, 0x3F,
  0x84, 0x99, 0xBE, 0xA3, 0xF0, 0xED, 0xCA, 0xD7,
  0x35, 0x28, 0x0F, 0x12, 0x41, 0x5C, 0x7B, 0x66,
  0xDD, 0xC0, 0xE7, 0xFA, 0xA9, 0xB4, 0x93, 0x8E,
  0xF8, 0xE5, 0xC2, 0xDF, 0x8C, 0x91, 0xB6, 0xAB,
  0x10, 0x0D, 0x2A, 0x37, 0x64, 0x79, 0x5E, 0x43,
  0xB2, 0xAF, 0x88, 0x95, 0xC6, 0xDB, 0xFC, 0xE1,
  0x5A, 0x47, 0x60, 0x7D, 0x2E, 0x33, 0x14, 0x09,
  0x7F, 0x62, 0x45, 0x58, 0x0B, 0x16, 0x31, 0x2C,
  0x97, 0x8A, 0xAD, 0xB0, 0xE3, 0xFE, 0xD9, 0xC4
};

#if defined F_SBC_ENC_USE_FLOAT
static const double MIK[8][16] =
{
  0.707106781, 0.831469612, 0.923879533, 0.98078528, 1, 0.98078528, 0.923879533, 0.831469612, 0.707106781, 0.555570233, 0.382683432, 0.195090322, 0.0, -0.195090322, -0.382683432, -0.555570233,
    -0.707106781, -0.195090322, 0.382683432, 0.831469612, 1, 0.831469612, 0.382683432, -0.195090322, -0.707106781, -0.98078528, -0.923879533, -0.555570233, 0.0, 0.555570233, 0.923879533, 0.98078528,
    -0.707106781, -0.98078528, -0.382683432, 0.555570233, 1, 0.555570233, -0.382683432, -0.98078528, -0.707106781, 0.195090322, 0.923879533, 0.831469612, 0.0, -0.831469612, -0.923879533, -0.195090322,
    0.707106781, -0.555570233, -0.923879533, 0.195090322, 1, 0.195090322, -0.923879533, -0.555570233, 0.707106781, 0.831469612, -0.382683432, -0.98078528, 0.0, 0.98078528, 0.382683432, -0.831469612,
    0.707106781, 0.555570233, -0.923879533, -0.195090322, 1, -0.195090322, -0.923879533, 0.555570233, 0.707106781, -0.831469612, -0.382683432, 0.98078528, 0.0, -0.98078528, 0.382683432, 0.831469612,
    -0.707106781, 0.98078528, -0.382683432, -0.555570233, 1, -0.555570233, -0.382683432, 0.98078528, -0.707106781, -0.195090322, 0.923879533, -0.831469612, 0.0, 0.831469612, -0.923879533, 0.195090322,
    -0.707106781, 0.195090322, 0.382683432, -0.831469612, 1, -0.831469612, 0.382683432, 0.195090322, -0.707106781, 0.98078528, -0.923879533, 0.555570233, 0.0, -0.555570233, 0.923879533, -0.98078528,
    0.707106781, -0.831469612, 0.923879533, -0.98078528, 1, -0.98078528, 0.923879533, -0.831469612, 0.707106781, -0.555570233, 0.382683432, -0.195090322, 0.0, 0.195090322, -0.382683432, 0.555570233,
};

static const double C[80] = 
{
  0.00000000E+00, 1.56575398E-04, 3.43256425E-04, 5.54620202E-04,
    8.23919506E-04, 1.13992507E-03, 1.47640169E-03, 1.78371725E-03,
    2.01182542E-03, 2.10371989E-03, 1.99454554E-03, 1.61656283E-03,
    9.02154502E-04, -1.78805361E-04, -1.64973098E-03, -3.49717454E-03,
    5.65949473E-03, 8.02941163E-03, 1.04584443E-02, 1.27472335E-02,
    1.46525263E-02, 1.59045603E-02, 1.62208471E-02, 1.53184106E-02,
    1.29371806E-02, 8.85757540E-03, 2.92408442E-03, -4.91578024E-03,
    -1.46404076E-02, -2.61098752E-02, -3.90751381E-02, -5.31873032E-02,
    6.79989431E-02, 8.29847578E-02, 9.75753918E-02, 1.11196689E-01,
    1.23264548E-01, 1.33264415E-01, 1.40753505E-01, 1.45389847E-01,
    1.46955068E-01, 1.45389847E-01, 1.40753505E-01, 1.33264415E-01,
    1.23264548E-01, 1.11196689E-01, 9.75753918E-02, 8.29847578E-02,
    -6.79989431E-02, -5.31873032E-02, -3.90751381E-02, -2.61098752E-02,
    -1.46404076E-02, -4.91578024E-03, 2.92408442E-03, 8.85757540E-03,
    1.29371806E-02, 1.53184106E-02, 1.62208471E-02, 1.59045603E-02,
    1.46525263E-02, 1.27472335E-02, 1.04584443E-02, 8.02941163E-03,
    -5.65949473E-03, -3.49717454E-03, -1.64973098E-03, -1.78805361E-04,
    9.02154502E-04, 1.61656283E-03, 1.99454554E-03, 2.10371989E-03,
    2.01182542E-03, 1.78371725E-03, 1.47640169E-03, 1.13992507E-03,
    8.23919506E-04, 5.54620202E-04, 3.43256425E-04, 1.56575398E-04,
};
#endif

/* internal functions declarations */
static void initXTab(int* pTab);

static void calcPartials4(int* pAudio, const int* pFactors, int channels, int* pMatrix);
static void createSubbandSamples4(int* pMatrix, const int* pFactors, int channels, int* pSamples, TSampleData* pSampleData);
static void calcPartials8(int* pAudio, const int* pFactors, int channels, int* pMatrix);
static void createSubbandSamples8(int* pMatrix, const int* pFactors, int channels, int* pSamples, TSampleData* pSampleData);
static void joinSubbands4(int* pSamples, TSBCData* pSBCData, TSampleData* pSampleData, unsigned char* pJoint);
static void joinSubbands8(int* pSamples, TSBCData* pSBCData, TSampleData* pSampleData, unsigned char* pJoint);

static void createCRC(unsigned char* pOutput);

static void computeBits(void);
static void computeSlicesInPoolStereo( int max_bitneed, int bitneed[SBC_MAX_CHANNELS][SBC_MAX_SUBBANDS]);

#if defined F_SBC_DEBUG_FILE
static void dumpXTab(int* pTab);
static void dumpYTab(int* pTab);
static void dumpSampleData(TSampleData* pTab);
static void dumpInputBuffer(short* pBuffer, int index);
#endif

/***************************** sbcInit ************************************/
/**/
/************************************************************************/
void sbcInitEncoder(void)
{
  initXTab(&x[0][0]);
}

/**************************** sbcEncode **********************************/
/**/
/************************************************************************/
int sbcEncode(unsigned char* pInputBuffer, int inputSize, TSBCParams* params, unsigned char* pOutputBuffer, int* pOutputSize)
{  
  int blk;
  TSBCData* pSBCData = &SBCData;
  unsigned char* pScaleFactors;
  unsigned char* pJoint;
  unsigned char* pCRC;
  int frameSize;
  int reqInSize;
  TSampleData* pSampleData = &sampleData[0][0];
  unsigned char* pOut = pOutputBuffer;
  short* pIn = (short*) pInputBuffer;
#if defined F_SBC_ENC_REUSE_INPUT_BUFFER
  short* pSamples;
#else
  int* pSamples;
#endif

  /* transfer public params to internal values */
  pSBCData->sampling_frequency = (unsigned) params->samplingFrequency;
  pSBCData->nrof_blocks = blockNumbers[params->blockNumber];
  pSBCData->channel_mode = params->channelMode;
  pSBCData->nrof_channels = (params->channelMode == SBC_MODE_MONO) ? 1 : 2;
  pSBCData->allocation_method = params->allocMethod;
  pSBCData->nrof_subbands = subbandNumbers[params->subbandNumber];
  pSBCData->bitpool = params->bitpool;

  /* check for sufficient input buffer size */
  reqInSize = pSBCData->nrof_subbands * pSBCData->nrof_channels * (pSBCData->nrof_blocks << 1);
  if (inputSize < reqInSize)
  {
    *pOutputSize = 0;
    return SBC_NOT_ENOUGH_DATA;
  }

  /* check for sufficient output buffer size */   
  {
    int sb, bb;
    frameSize = 4;
    sb = pSBCData->nrof_subbands;
    bb = pSBCData->nrof_blocks * pSBCData->bitpool;
    
    switch (pSBCData->channel_mode)
    {
    case SBC_MODE_MONO:
      frameSize += (sb >>1 ) + (bb >> 3); 
      /* (bb)>>3 must be rounded towards plus infinity */
      if (bb & 0x7)
        frameSize++;
      break;
    case SBC_MODE_DUAL:
      frameSize += sb + (bb >> 2); 
      break;
    case SBC_MODE_STEREO:
      frameSize += sb + (bb >> 3); 
      /* (bb)>>3 must be rounded towards plus infinity */
      if (bb & 0x7)
        frameSize++;
      break;
    case SBC_MODE_JOINT:
      frameSize += sb + ((sb + bb) >> 3); 
      /* (sb + bb)>>3 must be rounded towards plus infinity */
      if ((sb + bb) & 0x7)
        frameSize++;
      break;
    }

    if (*pOutputSize < frameSize)
    {
      *pOutputSize = frameSize;
      return SBC_BUFFER_TOO_SMALL;
    }
  }

  /* create frame header */
  {
    unsigned char c = 0;
    *pOut++ = SBC_SYNCBYTE;
    c |= (params->samplingFrequency << 6);
    c |= (params->blockNumber << 4);
    c |= (params->channelMode << 2);
    c |= (params->allocMethod << 1);
    c |= params->subbandNumber;
    *pOut++ = c;
    *pOut++ = params->bitpool;
    pCRC = pOut++;

    if (params->channelMode == SBC_MODE_JOINT)
    {
      pJoint = pOut++;
      *pJoint = 0;
    }
  }

  /* reserve space for scale factors */
  pScaleFactors = pOut;
  pOut += (pSBCData->nrof_channels * (pSBCData->nrof_subbands >> 1));

  /* initialize sample data */
  {
    int i;
    for (i = 0; i < 16; i++)
      pSampleData[i].calc1 = pSampleData[i].calc2 = 0;
  }


#if ! defined F_SBC_ENC_USE_FLOAT
  /* create subband samples */
  {
#if ! defined F_SBC_ENC_REUSE_INPUT_BUFFER
    pSamples = &samples[0];
#endif
    for (blk = pSBCData->nrof_blocks; blk > 0; blk--)
    {
      static int offset = 0;
      int* pxLeft, *pxRight;
      int sb, ns;
#if defined F_SBC_ENC_REUSE_INPUT_BUFFER
      pSamples = pIn; /* input buffer is used as temporary memory for subband samples */
#endif

      /* update audio ring buffer position */
      if (offset <= 0)
        offset = 81;
      else
        offset -= 9;
      pxLeft = &x[0][0] + offset;
      pxRight = pxLeft + 99;

      /* fill in new audio samples */ 
      {
        ns = pSBCData->nrof_subbands;
        for (sb = 0; sb < ns; sb++)
        {
          pxLeft[ns - sb - 1] = *pIn++;
          if (pSBCData->nrof_channels == 2)
          {
            pxRight[ns - sb - 1] = *pIn++;
          }
        }
      }

      /* frequency band analysis */
      if (pSBCData->nrof_subbands == 4)
      {
        calcPartials4(pxLeft, &c4[0], pSBCData->nrof_channels, &y[0][0]);
        createSubbandSamples4(&y[0][0], &mik4[0][0], pSBCData->nrof_channels, (int*) pSamples, &sampleData[0][0]);
      }
      else
      {
        calcPartials8(pxLeft, &c8[0], pSBCData->nrof_channels, &y[0][0]);
        createSubbandSamples8(&y[0][0], &mik8[0][0], pSBCData->nrof_channels, (int*) pSamples, &sampleData[0][0]);
      } /* if subbands */

#if ! defined F_SBC_ENC_REUSE_INPUT_BUFFER
      pSamples += pSBCData->nrof_subbands * pSBCData->nrof_channels;
#endif
    }  /* for blk */
  }

#else

  /* use floating point arithmetic; for debugging only, for stereo 8 subbands only */
  {
#if ! defined F_SBC_ENC_REUSE_INPUT_BUFFER
    pSamples = &samples[0];
#endif
    
    for (blk = pSBCData->nrof_blocks; blk > 0; blk--)
    {
      int i, k;

      /* shift audio samples */
      for (i = 79; i >= 8; i--)
      {
        X[0][i] = X[0][i - 8];
        X[1][i] = X[1][i - 8];
      }

      /* fill in new samples */
      for (i = 7; i >= 0; i--)
      {
        X[0][i] = *pIn++;
        X[1][i] = *pIn++;
      }

      /* multiply with filter coeffs */
      for (i = 0; i < 80 ; i++)
      {
        Z[0][i] = X[0][i] * C[i];
        Z[1][i] = X[1][i] * C[i];
      }

      /* init Y */
      for (i = 0; i < 16; i++)
      {
        Y[0][i] = 0;
        Y[1][i] = 0;
      }
      /* partial calc */
      for (i = 0; i < 16; i++)
      {
        for (k = 0; k < 5; k++)
        {
          Y[0][i] += Z[0][i + k * 16];
          Y[1][i] += Z[1][i + k * 16];
        }
      }

      /* init S */
      for (i = 0; i < 16; i++)
      {
        S[0][i] = 0;
        S[1][i] = 0;
      }
      /* subband samples */
      for (i = 0; i < 8; i++)
      {
        for (k = 0; k < 16; k++)
        {
          S[0][i] += MIK[i][k] * Y[0][k];
          S[1][i] += MIK[i][k] * Y[1][k];
        }
      }

      for (i = 0; i < 8; i++)
      {
        if (S[0][i] >= 0)
          *pSamples++ = (int) (S[0][i] + 0.5);
        else
          *pSamples++ = (int) (S[0][i] - 0.5);

        if (pSamples[-1] > sampleData[0][i].calc1)
          sampleData[0][i].calc1 = pSamples[-1];
        if (pSamples[-1] < sampleData[0][i].calc2)
          sampleData[0][i].calc2 = pSamples[-1];
      }

      for (i = 0; i < 8; i++)
      {
        if (S[1][i] >= 0)
          *pSamples++ = (int) (S[1][i] + 0.5);
        else
          *pSamples++ = (int) (S[1][i] - 0.5);

        if (pSamples[-1] > sampleData[1][i].calc1)
          sampleData[1][i].calc1 = pSamples[-1];
        if (pSamples[-1] < sampleData[1][i].calc2)
          sampleData[1][i].calc2 = pSamples[-1];
      }

    }  /* for blk */
  }
#endif

  /* calculate scale factors */
  {
    int i = 0;
    for (i = pSBCData->nrof_channels * pSBCData->nrof_subbands - 1; i >= 0; i--)
    {
      int sf;
      int mx = pSampleData[i].calc1;
      int mn = pSampleData[i].calc2 * -1;
      if (mn > mx)
        mx = mn;

      if (mx > 255) if (mx > 4095) if (mx > 16383) if (mx > 32767) sf = 15; else sf = 14; else if (mx > 8191) sf = 13; else sf = 12;
      else if (mx > 1023) if (mx > 2047) sf = 11; else sf = 10; else if (mx > 511) sf = 9; else sf = 8;
      else if (mx > 15) if (mx > 63) if (mx > 127) sf = 7; else sf = 6; else if (mx > 31) sf = 5; else sf = 4;
      else if (mx > 3) if (mx > 7) sf = 3; else sf = 2; else if (mx > 1) sf = 1; else sf = 0; 

      pSampleData[i].scale_factor = sf;
    } /* for i */
  }

  /* join subband samples */
#if defined F_SBC_ENC_REUSE_INPUT_BUFFER
  pSamples = (short*) pInputBuffer;
#else
  pSamples = &samples[0];
#endif
  if (pSBCData->channel_mode == SBC_MODE_JOINT)
  {  
    if (pSBCData->nrof_subbands == 4)
      joinSubbands4((int*) pSamples, pSBCData, &sampleData[0][0], pJoint);
    else
      joinSubbands8((int*) pSamples, pSBCData, &sampleData[0][0], pJoint);
  } /* if joint */

  /* write scale factors */
  {
    unsigned char c = 0;
    int ch;

    if (pSBCData->nrof_subbands == 4)
    {
      if (pSBCData->channel_mode == SBC_MODE_JOINT)
      {
        /* data pointer adjustment required due to just 4 join bits in header  */
        for (ch = 2; ch > 0; ch--)
        {
          c = pSampleData[0].scale_factor;
          pScaleFactors[-1] |= c;  /* write to output buffer */
          c = pSampleData[1].scale_factor << 4;
          c |= pSampleData[2].scale_factor;
          pScaleFactors[0] = c;  /* write to output buffer */
          c = pSampleData[3].scale_factor << 4;
          pScaleFactors[1] = c;  /* write to output buffer */
          pSampleData += 4;
          pScaleFactors += 2;
        }
      }
      else
      {
        for (ch = pSBCData->nrof_channels; ch > 0; ch--)
        {
          c = pSampleData[0].scale_factor << 4;
          c |= pSampleData[1].scale_factor;
          pScaleFactors[0] = c;  /* write to output buffer */
          c = pSampleData[2].scale_factor << 4;
          c |= pSampleData[3].scale_factor;
          pScaleFactors[1] = c;  /* write to output buffer */
          pSampleData += 4;
          pScaleFactors += 2;
        }
      }
    }
    else
    {
      for (ch = pSBCData->nrof_channels; ch > 0; ch--)
      {
        c = pSampleData[0].scale_factor << 4;
        c |= pSampleData[1].scale_factor;
        pScaleFactors[0] = c;  /* write to output buffer */
        c = pSampleData[2].scale_factor << 4;
        c |= pSampleData[3].scale_factor;
        pScaleFactors[1] = c;  /* write to output buffer */
        c = pSampleData[4].scale_factor << 4;
        c |= pSampleData[5].scale_factor;
        pScaleFactors[2] = c;  /* write to output buffer */
        c = pSampleData[6].scale_factor << 4;
        c |= pSampleData[7].scale_factor;
        pScaleFactors[3] = c;  /* write to output buffer */
        pSampleData += 8;
        pScaleFactors += 4;
      }
    } /* if subbands */
    pSampleData = &sampleData[0][0];
  } /* write scale factors */

  /* create checksum */
  createCRC(pOutputBuffer);

  /* adjust sample data for computeBits function */
  if (pSBCData->nrof_channels == 2)
  {
    if (pSBCData->nrof_subbands == 4)
    {
      /* move TSampleData structs into second row of sampleData array */
      memcpy(&pSampleData[8], &pSampleData[4], 4 * sizeof(TSampleData));
    }
  }

  /* calculate bit allocation */
  computeBits();

  /* re-adjust sample data */
  if (pSBCData->nrof_channels == 2)
  {
    if (pSBCData->nrof_subbands == 4)
    {
      /* move TSampleData structs into one row within sampleData array */
      memcpy(&pSampleData[4], &pSampleData[8], 4 * sizeof(TSampleData));
    }
  }
  /* set terminator */
  pSampleData[pSBCData->nrof_channels * pSBCData->nrof_subbands].bits = SBC_TERMINATOR;

  /* quantize, normalize and write subband samples */
  {
    unsigned out = 0;
    int resBits = 0;
#if defined F_SBC_ENC_REUSE_INPUT_BUFFER
    pSamples = (short*) pInputBuffer;  /* input buffer is storing the unquantized subband samples */
#else
    pSamples = &samples[0];
#endif

    if (pSBCData->nrof_subbands == 4)
    {
      if (pSBCData->channel_mode == SBC_MODE_JOINT)
      {
        /* data pointer adjustment required due to just 4 joint bits in header  */
        pOut--;
        resBits = 4;
        out = (*pOut & 0xf0) >> 4;
      }
    }

    /* precalculate fixed factors */
    for (;;)
    {
      int bits;

      if ((bits = (*pSampleData++).bits) != 0)
      {
        /* break on terminator */
        if (bits < 0)  /* arm assembler optimization; saves 3 cycles compared to 'if (bits == SBC_TERMINATOR)' ! */
          break;
        pSampleData[-1].calc1 = ((1 << bits) - 1) << (GUARD_QUANT - 1);  /* levels including factor 1/2, guarded */
        pSampleData[-1].calc2 = (pSampleData[-1].calc1) >> (pSampleData[-1].scale_factor + 1); /* levels/scale factors, guarded via calc1 */
      }
    }

    /* quantize, normalize and write subband samples */
    for (blk = pSBCData->nrof_blocks; blk > 0; blk--)
    {
      pSampleData = &sampleData[0][0];

      for (;;)
      {
        int bits;

        if ((bits = (*pSampleData++).bits) != 0)
        {
          int qs;
          int msk;
          /* break on terminator */
          if (bits < 0)  /* arm assembler optimization; saves 3 cycles compared to 'if (bits == SBC_TERMINATOR)' ! */
            break;
          /* calculate subband sample */
          qs = (*pSamples++ * pSampleData[-1].calc2 + pSampleData[-1].calc1) >> (GUARD_QUANT); /* un-guard (= round towards minus infinity) */
          /* qs is a 32 bit integer holding the data to be written within the [bits] LSBs */                    

#if defined F_SBC_ENC_USE_FLOAT
          {
            double should;
            should = ((((double) pSamples[-1]) / (1 <<(pSampleData[-1].scale_factor + 1)) + 1) * ((1 << pSampleData[-1].bits) - 1.0) - 1.0) / 2.0;
            if (should > 0)
              should += 0.5;
            else if (should < 0)
              should -= 0.5;
            qs = (int) should;
          }
#endif

          msk = (1 << bits) - 1;
          qs &= msk;
#if defined F_SBC_DEBUG_FILE
          pSampleData[-1].qs = qs;
#endif
          
          /* write subband sample */
          out <<= bits;
          out |= qs;
          resBits += bits;
          while (resBits >= 8)
          {
            *pOut++ = (unsigned char) (out >> (resBits - 8));
            resBits -= 8;
          } /* while */
        } /* if bits */
        else
        {
#if defined F_SBC_DEBUG_FILE
          pSampleData[-1].qs = 0;
#endif
          pSamples++;
        }
#if defined F_SBC_DEBUG_FILE
        pSampleData[-1].sb_sample = *pSamples;
#endif
      } /* for */

#if defined F_SBC_DEBUG_FILE
      {
        int sb, nsb;
        TSampleData* pd;
        nsb = pSBCData->nrof_subbands;
        pd = &sampleData[0][0];
        for (sb = 0; sb < nsb; sb++)
        {
          printf("%d, %d, %d, %u\n",
            pd[sb].sb_sample, pd[sb].bits, pd[sb].scale_factor, pd[sb].qs);
          printf("%d, %d, %d, %u\n",
            pd[sb+nsb].sb_sample, pd[sb+nsb].bits, pd[sb+nsb].scale_factor, pd[sb+nsb].qs);
        }  
      }
#endif

    } /* for blk */

    /* padding */
    if (resBits > 0)
    {
      *pOut++ = (unsigned char) (out >> (8 - resBits));
    }
  }

  *pOutputSize = pOut - pOutputBuffer;

#if defined F_SBC_DEBUG_FILE
  if (*pOutputSize != frameSize)
    printf("! output size mismatch: frameSize %d, created %d\n", frameSize, *pOutputSize);
  if ((unsigned char*) pIn - pInputBuffer != reqInSize)
    printf("! input size mismatch: requested input %d, read %d\n", reqInSize, ((unsigned char*) pIn - pInputBuffer));
#endif

  return ((unsigned char*) pIn - pInputBuffer);
}


/***************************** initXTab ***********************************/
/* initialize x array */
/************************************************************************/
static void initXTab(int* pTab)
{
  int i;
  int* px   = pTab;
  int* px1  = pTab + 99;

  memset(px, 0, 198);
  for (i = 0; i < 11; i++)
  {
    px [i * 9 + 8] = (int) &px [matrixingTable[i]]; /* set pointer to successor group, first channel */
    px1[i * 9 + 8] = (int) &px1[matrixingTable[i]]; /* set pointer to successor group, second channel */
  }
} /* initXTab */


/******************************** calcPartials4 *****************************/
/*  */
/************************************************************************/
static void calcPartials4(int* pAudio, const int* pFactors, int channels, int* pMatrix)
{
  int y0, y1, y2, y3;
  int iter = 2;
  int* px = pAudio;
  const int* pc = pFactors;
  int* py = pMatrix;

  while (--channels >= 0)
  {
    while (--iter >= 0)
    {
      y0 = y1 = y2 = y3 = GUARD_PAR_HALFBIT;

      y0 += *pc++ * px[0];
      y1 += *pc++ * px[1];
      y2 += *pc++ * px[2];
      y3 += *pc++ * px[3];
      px = (int*) px[8];  /* jump to next group */

      y0 += *pc++ * px[0];
      y1 += *pc++ * px[1];
      y2 += *pc++ * px[2];
      y3 += *pc++ * px[3];
      px = (int*) px[8];

      y0 += *pc++ * px[0];
      y1 += *pc++ * px[1];
      y2 += *pc++ * px[2];
      y3 += *pc++ * px[3];
      px = (int*) px[8];

      y0 += *pc++ * px[0];
      y1 += *pc++ * px[1];
      y2 += *pc++ * px[2];
      y3 += *pc++ * px[3];
      px = (int*) px[8];

      y0 += *pc++ * px[0];
      y1 += *pc++ * px[1];
      y2 += *pc++ * px[2];
      y3 += *pc++ * px[3];
      px += 9;
      px = (int*) px[8];

      *py++ = y0 >> (GUARD_PAR);
      *py++ = y1 >> (GUARD_PAR);
      *py++ = y2 >> (GUARD_PAR);
      *py++ = y3 >> (GUARD_PAR);

    }  /* iter */
    iter = 2;
    pc -= 40;
    px = pAudio + 99;
  }  /* channels */
} /* calcPartials4 */


/********************* createSubbandSamples4 *****************************/
/* */
/************************************************************************/
static void createSubbandSamples4(int* pMatrix, const int* pFactors, int channels, int* pSamples, TSampleData* pSampleData)
{
  int i, s;
  int* py = pMatrix;
  const int* pmik = pFactors;
#if defined F_SBC_ENC_REUSE_INPUT_BUFFER
  short* ps = (short*) pSamples;
#else
  int* ps = pSamples;
#endif
  TSampleData* pd = pSampleData;

  while (--channels >= 0)
  {
    for (i = 0; i < 4; i++)
    {
      s = GUARD_SUB_HALFBIT;
      s += *pmik++ * py[0];
      s += *pmik++ * py[1];
      s += *pmik++ * py[2];
      s += *pmik++ * py[3];
      s += *pmik++ * py[4];
      s += *pmik++ * py[5];
      s += *pmik++ * py[6];
      s += *pmik++ * py[7];
      s >>= (GUARD_SUB);
      
      /* save to sample buffer */
#if defined F_SBC_ENC_REUSE_INPUT_BUFFER
      limit(s);
      *ps++ = (short) s;  /* save to sample buffer */
#else
      *ps++ = s;
#endif

      if (s > (*pd++).calc1)
        pd[-1].calc1 = s;   /* save maximum */
      else
      {
        if (s < pd[-1].calc2)
          pd[-1].calc2 = s;   /* save minimum */
      }
    }

    py += 8;
    pmik -= 32;
  } /* channels */
} /* createSubbandSamples4 */


/******************************** calcPartials8 *****************************/
/*  */
/************************************************************************/
static void calcPartials8(int* pAudio, const int* pFactors, int channels, int* pMatrix)
{
  int y0, y1, y2, y3, y4, y5, y6, y7;
  int iter = 2;
  int* px = pAudio;
  const int* pc = pFactors;
  int* py = pMatrix;

  while (--channels >= 0)
  {
    while (--iter >= 0)
    {
      y0 = y1 = y2 = y3 = y4 = y5 = y6 = y7 = GUARD_PAR_HALFBIT;

      y0 += *pc++ * px[0];
      y1 += *pc++ * px[1];
      y2 += *pc++ * px[2];
      y3 += *pc++ * px[3];
      y4 += *pc++ * px[4];
      y5 += *pc++ * px[5];
      y6 += *pc++ * px[6];
      y7 += *pc++ * px[7];
      px = (int*) px[8];  /* jump to next group */

      y0 += *pc++ * px[0];
      y1 += *pc++ * px[1];
      y2 += *pc++ * px[2];
      y3 += *pc++ * px[3];
      y4 += *pc++ * px[4];
      y5 += *pc++ * px[5];
      y6 += *pc++ * px[6];
      y7 += *pc++ * px[7];
      px = (int*) px[8];

      y0 += *pc++ * px[0];
      y1 += *pc++ * px[1];
      y2 += *pc++ * px[2];
      y3 += *pc++ * px[3];
      y4 += *pc++ * px[4];
      y5 += *pc++ * px[5];
      y6 += *pc++ * px[6];
      y7 += *pc++ * px[7];
      px = (int*) px[8];

      y0 += *pc++ * px[0];
      y1 += *pc++ * px[1];
      y2 += *pc++ * px[2];
      y3 += *pc++ * px[3];
      y4 += *pc++ * px[4];
      y5 += *pc++ * px[5];
      y6 += *pc++ * px[6];
      y7 += *pc++ * px[7];
      px = (int*) px[8];

      y0 += *pc++ * px[0];
      y1 += *pc++ * px[1];
      y2 += *pc++ * px[2];
      y3 += *pc++ * px[3];
      y4 += *pc++ * px[4];
      y5 += *pc++ * px[5];
      y6 += *pc++ * px[6];
      y7 += *pc++ * px[7];
      px += 9;
      px = (int*) px[8];

      *py++ = y0 >> (GUARD_PAR);
      *py++ = y1 >> (GUARD_PAR);
      *py++ = y2 >> (GUARD_PAR);
      *py++ = y3 >> (GUARD_PAR);
      *py++ = y4 >> (GUARD_PAR);
      *py++ = y5 >> (GUARD_PAR);
      *py++ = y6 >> (GUARD_PAR);
      *py++ = y7 >> (GUARD_PAR);

    }  /* iter */
    iter = 2;
    pc -= 80;
    px = pAudio + 99;
  }  /* channels */
} /* calcPartials8 */


/********************* createSubbandSamples8 *****************************/
/* */
/************************************************************************/
static void createSubbandSamples8(int* pMatrix, const int* pFactors, int channels, int* pSamples, TSampleData* pSampleData)
{
  int i, s;
  int* py = pMatrix;
  const int* pmik = pFactors;
#if defined F_SBC_ENC_REUSE_INPUT_BUFFER
  short* ps = (short*) pSamples;
#else
  int* ps = pSamples;
#endif
  TSampleData* pd = pSampleData;

  while (--channels >= 0)
  {
    for (i = 0; i < 8; i++)
    {
      s = GUARD_SUB_HALFBIT;
      s += *pmik++ * py[0];
      s += *pmik++ * py[1];
      s += *pmik++ * py[2];
      s += *pmik++ * py[3];
      s += *pmik++ * py[4];
      s += *pmik++ * py[5];
      s += *pmik++ * py[6];
      s += *pmik++ * py[7];
      s += *pmik++ * py[8];
      s += *pmik++ * py[9];
      s += *pmik++ * py[10];
      s += *pmik++ * py[11];
      s += *pmik++ * py[12];
      s += *pmik++ * py[13];
      s += *pmik++ * py[14];
      s += *pmik++ * py[15];
      s >>= (GUARD_SUB);

      /* save to sample buffer */
#if defined F_SBC_ENC_REUSE_INPUT_BUFFER
      limit(s);
      *ps++ = (short) s;  /* save to sample buffer */
#else
      *ps++ = s;
#endif

      if (s > (*pd++).calc1)
        pd[-1].calc1 = s;   /* save maximum */
      else
      {
        if (s < pd[-1].calc2)
          pd[-1].calc2 = s;   /* save minimum */
      }
    }

    py += 16;
    pmik -= 128;
  } /* channels */
} /* createSubbandSamples8 */


/*************************** joinSubbands4 *********************************/
/* */
/************************************************************************/
static void joinSubbands4(int* pSamples, TSBCData* pSBCData, TSampleData* pSampleData, unsigned char* pJoint)
{
  int join = 0;
  int jointSamples[32];
  TSampleData jointData[8];
  int sb, blk, blkR, iL, iR;
#if defined F_SBC_ENC_REUSE_INPUT_BUFFER
  short* ps = (short*) pSamples;
#else
  int* ps = pSamples;
#endif

  /* initialize joint data */
  for (sb = 7; sb >= 0; sb--)
    jointData[sb].calc1 = jointData[sb].calc2 = 0;

  /* iterate through subbands; the highest subband is never joint */
  for (sb = 2; sb >= 0; sb--)
  {
    int sbR, mx, sf, sumN, sumJ;
    sbR = sb + 4;
    iL = sb + ((pSBCData->nrof_blocks) << 3);
    iR = iL + 4;
    for (blk = pSBCData->nrof_blocks - 1; blk >= 0; blk--)
    {
      /* calculate sum and difference signals */
      iL -= 8;
      iR -= 8;
      blkR = blk + pSBCData->nrof_blocks;
      jointSamples[blk] = (ps[iL] + ps[iR]) >> 1;
      jointSamples[blkR] = (ps[iL] - ps[iR]) >> 1;

      /* sum (left) channel maximum */
      if (jointSamples[blk] > jointData[sb].calc1)
        jointData[sb].calc1 = jointSamples[blk];
      else
      {
        /* sum (left) channel minimum */
        if (jointSamples[blk] < jointData[sb].calc2)
          jointData[sb].calc2 = jointSamples[blk];
      }

      /* difference (right) channel maximum */
      if (jointSamples[blkR] > jointData[sbR].calc1)
        jointData[sbR].calc1 = jointSamples[blkR];
      else
      {
        /* difference (right) channel minimum */
        if (jointSamples[blkR] < jointData[sbR].calc2)
          jointData[sbR].calc2 = jointSamples[blkR];
      }
    } /* for blk */

    /* calculate scale factors */
    mx = jointData[sb].calc1;
    jointData[sb].calc2 *= -1;
    if (jointData[sb].calc2 > mx)
      mx = jointData[sb].calc2;
    if (mx > 255) if (mx > 4095) if (mx > 16383) if (mx > 32767) sf = 15; else sf = 14; else if (mx > 8191) sf = 13; else sf = 12;
    else if (mx > 1023) if (mx > 2047) sf = 11; else sf = 10; else if (mx > 511) sf = 9; else sf = 8;
    else if (mx > 15) if (mx > 63) if (mx > 127) sf = 7; else sf = 6; else if (mx > 31) sf = 5; else sf = 4;
    else if (mx > 3) if (mx > 7) sf = 3; else sf = 2; else if (mx > 1) sf = 1; else sf = 0; 
    jointData[sb].scale_factor = sf;

    mx = jointData[sbR].calc1;
    jointData[sbR].calc2 *= -1;
    if (jointData[sbR].calc2 > mx)
      mx = jointData[sbR].calc2;
    if (mx > 255) if (mx > 4095) if (mx > 16383) if (mx > 32767) sf = 15; else sf = 14; else if (mx > 8191) sf = 13; else sf = 12;
    else if (mx > 1023) if (mx > 2047) sf = 11; else sf = 10; else if (mx > 511) sf = 9; else sf = 8;
    else if (mx > 15) if (mx > 63) if (mx > 127) sf = 7; else sf = 6; else if (mx > 31) sf = 5; else sf = 4;
    else if (mx > 3) if (mx > 7) sf = 3; else sf = 2; else if (mx > 1) sf = 1; else sf = 0; 
    jointData[sbR].scale_factor = sf;

    /* make joint decision */
    sumN = pSampleData[sb].scale_factor + pSampleData[sbR].scale_factor;
    sumJ = jointData[sb].scale_factor + jointData[sbR].scale_factor;
    if (sumJ < sumN)
    {
      /* add joint bit */
      join |= (0x80 >> sb);

      /* replace samples */
      iL = sb + ((pSBCData->nrof_blocks) << 3);
      iR = iL + 4;
      for (blk = pSBCData->nrof_blocks - 1; blk >= 0; blk--)
      {
        iL -= 8;
        iR -= 8;
#if defined F_SBC_ENC_REUSE_INPUT_BUFFER
        ps[iL] = (short) jointSamples[blk];
        ps[iR] = (short) jointSamples[blk + pSBCData->nrof_blocks];
#else
        ps[iL] = jointSamples[blk];
        ps[iR] = jointSamples[blk + pSBCData->nrof_blocks];
#endif
      } /* for blk */

      /* replace scale factors */
      pSampleData[sb].scale_factor = jointData[sb].scale_factor;
      pSampleData[sbR].scale_factor = jointData[sbR].scale_factor;
    } /* if join */
  } /* for subband */

  *pJoint = join;
}


/*************************** joinSubbands8 *********************************/
/* */
/************************************************************************/
static void joinSubbands8(int* pSamples, TSBCData* pSBCData, TSampleData* pSampleData, unsigned char* pJoint)
{
  int join = 0;
  int jointSamples[32];
  TSampleData jointData[16];
  int sb, blk, blkR, iL, iR;
#if defined F_SBC_ENC_REUSE_INPUT_BUFFER
  short* ps = (short*) pSamples;
#else
  int* ps = pSamples;
#endif

  /* initialize joint data */
  for (sb = 15; sb >= 0; sb--)
    jointData[sb].calc1 = jointData[sb].calc2 = 0;

  /* iterate through subbands; the highest subband is never joint */
  for (sb = 6; sb >= 0; sb--)
  {
    int sbR, mx, sf, sumN, sumJ;
    sbR = sb + 8;
    iL = sb + ((pSBCData->nrof_blocks) << 4);
    iR = iL + 8;
    for (blk = pSBCData->nrof_blocks - 1; blk >= 0; blk--)
    {
      /* calculate sum and difference signals */
      iL -= 16;
      iR -= 16;
      blkR = blk + pSBCData->nrof_blocks;
      jointSamples[blk] = (ps[iL] + ps[iR]) >> 1;
      jointSamples[blkR] = (ps[iL] - ps[iR]) >> 1;

      /* sum (left) channel maximum */
      if (jointSamples[blk] > jointData[sb].calc1)
        jointData[sb].calc1 = jointSamples[blk];
      else
      {
        /* sum (left) channel minimum */
        if (jointSamples[blk] < jointData[sb].calc2)
          jointData[sb].calc2 = jointSamples[blk];
      }

      /* difference (right) channel maximum */
      if (jointSamples[blkR] > jointData[sbR].calc1)
        jointData[sbR].calc1 = jointSamples[blkR];
      else
      {
        /* difference (right) channel minimum */
        if (jointSamples[blkR] < jointData[sbR].calc2)
          jointData[sbR].calc2 = jointSamples[blkR];
      }
    } /* for blk */

    /* calculate scale factors */
    mx = jointData[sb].calc1;
    jointData[sb].calc2 *= -1;
    if (jointData[sb].calc2 > mx)
      mx = jointData[sb].calc2;
    if (mx > 255) if (mx > 4095) if (mx > 16383) if (mx > 32767) sf = 15; else sf = 14; else if (mx > 8191) sf = 13; else sf = 12;
    else if (mx > 1023) if (mx > 2047) sf = 11; else sf = 10; else if (mx > 511) sf = 9; else sf = 8;
    else if (mx > 15) if (mx > 63) if (mx > 127) sf = 7; else sf = 6; else if (mx > 31) sf = 5; else sf = 4;
    else if (mx > 3) if (mx > 7) sf = 3; else sf = 2; else if (mx > 1) sf = 1; else sf = 0; 
    jointData[sb].scale_factor = sf;

    mx = jointData[sbR].calc1;
    jointData[sbR].calc2 *= -1;
    if (jointData[sbR].calc2 > mx)
      mx = jointData[sbR].calc2;
    if (mx > 255) if (mx > 4095) if (mx > 16383) if (mx > 32767) sf = 15; else sf = 14; else if (mx > 8191) sf = 13; else sf = 12;
    else if (mx > 1023) if (mx > 2047) sf = 11; else sf = 10; else if (mx > 511) sf = 9; else sf = 8;
    else if (mx > 15) if (mx > 63) if (mx > 127) sf = 7; else sf = 6; else if (mx > 31) sf = 5; else sf = 4;
    else if (mx > 3) if (mx > 7) sf = 3; else sf = 2; else if (mx > 1) sf = 1; else sf = 0; 
    jointData[sbR].scale_factor = sf;

    /* make joint decision */
    sumN = pSampleData[sb].scale_factor + pSampleData[sbR].scale_factor;
    sumJ = jointData[sb].scale_factor + jointData[sbR].scale_factor;
    if (sumJ < sumN)
    {
      /* add join bit */
      join |= (1 << (7 - sb));

      /* replace samples */
      iL = sb + ((pSBCData->nrof_blocks) << 4);
      iR = iL + 8;
      for (blk = pSBCData->nrof_blocks - 1; blk >= 0; blk--)
      {
        iL -= 16;
        iR -= 16;
#if defined F_SBC_ENC_REUSE_INPUT_BUFFER
        ps[iL] = (short) jointSamples[blk];
        ps[iR] = (short) jointSamples[blk + pSBCData->nrof_blocks];
#else
        ps[iL] = jointSamples[blk];
        ps[iR] = jointSamples[blk + pSBCData->nrof_blocks];
#endif
      }

      /* replace scale factors */
      pSampleData[sb].scale_factor = jointData[sb].scale_factor;
      pSampleData[sbR].scale_factor = jointData[sbR].scale_factor;
    } /* if join */
  } /* for subband */

  *pJoint = join;
}


/******************************* createCRC *******************************/
/* */
/************************************************************************/
static void createCRC(unsigned char* pOutput)
{
  int bits = 0;
  unsigned char data[11];
  TSBCData* pSBCData = &SBCData;
  unsigned crc = 0x0f;
  int i;
  unsigned octet;
  int len = 2;

  /* copy relevant data to local array */
  memcpy(data, pOutput + 1, len); /* first two bytes */
  if (pSBCData->nrof_channels == 2)
    len *= 2;
  if (pSBCData->nrof_subbands == 8)
    len *= 2;
  if (pSBCData->channel_mode == SBC_MODE_JOINT)
    len += 1;
  memcpy(data + 2, pOutput + 4, len); /* joint and scale bytes */
  bits = len * 8; /* joint and scale bits */
  if (len == 5)
    bits -= 4;  /* only 4 joint bits for 4 subbands */
  bits += 16;  /* header bits */

  for (i = 0; i < bits / 8; i++)
    crc = crcTable[crc ^ data[i]];

  octet = data[i];
  for (i = 0; i < bits % 8; i++) 
  {
    char bit = ((octet ^ crc) & 0x80) >> 7;
    crc = ((crc & 0x7f) << 1) ^ (bit ? 0x1d : 0);
    octet = octet << 1;
  }

  pOutput[3] = (unsigned char) crc;
}


/*************************** computeBits *********************************/
/* this implementation has been taken directly from the specification */
/************************************************************************/
static void computeBits(void)
{
  int loudness;
  int max_bitneed = 0;
  unsigned ch;
  unsigned sb;
  int bitneed[SBC_MAX_CHANNELS][SBC_MAX_SUBBANDS];
  TSBCData* pSBCData= &SBCData;

  if (pSBCData->channel_mode == SBC_MODE_MONO || pSBCData->channel_mode == SBC_MODE_DUAL)
  {
    for (ch=0; ch < pSBCData->nrof_channels; ch++)
    {
      /* Mono and Dual_Channel Bit Allocation */
      if (pSBCData->allocation_method == SBC_ALLOCSNR)
      {
        for (sb = 0; sb < pSBCData->nrof_subbands; sb++)
        {
          int b = bitneed[ch][sb] = sampleData[ch][sb].scale_factor;
          if (max_bitneed < b)
            max_bitneed = b;
        }
      }
      else
      {
        for (sb = 0;sb < pSBCData->nrof_subbands; sb++)
        {
          if (sampleData[ch][sb].scale_factor == 0)
          {
            int b = bitneed[ch][sb] = -5;
            if (max_bitneed < b)
              max_bitneed = b;
          }
          else
          {
            if (pSBCData->nrof_subbands == 4)
            {
              loudness = sampleData[ch][sb].scale_factor - offset4[pSBCData->sampling_frequency][sb];
            }
            else
            {
              loudness = sampleData[ch][sb].scale_factor - offset8[pSBCData->sampling_frequency][sb];
            }
            if (loudness > 0)
            {
              bitneed[ch][sb] = loudness / 2;
              if (max_bitneed < loudness / 2)
                max_bitneed = loudness / 2;

            }
            else
            {
              bitneed[ch][sb] = loudness;
              if (max_bitneed < loudness)
                max_bitneed = loudness;
            }
          }
        }
      }

      /* Next an iterative process finds out how many bitslices fit into the bitpool */
      {
        int bitcount = 0;
        int slicecount = 0;
        int bitslice = max_bitneed + 1; /* init just above the largest sf */
        do
        {
          bitslice--;
          bitcount += slicecount;
          slicecount = 0;
          for (sb = 0;sb < pSBCData->nrof_subbands; sb++)
          {
            if((bitneed[ch][sb] > bitslice + 1) && (bitneed[ch][sb] < bitslice + 16))
              slicecount++;
            else if(bitneed[ch][sb] == bitslice + 1)
              slicecount += 2;
          }
        } while (bitcount + slicecount < pSBCData->bitpool);

        if (bitcount + slicecount == pSBCData->bitpool)
        {
          bitcount += slicecount;
          bitslice--;
        }

        /* Thereafter, bits are distributed until the last bitslice is reached */
        for (sb=0;sb<pSBCData->nrof_subbands;sb++)
        {
          if(bitneed[ch][sb]<bitslice+2)
            sampleData[ch][sb].bits=0;
          else
            sampleData[ch][sb].bits=min(bitneed[ch][sb]-bitslice,16);
        }

        /* The remaining bits are allocated starting at subband 0 */
        sb=0;
        while(bitcount < pSBCData->bitpool && sb < pSBCData->nrof_subbands)
        {
          if((sampleData[ch][sb].bits>=2)&&(sampleData[ch][sb].bits<16))
          {
            sampleData[ch][sb].bits++; bitcount++;
          }
          else
            if((bitneed[ch][sb]==bitslice+1)&&(pSBCData->bitpool>bitcount+1))
            {
              sampleData[ch][sb].bits=2; bitcount+=2;
            }
            sb++;
        }
        sb=0;
        while(bitcount < pSBCData->bitpool && sb < pSBCData->nrof_subbands)
        {
          if (sampleData[ch][sb].bits < 16)
          {
            sampleData[ch][sb].bits++;
            bitcount++;
          }
          sb++;
        }
      }
    } /* for ch */
  } /* MONO and DUAL */
  else
  {
    /* Stereo and Joint_Stereo Bit Allocation */
    if (pSBCData->allocation_method == SBC_ALLOCSNR)
    {
      for (ch = 0;ch < 2; ch++)
      {
        for (sb = 0; sb < pSBCData->nrof_subbands; sb++)
        {
          int b = bitneed[ch][sb] = sampleData[ch][sb].scale_factor;
          if (b > max_bitneed)
            max_bitneed = b;
        }
      }
    }
    else
    {
      for (ch = 0;ch < 2; ch++)
      {
        for (sb=0;sb<pSBCData->nrof_subbands;sb++)
        {
          if (sampleData[ch][sb].scale_factor == 0)
          {
            bitneed[ch][sb] = -5;
            if (max_bitneed < -5)
              max_bitneed = -5;
          }
          else
          {
            if (pSBCData->nrof_subbands == 4)
            {
              loudness = sampleData[ch][sb].scale_factor - offset4[pSBCData->sampling_frequency][sb];
            }
            else
            {
              loudness = sampleData[ch][sb].scale_factor - offset8[pSBCData->sampling_frequency][sb];
            }
            if (loudness > 0)
            {
              bitneed[ch][sb] = (loudness >> 1);
              if ((loudness >> 1) > max_bitneed)
                max_bitneed = (loudness >> 1);
            }
            else
            {
              bitneed[ch][sb] = loudness;
              if (max_bitneed < loudness)
                max_bitneed = loudness;
            }
          }
        }
      }
    }    
    /* Next an iterative process finds out how many bitslices fit into the bitpool */
    computeSlicesInPoolStereo(max_bitneed, bitneed);
  } /* Joint and Stereo Allocation */
} /* computeBits */


/*************************** computeSlicesInPoolStereo ******************/
/* An iterative process finds out how many bitslices fit into the bitpool */
/* The result is stored in the global array "bits" */
/* this implementation has been taken directly from the specification */
/************************************************************************/
static void computeSlicesInPoolStereo( int max_bitneed, int bitneed[SBC_MAX_CHANNELS][SBC_MAX_SUBBANDS])
{
  static int n = 0;

  int bitcount    = 0;
  int slicecount  = 0;
  int bitslice    = max_bitneed + 1; /* init just above the largest sf */
  int sb, ch;
  TSBCData* pSBCData= &SBCData;
  int bitpool   = pSBCData->bitpool;
  int nrof_subbands = pSBCData->nrof_subbands;

  int* bn0 = bitneed[0];
  int* bn1 = bitneed[1];

  do
  {
    bitcount += slicecount;
    slicecount = 0;

    sb = nrof_subbands - 1;
    do
    {
      int bitn;
      /* channel 0 */
      bitn = bn0[sb];
      if((bitn >= bitslice) && (bitn < bitslice + 15))
        slicecount++;
      if(bitn == bitslice)
        slicecount++;

      /* channel 1 */
      bitn = bn1[sb];
      if((bitn >= bitslice) && (bitn < bitslice + 15))
        slicecount++;
      if(bitn == bitslice)
        slicecount++;

    } while (sb--);
    bitslice--;
  } while (bitcount + slicecount < bitpool);

  if (bitcount + slicecount == bitpool)
  {
    bitcount += slicecount;
    bitslice--;
  }

  /* Thereafter bits are distributed until the last bitslice is reached */
  for (ch = 0; ch < 2; ch++)
  {
    for (sb=0;sb<nrof_subbands;sb++) 
    {
      if(bitneed[ch][sb] < bitslice + 2) 
        sampleData[ch][sb].bits = 0; 
      else 
        sampleData[ch][sb].bits = min(bitneed[ch][sb] - bitslice, 16); 
    }
  }

  /* The remaining bits are allocated starting with subband 0 of the first channel */
  {
    unsigned ch = 0;
    sb=0;
    while(bitcount < pSBCData->bitpool && sb < (int)pSBCData->nrof_subbands)
    {
      if((sampleData[ch][sb].bits>=2)&&(sampleData[ch][sb].bits<16))
      {
        sampleData[ch][sb].bits++; bitcount++;
      }
      else if((bitneed[ch][sb]==bitslice+1)&&(pSBCData->bitpool>bitcount+1))
      {
        sampleData[ch][sb].bits=2; bitcount+=2;
      }
      if (ch == 1)
      {
        ch = 0;
        sb++;
      }
      else
      {
        ch = 1;
      }
    }
    ch=0;
    sb=0;
    while(bitcount < pSBCData->bitpool && sb < (int)pSBCData->nrof_subbands)
    {
      if (sampleData[ch][sb].bits < 16)
      {
        sampleData[ch][sb].bits++;
        bitcount++;
      }
      if (ch == 1)
      {
        ch = 0;
        sb++;
      }
      else
      {
        ch = 1;
      }
    }
  }
}

#if defined F_SBC_DEBUG_FILE
/************************************************************************/
/* various debug helper functions */
/************************************************************************/
static void dumpXTab(int* pTab)
{
  int i;
  int* px = pTab;

  printf("x dump\n");
  for (i = 0; i < 180; i++)
  {
    if ((i % 9) == 8)
      continue;
    printf("x[%d] = val %4d\n", i,  px[i]);
  }
  printf("\n");
}

static void dumpYTab(int* pTab)
{
  int i;
  int* py = pTab;

  printf("y dump\n");
  for (i = 0; i < 32; i++)
    printf("value of y[%2d] = %6d\n", i, py[i]);
  printf("\n");
}

static void dumpSampleData(TSampleData* pTab)
{
  int i;
  TSampleData* pd = pTab;

  printf("sample data dump\n");
  for (i = 0; i < 16; i++)
    printf("scale_factor[ ][ ] = %2d   bits[][] = %2d   max[][] = %2d\n", pd[i].scale_factor, pd[i].bits, pd[i].calc1);
  printf("\n");
}

static void dumpInputBuffer(short* pBuffer, int index)
{
  int i;
  short* pb = pBuffer;

  printf("in buffer dump\n");
  for (i = index; i < index + 16; i++)
    printf("in[%2d] = %6d\n", i - index, pb[i]);
  printf("\n");
}
#endif

#endif /*F_BT_MPA_A2DP_SOURCE*/
