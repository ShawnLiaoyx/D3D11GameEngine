#include "stdafx.h"
#include "components\player.h"
#include "animation.h"
#include "game.h"
#include "jsonUtil.h"
#include "physics.h"
#include "skinnedObj.h"
#include "stringUtil.h"


static const float s_accel = 900.0f;
static const float s_moveSpeed = 300.0f;
static const float s_turnSpeed = Math::ToRadians(720.0f);
static const float s_gravity = 500.0f;
static const float s_jumpStart = 300.0f;


Player::Player(SkinnedObj*pObj, Game* pGame)
    : Character(pObj, pGame)
	, mMoveSpeed(0.0f)
    , mState(State::UNKNOWN)
    , mVel(Vector3::Zero)
    , mPrevAnimTime(0.0f)
{
	Matrix4 mat = mRenderObj->mConstant.data;
    mPos = mat.GetTranslation();
    Vector3 fwd = mat.GetXAxis();
	mHeading = atan2f(fwd.y, fwd.x);
}

void Player::Update(float deltaTime)
{
	Vector3 move = Vector3::Zero;
	if (mGame->IsKeyHeld(VK_UP) || mGame->IsKeyHeld('W'))
		move.x += 1.0f;
	if (mGame->IsKeyHeld(VK_DOWN) || mGame->IsKeyHeld('S'))
		move.x -= 1.0f;
	if (mGame->IsKeyHeld(VK_RIGHT) || mGame->IsKeyHeld('D'))
		move.y += 1.0f;
	if (mGame->IsKeyHeld(VK_LEFT) || mGame->IsKeyHeld('A'))
		move.y -= 1.0f;

	float throttle = move.Length();
	if (throttle > 1.0f)
		throttle = 1.0f;

	if (throttle > 0.0f)
	{	// turn to heading
		float heading = atan2f(move.y, move.x);
		float angDelta = heading - mHeading;
		if (angDelta > Math::Pi)		
			angDelta -= Math::TwoPi;
		if (angDelta < -Math::Pi)
			angDelta += Math::TwoPi;
		float angSpd = s_turnSpeed * deltaTime;
		angDelta = Math::Clamp(angDelta, -angSpd, angSpd);
		mHeading += angDelta;
		if (mHeading > Math::Pi)		
			mHeading -= Math::TwoPi;
		if (mHeading < -Math::Pi)
			mHeading += Math::TwoPi;
	}

	{	// accelerate
		float speed = throttle * s_moveSpeed;
		float spdDelta = speed - mMoveSpeed;
		float accel = s_accel * deltaTime;
		spdDelta = Math::Clamp(spdDelta, -accel, accel);
		mMoveSpeed += spdDelta;
	}

    mPrevAnimTime = currentTime;
    Character::Update(deltaTime);

    ChangeState();
    UpdateState(deltaTime);
}

bool Player::CheckGround(float* groundHeight)
{
    Vector3 hitPoint;
    bool isOver = mGame->GetPhysics()->RayCast(Physics::LineSegment(Vector3(mPos.x, mPos.y, mPos.z + 10.0f), 
        Vector3(mPos.x, mPos.y, mPos.z - 10.0f)), &hitPoint);
    if (isOver) {
        *groundHeight = hitPoint.z;
    }
    return isOver;
}

void Player::SetState(State newState)
{
    switch (newState)
    {
    case State::IDLE:
        SetAnim("idle");
        mVel.z = 0.0f;
        break;
    case State::FALL:
        SetAnim("fall");
        break;
    case State::WALK:
        SetAnim("run");
        mVel.z = 0.0f;
        break;
    case State::JUMP:
        SetAnim("jumpStart");
        mVel.z = s_jumpStart;
        break;
    case State::LAND:
        SetAnim("land");
        mVel.z = 0.0f;
        break;
    }
    mState = newState;
}

void Player::ChangeState()
{
    float groundHeight = 0.0f;
    bool isOverGround = CheckGround(&groundHeight);
    bool isOnGround = isOverGround && mPos.z <= groundHeight;

    switch (mState)
    {
    case State::UNKNOWN:
        SetState(State::IDLE);
        break;
    case State::LAND:
        if (currentTime < mPrevAnimTime)
            SetState(State::IDLE);
    case State::IDLE:
        if (mGame->IsKeyHeld(VK_SPACE))
            SetState(State::JUMP);
        else if (false == isOnGround)
            SetState(State::FALL);
        else if (mVel.LengthSq() > 0.0001f)
            SetState(State::WALK);
        break;
    case State::WALK:
        if (mGame->IsKeyHeld(VK_SPACE))
            SetState(State::JUMP);
        else if (false == isOnGround)
            SetState(State::FALL);
        else if (mVel.LengthSq() < 0.0001f)
            SetState(State::IDLE);
        break;
    case State::JUMP:
        if (currentTime < mPrevAnimTime)
            SetAnim("jump");
    case State::FALL:
        if (mVel.z < 0.0f && isOnGround)
        {
            mPos.z = groundHeight;
            SetState(State::LAND);
        }
        break;
    }
}

void Player::UpdateState(float deltaTime)
{
    Matrix4 mat = Matrix4::CreateRotationZ(mHeading);
    Vector3 vel = mMoveSpeed * mat.GetXAxis();
    mVel.x = vel.x;
    mVel.y = vel.y;

    switch (mState)
    {
    case State::IDLE:
    case State::LAND:
        break;
    case State::WALK:
        break;
    case State::FALL:
    case State::JUMP:
        mVel.z -= s_gravity * deltaTime;
        break;
    }

    mPos += mVel * deltaTime;
    mRenderObj->mConstant.data = mat
        * Matrix4::CreateTranslation(mPos);
}
