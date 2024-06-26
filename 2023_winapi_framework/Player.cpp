#include "pch.h"
#include "Player.h"
#include "KeyMgr.h"
#include "TimeMgr.h"
#include "Object.h"
#include "PlayerDirCollider.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "Texture.h"
#include "PathMgr.h"
#include "ResMgr.h"
#include "EventMgr.h"
#include "Collider.h"
#include "Animator.h"
#include "Animation.h"
#include "Rigidbody2D.h"
#include "DefaultMonster.h"
#include "Bullet.h"
#include "Spike.h"
#include "WallSpike.h"
#include "Core.h"

Player::Player()
	: m_pTex(nullptr)
	, m_isJump(false)
	, m_isDoubleJump(false)
	, m_isRight(true)
	, m_isDie(false)
	, m_isPlayDieAnim(false)
	, m_isSlowMove(false)
	, m_fCurTime(0.f)
{

	SetName(L"Player");

	m_pTex = ResMgr::GetInst()->TexLoad(L"Player", L"Texture\\player.bmp");
	CreateCollider();
	GetCollider()->SetScale(Vec2(16.f,30.f));
	GetCollider()->SetOffSetPos(Vec2(0.f, 4.f));
	// 콜라이더 끝부분마다 감지용 콜라이더 오브젝트 4개 생성해서 감지하기
#pragma region Create PlayerDirCol
	PlayerDirCollider* m_pDirLeftCol = new PlayerDirCollider;
	m_pDirLeftCol->m_pOwner = this;
	m_pDirLeftCol->SetCollider(DIR::LEFT, { 8.f, 42.f }, { -10.5f, 8.f });

	PlayerDirCollider* m_pDirRightCol = new PlayerDirCollider;
	m_pDirRightCol->m_pOwner = this;
	m_pDirRightCol->SetCollider(DIR::RIGHT, { 8.f, 42.f }, { 10.5f, 8.f });

	PlayerDirCollider* m_pDirTopCol = new PlayerDirCollider;
	m_pDirTopCol->m_pOwner = this;
	m_pDirTopCol->SetCollider(DIR::TOP, { 15.5f, 20.f }, { 0.f, -10.f });

	PlayerDirCollider* m_pDirBottomCol = new PlayerDirCollider;
	m_pDirBottomCol->m_pOwner = this;
	m_pDirBottomCol->SetCollider(DIR::BOTTOM, { 15.5f, 18.f }, { 0.f, 25.f });

	SceneMgr::GetInst()->GetCurScene()->AddObject(m_pDirLeftCol, OBJECT_GROUP::PLAYER_DIR_COL);
	SceneMgr::GetInst()->GetCurScene()->AddObject(m_pDirRightCol, OBJECT_GROUP::PLAYER_DIR_COL);
	SceneMgr::GetInst()->GetCurScene()->AddObject(m_pDirTopCol, OBJECT_GROUP::PLAYER_DIR_COL);
	SceneMgr::GetInst()->GetCurScene()->AddObject(m_pDirBottomCol, OBJECT_GROUP::PLAYER_DIR_COL);
#pragma endregion

#pragma region Create Animator
	float size = 64.f;
	float leftStartX = size * 6;

	CreateAnimator();
	//
	GetAnimator()->CreateAnim(L"Player_Die_Left", m_pTex, Vec2(0.f, size * 3),
		Vec2(size, size), Vec2(size, 0.f), 5, 0.1f);
	GetAnimator()->CreateAnim(L"Player_Die_Right", m_pTex, Vec2(leftStartX, size * 3),
		Vec2(size, size), Vec2(size, 0.f), 5, 0.1f);
	//
	GetAnimator()->CreateAnim(L"Player_Revive_Right", m_pTex, Vec2(0.f, size * 4),
		Vec2(size, size), Vec2(size, 0.f), 5, 0.1f);
	GetAnimator()->CreateAnim(L"Player_Revive_Left", m_pTex, Vec2(leftStartX, size * 4),
		Vec2(size, size), Vec2(size, 0.f), 5, 0.1f);
	//
	GetAnimator()->CreateAnim(L"Player_DoubleJump_Right", m_pTex, Vec2(0.f, size * 6),
		Vec2(size, size), Vec2(size, 0.f), 4, 0.05f);
	GetAnimator()->CreateAnim(L"Player_DoubleJump_Left", m_pTex, Vec2(leftStartX, size * 6),
		Vec2(size, size), Vec2(size, 0.f), 4, 0.05f);
	//
	GetAnimator()->CreateAnim(L"Player_Idle_Left", m_pTex, Vec2(0.f, 0.f),
		Vec2(size, size), Vec2(size, 0.f), 4, 0.2f);
	GetAnimator()->CreateAnim(L"Player_Idle_Right", m_pTex, Vec2(leftStartX, 0.f),
		Vec2(size, size), Vec2(size, 0.f), 4, 0.2f);
	//
	GetAnimator()->CreateAnim(L"Player_SlowMove_Left", m_pTex, Vec2(0.f, size * 2),
		Vec2(size, size), Vec2(size, 0.f), 4, 0.2f);
	GetAnimator()->CreateAnim(L"Player_SlowMove_Right", m_pTex, Vec2(leftStartX, size * 2),
		Vec2(size, size), Vec2(size, 0.f), 4, 0.2f);
	//
	GetAnimator()->CreateAnim(L"Player_Move_Left", m_pTex, Vec2(0.f, size),
		Vec2(size, size), Vec2(size, 0.f), 6, 0.1f);
	GetAnimator()->CreateAnim(L"Player_Move_Right", m_pTex, Vec2(leftStartX, size),
		Vec2(size, size), Vec2(size, 0.f), 6, 0.1f);
	//
	GetAnimator()->CreateAnim(L"Player_JumpUp_Left", m_pTex, Vec2(0.f, size * 5),
		Vec2(size, size), Vec2(size, 0.f), 1, 0.1f);
	GetAnimator()->CreateAnim(L"Player_JumpUp_Right", m_pTex, Vec2(leftStartX, size * 5),
		Vec2(size, size), Vec2(size, 0.f), 1, 0.1f);
	//
	GetAnimator()->CreateAnim(L"Player_FallDown_Left", m_pTex, Vec2(size, size * 5),
		Vec2(size, size), Vec2(size, 0.f), 1, 0.1f);
	GetAnimator()->CreateAnim(L"Player_FallDown_Right", m_pTex, Vec2(leftStartX + size, size * 5),
		Vec2(size, size), Vec2(size, 0.f), 1, 0.1f);
	//
	GetAnimator()->PlayAnim(L"Player_Idle_Right", false);
#pragma endregion

	ResMgr::GetInst()->LoadSound(L"Jump", L"Sound\\Jump.wav", false);
	ResMgr::GetInst()->LoadSound(L"StepJump", L"Sound\\StepJump.wav", false);
	ResMgr::GetInst()->LoadSound(L"StageChange", L"Sound\\StageChange.wav", false);
	ResMgr::GetInst()->LoadSound(L"Death", L"Sound\\Death.wav", false);

	CreateRigidbody2D();

	//// 오프셋 건드리기
	//Animation* pAnim = GetAnimator()->FindAnim(L"Jiwoo_Front");
	//// 하나만
	//pAnim->SetFrameOffset(0, Vec2(0.f, 20.f));

	//// 프레임 다 
	//for (size_t i = 0; i < pAnim->GetMaxFrame(); ++i)
	//	pAnim->SetFrameOffset(i, Vec2(0.f, 20.f));
}
Player::~Player()
{
	//if (m_pTex != nullptr)
	//	delete m_pTex;
}
void Player::Update()
{
	Rigidbody2D* pRb = GetRigidbody2D();
	Vec2 vVelo = pRb->GetVelocity();
	Vec2 vPos = GetPos();

	if (m_isDie)
	{
		m_fCurTime += fDT;
		if (m_fCurTime >= 1.f)
		{
			ResMgr::GetInst()->Play(L"StageChange");
			EventMgr::GetInst()->SceneChange(L"StageSelect_Scene");

		}
	}

#pragma region MoveInput
	m_isSlowMove = KEY_PRESS(KEY_TYPE::LSHIFT);

	if ( vPos.x - 16.f >= 0 &&
		(KEY_PRESS(KEY_TYPE::LEFT) || KEY_PRESS(KEY_TYPE::A)))
	{
		
		m_isRight = false;
		if(!m_isSlowMove || m_isJump)
			pRb->SetVelocity({ -250.f, vVelo.y });
		else
			pRb->SetVelocity({ -100.f, vVelo.y });
	}
	else if (vPos.x + 16.f <= WINDOW_WIDTH && 
		(KEY_PRESS(KEY_TYPE::RIGHT) || KEY_PRESS(KEY_TYPE::D)))
	{
		m_isRight = true;
		if (!m_isSlowMove || m_isJump)
			pRb->SetVelocity({ 250.f, vVelo.y });
		else
			pRb->SetVelocity({ 100.f, vVelo.y });
	}
	else
	{
		pRb->SetVelocity({ 0.f, vVelo.y });
	}

#pragma endregion

#pragma region Jump

	if (m_isJump == false && m_isDie == false
		&& (KEY_DOWN(KEY_TYPE::SPACE)
			|| KEY_DOWN(KEY_TYPE::W)
			|| KEY_DOWN(KEY_TYPE::UP)))
	{
		//
		
		//

		Jump();
	}
	else if (m_isJump == true && m_isDoubleJump == false && m_isDie == false
		&& (KEY_DOWN(KEY_TYPE::SPACE)
			|| KEY_DOWN(KEY_TYPE::W)
			|| KEY_DOWN(KEY_TYPE::UP)))
	{
		DoubleJump();
	}

	Vec2 pos = GetPos();
	if (pos.y >= WINDOW_HEIGHT - 128 - 6)
	{
		SetPos(Vec2(pos.x, WINDOW_HEIGHT - 128 - 16.f));
	}
#pragma endregion

#pragma region Animation
	// Animation
	vVelo = pRb->GetVelocity();
	if (m_isPlayDieAnim)
	{
		//None
		pRb->StopMoveLeft(true);
		pRb->StopMoveRight(true);
	}
	else if (m_isDie)
	{
		if(m_isRight)
			GetAnimator()->PlayAnim(L"Player_Die_Right", false);
		else
			GetAnimator()->PlayAnim(L"Player_Die_Left", false);
		m_isPlayDieAnim = true;
	}
	else if (m_isDoubleJump)
	{
		if (m_isRight)
		{
			GetAnimator()->PlayAnim(L"Player_DoubleJump_Right", false);
		}
		else
		{
			GetAnimator()->PlayAnim(L"Player_DoubleJump_Left", false);
		}
	}
	else if (m_isJump && vVelo.y > 0) // 점프
	{
		if (m_isRight)
		{
			GetAnimator()->PlayAnim(L"Player_FallDown_Right", false);
		}
		else
		{
			GetAnimator()->PlayAnim(L"Player_FallDown_Left", false);
		}
	}
	else if (m_isJump == false) // 움직임 또는 멈춤
	{
		if (vVelo.x == 0)
		{
			if (m_isRight)
			{
				GetAnimator()->PlayAnim(L"Player_Idle_Right", true);
			}
			else
			{
				GetAnimator()->PlayAnim(L"Player_Idle_Left", true);
			}
		}
		else
		{
			if (!m_isSlowMove)
			{
				if (m_isRight)
				{
					GetAnimator()->PlayAnim(L"Player_Move_Right", true);
				}
				else
				{
					GetAnimator()->PlayAnim(L"Player_Move_Left", true);
				}
			}
			else
			{
				if (m_isRight)
				{
					GetAnimator()->PlayAnim(L"Player_SlowMove_Right", true);
				}
				else
				{
					GetAnimator()->PlayAnim(L"Player_SlowMove_Left", true);
				}
			}
			
		}


	}
	else if (m_isJump == true)// 떨어지는 중
	{
		if (m_isRight)
		{
			GetAnimator()->PlayAnim(L"Player_JumpUp_Right", false);
		}
		else
		{
			GetAnimator()->PlayAnim(L"Player_JumpUp_Left", false);
		}
	}
#pragma endregion

	GetAnimator()->Update();
	GetRigidbody2D()->LateUpdate();
}

