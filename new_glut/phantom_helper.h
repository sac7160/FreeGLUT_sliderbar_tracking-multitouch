#ifndef __PHANTOM_HELPER_H__
#define __PHANTOM_HELPER_H__

#include <HD/hd.h>
#include <HL/hl.h>
#include <HDU/hduError.h>
#include <HDU/hduVector.h>

#include <iostream>



namespace PHANTOM_TOOLS 
{
	void adjust_force();
	void initHD();
	void exitHandler();
}

HDCallbackCode HDCALLBACK DeviceStateCallback(void* data);


#endif	//__PHANTOM_HELPER_H__