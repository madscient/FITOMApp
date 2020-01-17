#pragma once

class CMasterVolumeCtrl
{
protected:
	uint32_t volume;
	bool valid;
	virtual void UpdateVolume() = 0;
public:
	CMasterVolumeCtrl();
	~CMasterVolumeCtrl();
	void SetVolume(uint32_t vol);
	uint32_t GetVolume();
	bool IsValid() { return valid; };
};

class CMasVolDummy : public CMasterVolumeCtrl
{
protected:
	virtual void UpdateVolume() {};
public:
	CMasVolDummy() : CMasterVolumeCtrl() {};
};
