#pragma once

class CMasterVolumeCtrl
{
protected:
	uint32_t volume;
	BOOL valid;
	virtual void UpdateVolume();
public:
	CMasterVolumeCtrl() {};
	~CMasterVolumeCtrl();
	void SetVolume(uint32_t vol);
	uint32_t GetVolume();
	BOOL IsValid() { return valid; };
};
