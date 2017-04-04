#pragma once
class CMasterVolumeCtrl
{
protected:
	uint32_t volume;
	virtual void UpdateVolume();
public:
	CMasterVolumeCtrl(const char* param);
	~CMasterVolumeCtrl();
	void SetVolume(uint32_t vol);
	uint32_t GetVolume();
};

