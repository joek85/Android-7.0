/******************************************************
* Project      : SBC
* FileName     : $RCSfile: sbcdec.c,v $
* Created      : April 2005
* Version      : $Revision: 1.10 $
* Last access  : $Date: 2005/10/27 06:06:23 $
* Archive      : $Source: /var/lib/cvs/sw/src/codec/sbc/sbcdec.c,v $
* Copyright    : (C) Stollmann E+V GmbH 2005
* Author       : Klaus Muehle (km@stollmann.de)
* Compiler     : ANSI C compatible
* Environment  : Windows NT 2000 user mode
* Contents     : implementation of SBC decoder
* Note         : 
* History      : 
*              : $Log: sbcdec.c,v $
*              : Revision 1.10  2005/10/27 06:06:23  lv
*              : fixed high level distortion problems
*              : various optimizations
*              : added sbcGetParams
*              :
*              : Revision 1.9  2005/10/26 08:06:40  jz
*              : moved constant data to "const" memory
*              :
*              : Revision 1.8  2005/07/28 10:14:54  jz
*              : Avoid castings
*              :
*              : Revision 1.7  2005/06/01 06:00:28  lv
*              : integrated internal headers
*              : fixed joint decoding bug
*              :
*              : Revision 1.6  2005/05/12 09:27:19  lv
*              : eliminated bracket warning
*              :
*              : Revision 1.5  2005/05/12 08:05:32  lv
*              : improved precision by using GUARD_HALFBIT
*              : non optimized fix for JOint distortion
*              :
*              : Revision 1.4  2005/04/27 07:58:17  lv
*              : changed internal sample representation from short to int
*              :
*              : Revision 1.3  2005/04/26 08:37:11  lv
*              : formatting
*              :
*              : Revision 1.2  2005/04/26 08:21:06  lv
*              : added crc creation
*              :
*              : Revision 1.1  2005/04/21 09:52:28  lv
*              : first checkin
*              :
*******************************************************
* sbcdec.c
******************************************************/

#if 1

#include <string.h>
#include <stdio.h>
#include "sbc.h"



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
  if (a > 1073709056)\
  a = 1073709056;\
  if (a < -1073741824)\
  a = -1073741824;
/* limit to 32767 << GUARD resp.  -32768 << GUARD */

/* sbc defines */
#define GUARD           15
#define GUARD_MINUS_2   13
#define GUARD_HALFBIT   16384   /* must equal (1 << (GUARD - 1)) */ 


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
  unsigned join;
} TSBCData;

typedef struct tagSampleData
{
  int bits;
  int scale_factor;
  int sb_sample;
#if defined F_SBC_DEBUG_FILE
  int qs;
#endif
} TSampleData;

/* global variables */
static TSBCData SBCData;
static TSampleData sampleData[SBC_MAX_CHANNELS + 1][SBC_MAX_SUBBANDS]; /* one additional line for terminator  */
static int v[2][200]; /* size 200 instead of 160 for optimized pointer management  */

/* global constants */
static const unsigned blockNumbers[4] = {4,  8,  12,  16};
static const unsigned subbandNumbers[2]   = {4,  8};
static const int offset4[4][4] = {{-1, 0, 0, 0}, {-2, 0, 0, 1}, {-2, 0, 0, 1}, {-2, 0, 0, 1}};
static const int offset8[4][8] = {{-2, 0, 0, 0, 0, 0, 0, 1}, {-3, 0, 0, 0, 0, 0, 1, 2}, {-4, 0, 0, 0, 0, 0, 1, 2}, {-4, 0, 0, 0, 0, 0, 1, 2}};

static const int divTable[32] = 
{
  0, 65536, 43691, 37449, 34953, 33825, 33288, 33026,
  32897, 32832, 32800, 32784, 32776, 32772, 32770, 32769,
  32769, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
};

static const signed char matrixingTable[20] = 
{
 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14, 17, 16, 19, 18, 1, 0
};

