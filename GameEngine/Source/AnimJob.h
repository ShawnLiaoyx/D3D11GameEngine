#pragma once
#include "Job.h"
class Character;

class AnimJob : public Job
{
public:
	AnimJob(Character* pCh);
	void DoIt() override;
private:
	Character* mCharacter;
};

