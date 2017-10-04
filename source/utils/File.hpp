#ifndef _ADJ_FILESTREAM_H
#define _ADJ_FILESTREAM_H

#include <string>
#include <fstream>
#include "../core/String.hpp"
#include "../utils/Log.hpp"

	class File{
	public:
		virtual ~File(){}
		virtual bool isOpen() = 0;
		virtual int read(void* pointer, int bytes, bool backward)= 0;
		virtual int read(void* pointer, int bytes)= 0;
		virtual void seek(int bytes) = 0;
		virtual int position()= 0;
		virtual int getSize()= 0;
		virtual bool eof() = 0;
		virtual bool fail() = 0;
		virtual std::string getFilename() = 0;
	};
	class FileMap : public  File{
		
		char *_buffer;
		bool _open;
		bool _eof;
		int _pos;
		int _size;

		std::string _filename;
	public:
		FileMap(std::string filename){
			_filename = filename;

			_pos = 0;
			_size = 0;
			_eof = false;



			std::fstream fs(filename.c_str(), std::ios::binary | std::ios::in | std::ios::ate);
			if(!fs.is_open()){
				LOGE("FileStream: Failed to open '%s'", filename.c_str());
				
				_open = false;
			}else{
				_size  = fs.tellg();
				_buffer = (char *)malloc(_size);

				fs.seekg(0, std::ios::beg);
				fs.read(_buffer, _size);
				fs.close();
				_open = true;
			}
		}
		void *getPointer(){
			return _buffer;
		}
		int getSize(){
			return _size;
		}
		~FileMap(){
			
			free(_buffer);
		}
		bool isOpen(){
			return _open;
		}

		int read(void* pointer, int bytes, bool backward){

			if(!backward){
				if(_pos+bytes >= _size){
					bytes = _size-_pos;
				}
			}else{
				if(_pos-bytes < 0){
					bytes = _pos;
				}
				_pos -= bytes;
			}
			memcpy(pointer, &_buffer[_pos/sizeof(char)], bytes);
			if(!backward){
				_pos += bytes;
			}else{
				bytes *= -1;
			}
			return bytes;		

		}
		int read(void* pointer, int bytes){
			return read(pointer, bytes, false);
		}
		void seek(int bytes){
			if(bytes < 0){
				bytes = 0;
			}else if(bytes > _size){
				_pos = _size;
			}else{
				_pos = bytes;
			}
		}
		int position(){
			return _pos;
		}
		bool eof(){
			return _eof;
		}
		bool fail(){
			return _eof;
		}
		std::string getFilename(){
			return _filename;
		}
	};
	class FileStream : File{
	public:
		FileStream(std::string filename){
			mFilename = filename;
			mFileStream = new std::fstream(filename.c_str(), std::ios::binary | std::ios::in | std::ios::ate);
			if(!mFileStream->is_open()){
				LOGE("FileStream: Failed to open '%s'", filename.c_str());
				mSize = 0;
			}else{
				mSize  = mFileStream->tellg();
				mFileStream->seekg(0, std::ios::beg);
			}
		}
		virtual ~FileStream(){
			mFileStream->close();
			delete mFileStream;
		}
		bool isOpen(){
			return mFileStream->is_open();
		}
		int read(void* pointer, int bytes, bool backward){
			if(backward)
			mFileStream->seekg(-bytes,std::ios_base::cur);
			mFileStream->read(reinterpret_cast<char *>(pointer), bytes);
			return (backward)?-mFileStream->gcount():mFileStream->gcount();
		}
		int read(void* pointer, int bytes){
			return read(pointer, bytes, false);
		}
		void seek(int bytes){
			//mFileStream->clear();
			mFileStream->seekg(bytes, std::ios::beg);
		}
		int position(){
			return (int)mFileStream->tellg();
		}
		bool eof(){
			return mFileStream->eof();
		}
		bool fail(){
			return mFileStream->fail();
		}
		int getSize(){
			return mSize;
		}
		std::string getFilename(){
			return mFilename;
		};
	private:
		int mSize;
		std::fstream* mFileStream;
		std::string mFilename;
	};

#endif