static const int d4[40] = 
{
  0, -70, -196, -358, -503, -510, -245, 401,
  -1430, -2679, -3785, -4220, -3392, -804, 3778, 10177,
  -17772, -25557, -32327, -36940, -38576, -36940, -32327, -25557,
  17772, 10177, 3778, -804, -3392, -4220, -3785, -2679,
  1430, 401, -245, -510, -503, -358, -196, -70,
};

static const int d8[80] = 
{
  0, -41, -90, -145, -527, -551, -523, -424,
  -1484, -2105, -2742, -3342, -3391, -2322, -767, 1289,
  -17826, -21754, -25579, -29150, -38523, -38113, -36898, -34934,
  17826, 13943, 10243, 6845, -3391, -4016, -4252, -4169,
  1484, 917, 432, 47, -527, -468, -387, -299,
  -216, -299, -387, -468, -236, 47, 432, 917,
  -3841, -4169, -4252, -4016, 3838, 6845, 10243, 13943,
  -32313, -34934, -36898, -38113, -32313, -29150, -25579, -21754,
  3838, 1289, -767, -2322, -3841, -3342, -2742, -2105,
  -236, -424, -523, -551, -216, -145, -90, -41,
};

static const int nik4[4][8] = 
{
  23170, 12540, 0, -12540, -23170, -30274, -32768, -30274,
  -23170, -30274, 0, 30274, 23170, -12540, -32768, -12540,
  -23170, 30274, 0, -30274, 23170, 12540, -32768, 12540,
  23170, -12540, 0, 12540, -23170, 30274, -32768, 30274,
};