void Player::EnterCollision(Collider* _pOther)
{
	const Object* pOtherObj = _pOther->GetObj();
	wstring objName = pOtherObj->GetName();

	if (objName == L"DamageObject" || objName == L"DamageAndJumpAbleObject"
		|| objName == L"FallingBlock")
		if (GetDie() == false)
			Die();
}

void Player::Jump()
{
	if (GetDie())
		return;

	ResMgr::GetInst()->Play(L"Jump");
	Rigidbody2D* pRb = GetRigidbody2D();
	Vec2 vVelo = pRb->GetVelocity();

	m_isJump = true;
	pRb->SetVelocity({vVelo.x, -600.f});
}

void Player::Die()
{
	if (m_isDie) return;

	ResMgr::GetInst()->Play(L"Death");
	// 아직 안 만듬
	GetRigidbody2D()->SetVelocity({ 0.f, -300.f });
	Core::GetInst()->Shake(0.2f, 2.f);
	m_isDie = true;
}

void Player::DoubleJump()
{
	ResMgr::GetInst()->Play(L"Jump");
	Rigidbody2D* pRb = GetRigidbody2D();
	Vec2 vVelo = pRb->GetVelocity();

	m_isJump = true;
	m_isDoubleJump = true;
	pRb->SetVelocity({ vVelo.x, -500.f });
}

