#pragma once

class CSPFMUart
{
protected:
	int hUart;
public:
	CSPFMUart(const char* param);
	~CSPFMUart();
	int Init();
	void write(int sl, int addr, int data);
	int read(int sl, int addr);
};