static const int nik8[8][16] =
{
  23170, 18205, 12540, 6393, 0, -6393, -12540, -18205, -23170, -32138, -30274, -18205, 0, 18205, 30274, 32138,
  -23170, 6393, 30274, 27246, 0, -27246, -30274, -6393, 23170, 27246, -12540, -32138, 0, 32138, 12540, -27246,
  23170, -27246, -12540, 32138, 0, -32138, 12540, 27246, -23170, -6393, 30274, -27246, 0, 27246, -30274, 6393,
  -23170, 32138, -30274, 18205, 0, -18205, 30274, -32138, 23170, -18205, 12540, -6393, 0, 6393, -12540, 18205,
  -23170, -27246, -30274, -32138, -32768, -32138, -30274, -27246, 23170, 6393, -12540, -27246, -32768, -27246, -12540, 6393,
  23170, 32138, 12540, -18205, -32768, -18205, 12540, 32138, -23170, 18205, 30274, -6393, -32768, -6393, 30274, 18205,
  -23170, -18205, 30274, 6393, -32768, 6393, 30274, -18205, 23170, -32138, 12540, 18205, -32768, 18205, 12540, -32138,
  23170, -6393, -12540, 27246, -32768, 27246, -12540, -6393, -23170, 27246, -30274, 32138, -32768, 32138, -30274, 27246,
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


/* internal functions declarations */
static void initVTab(int* v);

static void processMatrixing4(const int* fnik, int* s, int* v, int iter);
static void calcAudioSamples4(int* v, const int* d4, short* x, int mono, int channels);
static void processMatrixing8(const int* nik, int* s, int* v, int iter);
static void calcAudioSamples8(int* v, const int* d8, short* x, int iter, int mono, int channels);

static int checkCRC(unsigned char* pInput);

static void computeBits(void);
static void computeSlicesInPoolStereo( int max_bitneed, int bitneed[SBC_MAX_CHANNELS][SBC_MAX_SUBBANDS]);

/**
* @brief		init sbc decoder
* 
* @param	
*
* @return
*
*/
void sbcInitDecoder(void)
{
	memset(&SBCData, 0, sizeof(SBCData));
	memset(&v[0][0], 0, 2*200*4);
	initVTab(v[0]);
	initVTab(v[1]);
}

/**
* @brief  sbc decode
* 
* @param  pInputBuffer
* @param  inputSize
* @param  pOutputBuffer
* @param  pOutputSize
*
* @return  
*
*/
int sbcDecode(unsigned char* pInputBuffer, int inputSize, unsigned char* pOutputBuffer, int* pOutputSize)
{
	unsigned ch;
	unsigned blk;
	unsigned int c;
	int frameSize;
	TSBCData* pSBCData = &SBCData;
	unsigned char* pIn = pInputBuffer;
	unsigned char* pOut = pOutputBuffer;

	/* read header */
	c = *pIn++;
	if (c != SBC_SYNCBYTE)
	{
		*pOutputSize = 0;
		return SBC_NO_SYNCBYTE;
	}

	c = *pIn++;
	pSBCData->sampling_frequency = (c >> 6) & 0x03;
	pSBCData->nrof_blocks = blockNumbers[(c >> 4) & 0x03];
	pSBCData->channel_mode = (c >> 2) & 0x03;
	pSBCData->allocation_method = (c >> 1) & 0x01;
	pSBCData->nrof_subbands = subbandNumbers[(c >> 0) & 0x01];
	pSBCData->bitpool = *pIn++;
	*pIn++; /* crc  */
	pSBCData->nrof_channels = pSBCData->channel_mode == SBC_MODE_MONO ? 1 : 2;

	/* check for sufficient input data */
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

		if (inputSize < frameSize)
			return SBC_NOT_ENOUGH_DATA;
	}

	/* check for sufficient output buffer size */ 
	{
		int ns = pSBCData->nrof_channels * pSBCData->nrof_blocks * pSBCData->nrof_subbands;

		if (*pOutputSize < ns * 2)
		{
			*pOutputSize = ns;
			return SBC_BUFFER_TOO_SMALL;
		}
	}

	/* check checksum */
	if (checkCRC(pInputBuffer) != 0)
	{
		*pOutputSize = 0;
		return SBC_CHECKSUM_ERROR;
	}

	if (pSBCData->channel_mode == SBC_MODE_JOINT)
	{
		pSBCData->join = *pIn++;
		/* filtering of join bits is not required if appropriate iteration is applied  */
		if (pSBCData->nrof_subbands == 4)
		{
			pSBCData->join &= 0xe0;
		}
		else
		{
			pSBCData->join &= 0xfe; 
		}
	}

	/* read scale factors */
	{
	TSampleData* pd = &sampleData[0][0];
	if (pSBCData->nrof_subbands == 4)
	{
	  if (pSBCData->channel_mode == SBC_MODE_JOINT)
	  {
	    /* data pointer adjustment required due to just 4 join bits in header  */
	    for (ch = 2; ch > 0; ch--)
	    {
	      c = pIn[-1];
	      pd[0].scale_factor = c & 0x0f;
	      c = pIn[0];
	      pd[1].scale_factor = c >> 4;
	      pd[2].scale_factor = c & 0x0f;
	      c = pIn[1];
	      pd[3].scale_factor = c >> 4;
	      pd  += 8;
	      pIn += 2;
	    }
	  } /* if joint */
	  else
	  {
	    for (ch = pSBCData->nrof_channels; ch > 0; ch--)
	    {
	      c = pIn[0];
	      pd[0].scale_factor = c >> 4;
	      pd[1].scale_factor = c & 0x0f;
	      c = pIn[1];
	      pd[2].scale_factor = c >> 4;
	      pd[3].scale_factor = c & 0x0f;
	      pd  += 8;
	      pIn += 2;
	    }
	  }
	} /* if 4 subbands */
	else
	{
	  for (ch = pSBCData->nrof_channels; ch > 0; ch--)
	  {
	    c = pIn[0];
	    pd[0].scale_factor = c >> 4;
	    pd[1].scale_factor = c & 0x0f;
	    c = pIn[1];
	    pd[2].scale_factor = c >> 4;
	    pd[3].scale_factor = c & 0x0f;
	    c = pIn[2];
	    pd[4].scale_factor = c >> 4;
	    pd[5].scale_factor = c & 0x0f;
	    c = pIn[3];
	    pd[6].scale_factor = c >> 4;
	    pd[7].scale_factor = c & 0x0f;
	    pd  += 8;
	    pIn += 4;
	  }
	}
	}

	computeBits();

	{
	int nrof_samples = pSBCData->nrof_channels * pSBCData->nrof_subbands;
	TSampleData* pd = &sampleData[0][0];
	int bits;
	unsigned int acc = 0;
	int resBits = 0;
	int sb = 0;

	if (pSBCData->nrof_subbands == 4)
	{
	  if (pSBCData->channel_mode == SBC_MODE_JOINT)
	  {
	    /* data pointer adjustment required due to just 4 join bits in header  */
	    resBits = 4;
	    acc = pIn[-1] & 0x0f;
	  }
	}

	if (pSBCData->nrof_channels == 2)
	{
	  if (pSBCData->nrof_subbands == 4)
	  {
	    /* move TSampleData structs into one row within sampleData array */
	    memcpy(&pd[4], &pd[8], 4 * sizeof(TSampleData));
	  }
	}

	/* set terminator */
	pd[nrof_samples].bits = SBC_TERMINATOR; /* terminator cannot be 1 because legal bit lengths of 1 may occur ! */

	/* reconstruct frequency subband samples */
	for (blk = pSBCData->nrof_blocks; blk > 0; blk--)
	{
	  pd = &sampleData[0][0];
	  for (;;)
	  {
	    /* advantages of this loop optimizing:
	    - testing only done if bits != 0, i.e. for less than half of the iterations
	    - no updating of a loop variable
	    - no use of a register
	    - no entry test */
	    if ((bits = (*pd++).bits) != 0)
	    {
	      int qs;
	      int msk;
	      /* break on terminator */
	      if (bits < 0)  /* arm assembler optimization; saves 3 cycles compared to bits == SBC_TERMINATOR ! */
	        break;
	      while (bits > resBits)
	      {
	        /* load 16 new bits into accumulator */
	        acc <<= 16;
	        acc |= *pIn++ << 8;
	        acc |= *pIn++;
	        resBits += 16;
	      }
	      resBits -= bits;
	      msk = (1 << bits) - 1;
	      qs = ((acc >> resBits) & msk);
#if defined F_SBC_DEBUG_FILE
	      pd[-1].qs = qs;
#endif
	      /* use value precomputed with divTable; this leads to a value guarded by GUARD + bits - scale_factor - 2 */
	      /* (msk >> 1) is a short form of (1 << (bits - ) - 1) */
	      pd[-1].sb_sample = ((qs - (msk >> 1)) * divTable[bits]) >> (GUARD_MINUS_2 + pd[-1].bits - pd[-1].scale_factor);
	      if (pd[-1].sb_sample < 0)
	        pd[-1].sb_sample++;

	    } /* if bits != 0 */
	    else
	    {
#if defined F_SBC_DEBUG_FILE
	      pd[-1].qs = 0;
#endif
	      pd[-1].sb_sample = 0;
	    }
	  } /* for samples */

	  /* decode joint samples */
	  if (pSBCData->channel_mode == SBC_MODE_JOINT)
	  {
	    int nsb;
	    pd = &sampleData[0][0];
	    nsb = pSBCData->nrof_subbands;
	    for (sb = nsb - 2; sb >= 0; sb--)
	    {
	      if (pSBCData->join & (0x80 >> sb))
	      {
	        int sbR = sb + nsb;
	        int temp = pd[sb].sb_sample;
	        pd[sb].sb_sample = temp + pd[sbR].sb_sample;
	        pd[sbR].sb_sample = temp - pd[sbR].sb_sample;
	      } /* if subband joint*/
	    } /* for sb */
	  } /* if joint */

	  /* reconstruct audio samples */
	  {
	    static int offset = 0;
	    int* pv;

	    if (offset <= 0)
	      offset = 180;
	    else
	      offset -= 20;

	    pv = &v[0][0] + offset;

	    if (pSBCData->nrof_subbands == 4)
	    {
	      processMatrixing4( &nik4[0][0], &sampleData[0][0].sb_sample, pv, pSBCData->nrof_channels);
	      calcAudioSamples4(pv, d4, (short*) pOut, pSBCData->channel_mode == SBC_MODE_MONO, pSBCData->nrof_channels);
	    }
	    else
	    {
	      processMatrixing8( &nik8[0][0], &sampleData[0][0].sb_sample, pv, pSBCData->nrof_channels);
	      calcAudioSamples8(pv, d8, (short*) pOut, 2, pSBCData->channel_mode == SBC_MODE_MONO, pSBCData->nrof_channels);
	    } /* if 4 subbands */

	    pOut += 2 * nrof_samples;

#if defined F_SBC_DEBUG_FILE
	    {
	      int sb, nsb;
	      short* pAudio;
	      pAudio = (short*) pOut;
	      nsb = pSBCData->nrof_subbands;
	      pd = &sampleData[0][0];
	      for (sb = 0; sb < nsb; sb++)
	      {
	        printf("%d, %d, %u, %d, %d\n",
	          pd[sb].bits, pd[sb].scale_factor, pd[sb].qs, pd[sb].sb_sample, pAudio[sb * 2]);
	        printf("%d, %d, %u, %d, %d\n",
	          pd[sb+8].bits, pd[sb+nsb].scale_factor, pd[sb+nsb].qs, pd[sb+nsb].sb_sample, pAudio[sb * 2 + 1]);
	      }  
	    }
#endif

	  }
	} /* for blk */

	  /* adjust read pointer */
	if (resBits >= 8)
	{
	  pIn--;
	  if (resBits >= 16)
	    pIn--;
	}

#if defined F_SBC_DEBUG_FILE
	if (pIn - pInputBuffer != frameSize)
	  printf("! decoded bytes mismatch: frameSize %d, read %d\n", frameSize, (pIn - pInputBuffer));
#endif

	}

	*pOutputSize = pOut - pOutputBuffer;
	return (pIn - pInputBuffer);
}