void Player::Render(HDC _dc)
{
	//Vec2 vPos = GetPos();
	//Vec2 vScale = GetScale();
	//int Width = m_pTex->GetWidth();
	//int Height = m_pTex->GetHeight();
	//// 1. 기본 옮기기
	//BitBlt(_dc
	//	,(int)(vPos.x - vScale.x /2)
	//	,(int)(vPos.y - vScale.y /2)
	//	, Width,Height, m_pTex->GetDC()
	//	,0,0,SRCCOPY);

	//// 2. 색상 걷어내기
	//TransparentBlt(_dc
	//	, (int)(vPos.x - vScale.x / 2)
	//	, (int)(vPos.y - vScale.y / 2)
	//	, Width, Height, m_pTex->GetDC()
	//	, 0, 0, Width,Height, RGB(255,0,255));

	//// 3. 확대 및 축소
	//StretchBlt(_dc
	//	, (int)(vPos.x - vScale.x / 2)
	//	, (int)(vPos.y - vScale.y / 2)
	//	, Width ,Height, m_pTex->GetDC()
	//	, 0, 0, Width, Height, SRCCOPY);

	// 4. 회전
	// 삼각함수, 회전행렬
	//Plgblt(_dc
	//	, (int)(vPos.x - vScale.x / 2)
	//	, (int)(vPos.y - vScale.y / 2)
	//	, Width, Height, m_pTex->GetDC()
	//	, 0, 0, Width, Height, RGB(255, 0, 255));
	Component_Render(_dc);
}
