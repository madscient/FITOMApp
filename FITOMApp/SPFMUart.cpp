#include "stdafx.h"
#include "SPFMUart.h"
#include <wiringPi.h>
#include <wiringSerial.h>


CSPFMUart::CSPFMUart(const char* param)
{
	hUart = serialOpen(param, 1500000);
}

CSPFMUart::~CSPFMUart()
{
	serialClose(hUart);
}

int CSPFMUart::Init()
{
	int ret = -1;
	int res[2];
	serialPutchar(hUart, 0xff);	//Inqury
	while (!serialDataAvail(hUart));
	res[0] = serialGetchar(hUart);
	res[1] = serialGetchar(hUart);
	if (res[0] == 'L' && res[1] == 'T') {
		serialPutchar(hUart, 0xfe);	//Reset
		while (!serialDataAvail(hUart));
		res[0] = serialGetchar(hUart);
		res[1] = serialGetchar(hUart);
		if (res[0] == 'O' && res[1] == 'K') {
			ret = 0;
		}
	}
	return ret;
}

void CSPFMUart::write(int sl, int addr, int data)
{
	serialPutchar(hUart, sl);
	serialPutchar(hUart, addr);
	serialPutchar(hUart, data);
	serialFlush(hUart);
}

int CSPFMUart::read(int sl, int addr)
{
	return 0;
}