/**
* @brief		initialize v array
* 
* @param	v
*
* @return
*
*/
static void initVTab(int* v)
{
	int i;
	for (i = 0; i < 20; i++)
	{
		v[10 * i + 4] = (int) &v[10 * matrixingTable[i]]; /**< set pointer to successor group, first 4 */
		v[10 * i + 5 + 4] = (int) &v[10 * matrixingTable[i] + 5]; /**< set pointer to successor group, second 4 */
	}
}


/******************************* checkCRC *******************************/
/* */
/************************************************************************/
static int checkCRC(unsigned char* pInput)
{
  int bits = 0;
  unsigned char data[11];
  TSBCData* pSBCData = &SBCData;
  unsigned readCRC = pInput[3];
  unsigned crc = 0x0f;
  int i;
  unsigned octet;
  int len = 2;

  /* copy relevant data to local array */
  memcpy(data, pInput + 1, len); /* first two bytes */
  if (pSBCData->nrof_channels == 2)
    len *= 2;
  if (pSBCData->nrof_subbands == 8)
    len *= 2;
  if (pSBCData->channel_mode == SBC_MODE_JOINT)
    len += 1;
  memcpy(data + 2, pInput + 4, len); /* joint and scale bytes */
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

  if (crc != readCRC)
    return - 1;

  return 0; 
}


