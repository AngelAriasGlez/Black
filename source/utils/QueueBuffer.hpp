#ifndef QUEUEBUFFER_H
#define QUEUEBUFFER_H




class QueueBuffer {
public:
	QueueBuffer() {
		m_capacity = 256;
		m_size = 0;

		m_buffer = (unsigned char*)malloc(m_capacity);
	}

	~QueueBuffer() {
		m_buffer = (unsigned char*)realloc(m_buffer, 0);
	}

	int getSize() {
		return m_size;
	}

	void write(const void* buffer, int size) {
		if(size + m_size > m_capacity)
			reserve(size);

		memcpy(m_buffer + m_size, buffer, size);
		m_size += size;
	}

	void reserve(int size){
		bool need_realloc = false;
		while (size + m_size > m_capacity) {
			m_capacity *= 2;
			need_realloc = true;
		}

		if (need_realloc) {
			m_buffer = (unsigned char*)realloc(m_buffer, m_capacity);
		}	
	}

	int read(void* buffer, int size) {
		int to_read = std::min(size, m_size);
		memcpy(buffer, m_buffer, to_read);
		memmove(m_buffer, m_buffer + to_read, m_size - to_read);
		m_size -= to_read;
		return to_read;
	}
	int ltrim(int size){
		int to_read = std::min(size, m_size);
		memmove(m_buffer, m_buffer + to_read, m_size - to_read);
		m_size -= to_read;		
		return to_read;
	}
	void clear() {
		m_size = 0;
	}
	const void* getPointer(){
		return m_buffer;
	}

private:
	unsigned char* m_buffer;
	int m_capacity;
	int m_size;

	// private and unimplemented to prevent their use
	QueueBuffer(const QueueBuffer&);
	QueueBuffer& operator=(const QueueBuffer&);
};


#endif