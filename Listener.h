#include <tisudshl.h>  


class CListener : public DShowLib::GrabberListener
{
public:
	// Increments the number of processed frames
	virtual void frameReady(DShowLib::Grabber& caller, smart_ptr<DShowLib::MemBuffer> pBuffer, DWORD FrameNumber);

	// Number of processed frames
	size_t done;
};