/****************************** processMatrixing4 **************************/
/**/
/************************************************************************/
static void processMatrixing4(const int* fnik, int* s, int* v, int iter)
{
  int i, si;
  int v0, v1, v2, v3, v4, v5, v6, v7;
  while (--iter >= 0)         /* for one or two channels */
  {
    const int * nik = fnik;
    v0 = v1 = v2 = v3 = v4 = v5 = v6 = v7 = GUARD_HALFBIT;
    for (i = 0; i < 4; i++)    /* iterate through samples */
    {
      int k = sizeof(TSampleData) / sizeof(int);    /* size of TSampleData in units of 'short' */
      si = s[i * k];        /* current frequency sample */
      if (si)     /* don't waste time on zero sample */
      {
        /* process 8 MACs */
        v0 += *nik++ * si;
        v1 += *nik++ * si;
        v2 += *nik++ * si;
        v3 += *nik++ * si;
        v4 += *nik++ * si;
        v5 += *nik++ * si;
        v6 += *nik++ * si;
        v7 += *nik++ * si;
      } /* if (si) */
      else
        nik += 8;
    } /* for i */
    /* fill v array with MAC results */
    *v++ = v0 >> GUARD;
    *v++ = v1 >> GUARD;
    *v++ = v2 >> GUARD;
    *v++ = v3 >> GUARD;
    v += 6;         /* skip fields reserved for pointer management and for 8 subbands */
    *v++ = v4 >> GUARD;
    *v++ = v5 >> GUARD;
    *v++ = v6 >> GUARD;
    *v++ = v7 >> GUARD;
    v += 6;         /* skip fields reserved for pointer management and for 8 subbands */

    /* adjust pointers for second channel */
    v -= 20;
    v += 200;
    s += ((sizeof(TSampleData) / sizeof(int)) << 2);
  } /* while iter */
}


