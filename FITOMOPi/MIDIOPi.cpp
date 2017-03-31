#include "stdafx.h"
#include "MIDIDEV.h"
#include <wiringPi.h>
#include <wiringSerial.h>

CUartMidiIn::CUartMidiIn(const char* param)
{
	hUart = serialOpen(param, 312500);
}

CUartMidiIn::~CUartMidiIn()
{
	serialClose(hUart);
}

UINT8 CUartMidiIn::IsReceived()
{
	return (UINT8)serialDataAvail(hUart);
}

UINT8 CUartMidiIn::Read()
{
	return (UINT8)serialGetchar(hUart);
}
