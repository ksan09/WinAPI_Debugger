#include "pch.h"
#include "Boss2.h"
#include "ResMgr.h"
#include "Texture.h"
#include "Animator.h"
#include "Node.h"
#include "Boss2Node.h"
#include "Collider.h"

#include "DefaultMonster.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "KeyMgr.h"
#include "TimeMgr.h"
#include "EventMgr.h"
#include "SaveLoadMgr.h"

Boss2::Boss2(Object* target)
	: m_iHp(3)
	, m_pTarget(target)
	, m_iCurTime(0)
	, m_iDamageDelayTime(1.5f)
	, m_isDamage(false)
{

#pragma region Collider
	CreateCollider();
	GetCollider()->SetScale({ 32.f, 32.f });
	GetCollider()->SetOffSetPos({ 0.f, -16.f });
	SetName(L"DamageAndJumpAbleObject");
#pragma endregion

#pragma region Animator
	float sizeX = 96.f, sizeY = 96.f;

	m_pTex = ResMgr::GetInst()->TexLoad(L"Boss2", L"Texture\\Boss2.bmp");

	CreateAnimator();
	//
	GetAnimator()->CreateAnim(L"Boss2_Idle", m_pTex, Vec2(0.f, 0.f),
		Vec2(sizeX, sizeY), Vec2(sizeX, 0.f), 6, 0.1f);
	GetAnimator()->CreateAnim(L"Boss2_Damage", m_pTex, Vec2(0.f, sizeY),
		Vec2(sizeX, sizeY), Vec2(sizeX, 0.f), 3, 0.1f);
	//
	GetAnimator()->PlayAnim(L"Boss2_Idle", true);
#pragma endregion

#pragma region AI_Tree
	RandomPatternNode2* randPatternNode = new RandomPatternNode2();

#pragma region pattern1
	SequenceNode* pattern1SeqNode = new SequenceNode();

	RandomPatternNode2* randMoveNode = new RandomPatternNode2();
	MoveNode2* moveRightNode = new MoveNode2(this, Vec2(WINDOW_WIDTH - 64, 160), 500);
	MoveNode2* moveLeftNode = new MoveNode2(this, Vec2(64, 160), 500);

	randMoveNode->RegisterChild(moveLeftNode);
	randMoveNode->RegisterChild(moveRightNode);

	Boss2Pattern1Node* pattern1 = new Boss2Pattern1Node(this, m_pTarget);

	pattern1SeqNode->RegisterChild(randMoveNode);
	pattern1SeqNode->RegisterChild(pattern1);

	randPatternNode->RegisterChild(pattern1SeqNode);
#pragma endregion

#pragma region pattern2
	SequenceNode* pattern2SeqNode = new SequenceNode();

	Boss2Pattern2Node* pattern2Node = new Boss2Pattern2Node(this, m_pTarget);

	MoveNode2* movePattern2Node = new MoveNode2(this, Vec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 50), 500);

	pattern2SeqNode->RegisterChild(movePattern2Node);
	pattern2SeqNode->RegisterChild(pattern2Node);

	randPatternNode->RegisterChild(pattern2SeqNode);
#pragma endregion

#pragma region pattern3
	SequenceNode* pattern3SeqNode = new SequenceNode();

	Boss2Pattern3Node* pattern3Node = new Boss2Pattern3Node(this, m_pTarget);

	pattern3SeqNode->RegisterChild(randMoveNode);
	pattern3SeqNode->RegisterChild(pattern3Node);

	randPatternNode->RegisterChild(pattern3SeqNode);
#pragma endregion

	//여기에 AI짜서 넣어야 함
	RepeatNode* rootNode = new RepeatNode(randPatternNode);
	m_pTree = new BehaviorTree(rootNode);
#pragma endregion
}

Boss2::~Boss2()
{
	if (m_pTree != nullptr)
		delete m_pTree;
}

void Boss2::Update()
{
	GetAnimator()->Update();

	if (m_isDamage)
	{
		m_iCurTime += fDT;
		if (m_iCurTime >= m_iDamageDelayTime)
		{
			if (m_iHp <= 0)
			{
				Die();
			}

			m_isDamage = false;
			SetName(L"DamageAndJumpAbleObject");
			GetAnimator()->PlayAnim(L"Boss2_Idle", true);
		}
	}
	else
	{
		NODE_STATE state = m_pTree->Update();
	}
}

void Boss2::EnterCollision(Collider* _pOther)
{
	if (m_isDamage) return;

	const Object* pOtherObj = _pOther->GetObj();
	if (pOtherObj->GetName() == L"DIR_BOTTOM_COL")
	{
		--m_iHp;
		SetName(L"None");
		m_isDamage = true;
		m_iCurTime = 0;

		GetAnimator()->PlayAnim(L"Boss2_Damage", true);
	}
}

void Boss2::Render(HDC _dc)
{
	Component_Render(_dc);
}

void Boss2::Die()
{
	EventMgr::GetInst()->SceneChange(L"StageSelect_Scene");
	SaveLoadMgr::GetInst()->Stage2();
}