/*************************** calcAudioSamples4 ****************************/
/**/
/************************************************************************/
static void calcAudioSamples4(int* v, const int* d4, short* x, int mono, int channels)
{
  int* pv;
  const int* pd;
  int x0,x1,x2,x3;

  while (--channels >= 0)
  {
    pd     = d4;
    pv     = v;

    x0 = *pd++ * pv[0] + GUARD_HALFBIT;
    x1 = *pd++ * pv[1] + GUARD_HALFBIT;
    x2 = *pd++ * pv[2] + GUARD_HALFBIT;
    x3 = *pd++ * pv[3] + GUARD_HALFBIT;
    pv = (int *)(pv[4]);
    x0 += *pd++ * pv[0];
    x1 += *pd++ * pv[1];
    x2 += *pd++ * pv[2];
    x3 += *pd++ * pv[3];
    pv = (int *)(pv[4]);
    x0 += *pd++ * pv[0];
    x1 += *pd++ * pv[1];
    x2 += *pd++ * pv[2];
    x3 += *pd++ * pv[3];
    pv = (int *)(pv[4]);
    x0 += *pd++ * pv[0];
    x1 += *pd++ * pv[1];
    x2 += *pd++ * pv[2];
    x3 += *pd++ * pv[3];
    pv = (int *)(pv[4]);
    x0 += *pd++ * pv[0];
    x1 += *pd++ * pv[1];
    x2 += *pd++ * pv[2];
    x3 += *pd++ * pv[3];
    pv = (int *)(pv[4]);
    x0 += *pd++ * pv[0];
    x1 += *pd++ * pv[1];
    x2 += *pd++ * pv[2];
    x3 += *pd++ * pv[3];
    pv = (int *)(pv[4]);
    x0 += *pd++ * pv[0];
    x1 += *pd++ * pv[1];
    x2 += *pd++ * pv[2];
    x3 += *pd++ * pv[3];
    pv = (int *)(pv[4]);
    x0 += *pd++ * pv[0];
    x1 += *pd++ * pv[1];
    x2 += *pd++ * pv[2];
    x3 += *pd++ * pv[3];
    pv = (int *)(pv[4]);
    x0 += *pd++ * pv[0];
    x1 += *pd++ * pv[1];
    x2 += *pd++ * pv[2];
    x3 += *pd++ * pv[3];
    pv = (int *)(pv[4]);
    x0 += *pd++ * pv[0];
    x1 += *pd++ * pv[1];
    x2 += *pd++ * pv[2];
    x3 += *pd++ * pv[3];

    limit(x0);
    limit(x1);
    limit(x2);
    limit(x3);

    if (mono)
    {
      /* store result in every output byte */
      x[0] = x0 >> GUARD;
      x[1] = x1 >> GUARD;
      x[2] = x2 >> GUARD;
      x[3] = x3 >> GUARD;
    }
    else
    {
      /* store result in every second output byte */
      x[0] = x0 >> GUARD;
      x[2] = x1 >> GUARD;
      x[4] = x2 >> GUARD;
      x[6] = x3 >> GUARD;
    }
    x += 1;    /* go from left to right channel */
    v += 200;  /* goto next v group */
  } /* while channels */
}


