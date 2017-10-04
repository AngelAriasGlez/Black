#ifndef _ADJ_MADCODEC_H
#define _ADJ_MADCODEC_H

/*#include "../utils/File.hpp"


extern "C" {

	#include <mad.h>
	#include <id3tag.h>
}
#include "Codec.hpp"
#include <vector>





	#define READLENGTH 5000

	// Struct used to store mad frames for seeking
	typedef struct MadSeekFrameType {
		unsigned char *m_pStreamPos;
		long int pos;
	} MadSeekFrameType;

	class MadCodec : public Codec{
	public:


		MadCodec();
		~MadCodec();
		
		bool _open();
		int _read(void* pointer, int frameCount);
		void _seek(int frameNumber);

	private:
		FileMap *mFile;

		unsigned long discard(unsigned long size);
		inline long unsigned length();

	// Returns the position of the frame which was found. The found frame is set to the current element in m_qSeekList
		int findFrame(int pos);
		// Scale the mad sample to be in 16 bit range.
		inline signed int madScale (mad_fixed_t sample);
		MadSeekFrameType* getSeekFrame(long frameIndex) const;

		// Returns true if the loaded file is valid and usable to read audio.
		bool isValid() const;

		int bitrate;
		int framecount;
		int currentframe;
		// current play position.
		mad_timer_t pos;
		mad_timer_t filelength;
		mad_stream *Stream;
		mad_frame *Frame;
		mad_synth *Synth;
		unsigned inputbuf_len;
		unsigned char *inputbuf;

		// Start index in Synth buffer of samples left over from previous call to read
		int rest;
		// Number of channels in file
		int m_iChannels;

		// It is not possible to make a precise seek in an mp3 file without decoding the whole stream.
        // To have precise seek within a limited range from the current decode position, we keep track
        // of past decodeded frame, and their exact position. If a seek occours and it is within the
        // range of frames we keep track of a precise seek occours, otherwise an unprecise seek is performed
        
		std::vector<MadSeekFrameType*> m_qSeekList;
		// Index iterator for m_qSeekList. Helps us keep track of where we are in the file.
		long m_currentSeekFrameIndex;
		// Average frame size used when searching for a frame
		int m_iAvgFrameSize;


	};
*/

#endif



