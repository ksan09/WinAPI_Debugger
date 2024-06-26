#pragma once
#include "Object.h"
#include "Player.h"

class PlayerDirCollider :
    public Object
{
public:
    PlayerDirCollider();
    ~PlayerDirCollider();
public:
    virtual void Update();
    virtual void EnterCollision(Collider* _pOther) override;
    virtual void ExitCollision(Collider* _pOther) override;
public:
    void SetCollider(DIR state, Vec2 scale, Vec2 offset);
private:
    void BlockCheck();
    void JumpAbleObjectCheck();
private:
    void BlockCheckOut();
private:
    DIR m_eState;
    Player* m_pOwner;
    friend class Player;
};