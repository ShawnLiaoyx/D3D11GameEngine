#pragma once
#include "engineMath.h"
#include "Components/character.h"

class Game;

class Player : public Character
{
public:
    Player(SkinnedObj* pObj, Game* pGame);

    void Update(float deltaTime) override;

protected:
    enum class State
    {
        UNKNOWN,
        IDLE,
        FALL,
        WALK,
        JUMP,
        LAND,
    };

    bool CheckGround(float* groundHeight);
    void SetState(State newState);
    void ChangeState();
    void UpdateState(float deltaTime);

    State mState;
	float mMoveSpeed;
	float mHeading;
    Vector3 mPos;
    Vector3 mVel;
    float mPrevAnimTime;
};