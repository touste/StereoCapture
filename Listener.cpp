#include "stdafx.h"
#include <string>
#include "Listener.h"  


using namespace DShowLib;


// Increments the number of processed frames
void    CListener::frameReady(Grabber& caller, smart_ptr<MemBuffer> pBuffer, DWORD currFrame)
{
	MessageBeep(-1);
	done++;
}