/****************************** processMatrixing8 **************************/
/**/
/************************************************************************/
static void processMatrixing8(const int* fnik, int* s, int* v, int iter)
{
  int i, si;
  int v0, v1, v2, v3, v4, v5, v6, v7;
  while (--iter >= 0)         /* for one or two channels */
  {
    const int* nik = fnik;
    v0 = v1 = v2 = v3 = v4 = v5 = v6 = v7 = GUARD_HALFBIT;    /* using 8 registers for speed */
    for (i = 0; i < 8; i++)    /* iterate through samples */
    {
      int k = sizeof(TSampleData) / sizeof(int);    /* size of TSampleData in units of 'int' */
      si = s[i * k];        /* current frequency sample */
      if (si)     /* don't waste time on zero sample */
      {
        /* process 8 MACs */
        v0 += *nik++ * si;
        v1 += *nik++ * si;
        v2 += *nik++ * si;
        v3 += *nik++ * si;
        v4 += *nik++ * si;
        v5 += *nik++ * si;
        v6 += *nik++ * si;
        v7 += *nik++ * si;
      } /* if (si) */
      else
        nik += 8;
    } /* for i */

    /* fill v array with MAC results */
    *v++ = v0 >> GUARD;
    *v++ = v1 >> GUARD;
    *v++ = v2 >> GUARD;
    *v++ = v3 >> GUARD;
    v++;         /* skip field reserved for pointer management */
    *v++ = v4 >> GUARD;
    *v++ = v5 >> GUARD;
    *v++ = v6 >> GUARD;
    *v++ = v7 >> GUARD;
    v++;        /* skip field reserved for pointer management */

    /* process the second group of 8 v fields */
    v0 = v1 = v2 = v3 = v4 = v5 = v6 = v7 = GUARD_HALFBIT;
    for (i = 0; i < 8; i++)
    {
      int k = sizeof(TSampleData) / sizeof(int);
      si = s[i * k];
      if (si)
      {
        v0 += *nik++ * si;
        v1 += *nik++ * si;
        v2 += *nik++ * si;
        v3 += *nik++ * si;
        v4 += *nik++ * si;
        v5 += *nik++ * si;
        v6 += *nik++ * si;
        v7 += *nik++ * si;
      } /* if */
      else
        nik += 8;
    } /* for */
    *v++ = v0 >> GUARD;
    *v++ = v1 >> GUARD;
    *v++ = v2 >> GUARD;
    *v++ = v3 >> GUARD;
    v++;
    *v++ = v4 >> GUARD;
    *v++ = v5 >> GUARD;
    *v++ = v6 >> GUARD;
    *v++ = v7 >> GUARD;
    v++;
    /* adjust pointers for second channel */
    v -= 20;
    v += 200;
    s += ((sizeof(TSampleData) / sizeof(int)) << 3);
  } /* while iter */
}


