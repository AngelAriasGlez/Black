#ifndef _ADJ_WAVCODEC_H
#define _ADJ_WAVCODEC_H

#include <string>

#include "../utils/File.hpp"
#include "Codec.hpp"

	class WavCodec : public Codec{
	public:
		struct Riff
		{
			char                RIFF[4];        /* RIFF Header      */ //Magic header
			unsigned int       ChunkSize;      /* RIFF Chunk Size  */
		};
		struct Wave
		{			
			char                WAVE[4];        /* WAVE Header      */
			char                fmt[4];         /* FMT header       */
			unsigned int       Subchunk1Size;  /* Size of the fmt chunk                                */
			unsigned short      AudioFormat;    /* Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM */
			unsigned short      NumOfChan;      /* Number of channels 1=Mono 2=Sterio                   */
			unsigned int       SamplesPerSec;  /* Sampling Frequency in Hz                             */
			unsigned int       bytesPerSec;    /* bytes per second */
			unsigned short      blockAlign;     /* 2=16-bit mono, 4=16-bit stereo */
			unsigned short      bitsPerSample;  /* Number of bits per sample      */
			
			unsigned short      ExtSize;		/*Size of the extension*/
			
			unsigned short      ValidBitsPerSample;		/*Number of valid bits*/
			unsigned int		ChannelMask;			/*Speaker position mask*/
			char				SubFormat[16]	;				/*GUID, including the data format code*/

		};
		struct Fact
		{
			char                FACT[4];		/* Fact Header      */ 
			unsigned int       ChunkSize;		/* Fact Chunk Size  */
			unsigned int       SampleLength;	/* Number of samples (per channel) */
		};
		struct Data
		{
			char                Subchunk2ID[4]; /* "data"  string   */
			unsigned int       Subchunk2Size;  /* Sampled data length    */
		};


		WavCodec();
		~WavCodec();
		
		bool _open();
		int _read(void* pointer, int frameCount);
		void _seek(int frameNumber);

		int mHeaderSize;

		FileStream *mFile;
	};


#endif