/*************************** calcAudioSamples8 ****************************/
/**/
/************************************************************************/
static void calcAudioSamples8(int* v, const int* d8, short* x, int iter, int mono, int channels)
{
  int* pv;
  const int*  pd;
  int x0, x1, x2, x3;

  while (--channels >= 0)
  {
    pd     = d8;
    pv     = v;
    while (--iter >= 0)
    {
      x0 = *pd++ * pv[0] + GUARD_HALFBIT;
      x1 = *pd++ * pv[1] + GUARD_HALFBIT;
      x2 = *pd++ * pv[2] + GUARD_HALFBIT;
      x3 = *pd++ * pv[3] + GUARD_HALFBIT;
      pv = (int *)(pv[4]);
      x0 += *pd++ * pv[0];
      x1 += *pd++ * pv[1];
      x2 += *pd++ * pv[2];
      x3 += *pd++ * pv[3];
      pv = (int *)(pv[4]);
      x0 += *pd++ * pv[0];
      x1 += *pd++ * pv[1];
      x2 += *pd++ * pv[2];
      x3 += *pd++ * pv[3];
      pv = (int *)(pv[4]);
      x0 += *pd++ * pv[0];
      x1 += *pd++ * pv[1];
      x2 += *pd++ * pv[2];
      x3 += *pd++ * pv[3];
      pv = (int *)(pv[4]);
      x0 += *pd++ * pv[0];
      x1 += *pd++ * pv[1];
      x2 += *pd++ * pv[2];
      x3 += *pd++ * pv[3];
      pv = (int *)(pv[4]);
      x0 += *pd++ * pv[0];
      x1 += *pd++ * pv[1];
      x2 += *pd++ * pv[2];
      x3 += *pd++ * pv[3];
      pv = (int *)(pv[4]);
      x0 += *pd++ * pv[0];
      x1 += *pd++ * pv[1];
      x2 += *pd++ * pv[2];
      x3 += *pd++ * pv[3];
      pv = (int *)(pv[4]);
      x0 += *pd++ * pv[0];
      x1 += *pd++ * pv[1];
      x2 += *pd++ * pv[2];
      x3 += *pd++ * pv[3];
      pv = (int *)(pv[4]);
      x0 += *pd++ * pv[0];
      x1 += *pd++ * pv[1];
      x2 += *pd++ * pv[2];
      x3 += *pd++ * pv[3];
      pv = (int *)(pv[4]);
      x0 += *pd++ * pv[0];
      x1 += *pd++ * pv[1];
      x2 += *pd++ * pv[2];
      x3 += *pd++ * pv[3];

      limit(x0);
      limit(x1);
      limit(x2);
      limit(x3);

      if (mono)
      {
        /* store result in every output byte */
        x[0] = x0 >> GUARD;
        x[1] = x1 >> GUARD;
        x[2] = x2 >> GUARD;
        x[3] = x3 >> GUARD;
      }
      else
      {
        /* store result in every second output byte */
        x[0] = x0 >> GUARD;
        x[2] = x1 >> GUARD;
        x[4] = x2 >> GUARD;
        x[6] = x3 >> GUARD;
      }
      x     += (mono ? 4 : 8); /* step to next group */
      pv     = v  + 5;
    } /* while iter */
    x -= 16;
    x += 1;    /* go from left to right channel */
    v += 200;  /* goto next v group */
    iter = 2;
  } /* while channels */
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

/*************************** sbcGetParams ********************************/
/**/
/************************************************************************/
int sbcGetParams(unsigned char* pInputBuffer, int inputSize, TSBCParams* pParams)
{
	unsigned int c;
	unsigned char* pIn = pInputBuffer;

	if (inputSize < 3)
		return SBC_NOT_ENOUGH_DATA;

	/* read header */
	c = *pIn++;
	if (c != SBC_SYNCBYTE)
		return SBC_NO_SYNCBYTE;

	c = *pIn++;
	pParams->samplingFrequency = (c >> 6) & 0x03;
	pParams->blockNumber = (c >> 4) & 0x03;
	pParams->channelMode = (c >> 2) & 0x03;
	pParams->allocMethod = (c >> 1) & 0x01;
	pParams->subbandNumber = (c >> 0) & 0x01;
	pParams->bitpool = *pIn++;

	return SBC_SUCCESS;	
}

#endif /*F_BT_MPA_A2DP_SINK*/
