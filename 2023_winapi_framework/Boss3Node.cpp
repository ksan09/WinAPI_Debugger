#include "pch.h"
#include "Boss3Node.h"
#include "TimeMgr.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "DefaultMonster.h"
#include "Spike.h"
#include "Bullet.h"

RandomPatternNode::RandomPatternNode()
	: m_iSelect(0)
{
}

RandomPatternNode::~RandomPatternNode()
{
}

void RandomPatternNode::OnStart()
{
	m_iSelect = rand() % m_pChildren.size();
}

NODE_STATE RandomPatternNode::OnUpdate()
{
	return m_pChildren[m_iSelect]->Update();
}

void RandomPatternNode::OnStop()
{
}

BoundMonsterSpawnPattern1Node::BoundMonsterSpawnPattern1Node(Object* owner, Object* target)
	: m_pOwner(owner)
	, m_pTarget(target)
	, m_isRight(false)
	, m_fCurTime(0.f)
	, m_fSpawnDelay(0.5f)
	, m_iCurSpawnCount(0)
	, m_iSpawnCount(10)
	, m_fDelayTime(3.f)
	, m_fSpawnMonsterSpeed(200.f)
{
}

BoundMonsterSpawnPattern1Node::~BoundMonsterSpawnPattern1Node()
{
}

void BoundMonsterSpawnPattern1Node::OnStart()
{
	m_fCurTime = 0;
	m_iCurSpawnCount = 0;

	m_isRight = m_pOwner->GetPos().x > (WINDOW_WIDTH / 2);
}

NODE_STATE BoundMonsterSpawnPattern1Node::OnUpdate()
{
	m_fCurTime += fDT;
	if (m_iCurSpawnCount < m_iSpawnCount && m_fCurTime >= m_fSpawnDelay)
	{
		SpawnObject();
		m_fCurTime = 0;
		m_iCurSpawnCount++;

		if (m_iCurSpawnCount % 5 - 2 == 0)
		{
			SpawnBullet();
		}

	}

	if (m_iCurSpawnCount >= m_iSpawnCount && m_fCurTime >= m_fDelayTime)
	{
		return NODE_STATE::SUCCESS;
	}

	return NODE_STATE::RUNNING;
}

void BoundMonsterSpawnPattern1Node::OnStop()
{
}

void BoundMonsterSpawnPattern1Node::SpawnObject()
{
	float speed = (m_isRight ? -m_fSpawnMonsterSpeed : m_fSpawnMonsterSpeed);

	DefaultMonster* pJDBlock = new DefaultMonster;
	pJDBlock->SetPos(m_pOwner->GetPos());
	pJDBlock->SetBlock((Vec2(32.f, 32.f)));
	pJDBlock->GetRigidbody2D()->SetVelocity({ speed, -10.f });
	pJDBlock->SetSpeed(speed);
	SceneMgr::GetInst()->GetCurScene()->AddObject(pJDBlock, OBJECT_GROUP::OBJ);
}

void BoundMonsterSpawnPattern1Node::SpawnBullet()
{
	if (m_pTarget == nullptr)
		return;

	Vec2 targetPos = m_pTarget->GetPos();
	Vec2 pos = m_pOwner->GetPos();
	Vec2 dir = ( targetPos - pos );
	dir.Normalize();

	Bullet* bullet = new Bullet;
	bullet->SetPos(pos);
	bullet->GetRigidbody2D()->SetVelocity(dir * 300.f);
	SceneMgr::GetInst()->GetCurScene()->AddObject(bullet, OBJECT_GROUP::OBJ);
}

MoveNode::MoveNode(Object* owner, Vec2 endPos, float speed)
	: m_pOwner(owner)
	, m_vEndPos(endPos)
	, m_fSpeed(speed)
{
}

MoveNode::~MoveNode()
{
}

void MoveNode::OnStart()
{
}

NODE_STATE MoveNode::OnUpdate()
{
	if (m_pOwner == nullptr)
		return NODE_STATE::FAILURE;

	Vec2 pos = m_pOwner->GetPos();

	float dist = (m_vEndPos - pos).Length();
	if (dist < 0.5f)
	{
		return NODE_STATE::SUCCESS;
	}
	 
	Vec2 dir = (m_vEndPos - pos).Normalize();
	pos.x += dir.x * m_fSpeed * fDT;
	pos.y += dir.y * m_fSpeed * fDT;

	m_pOwner->SetPos(pos);

	return NODE_STATE::RUNNING;
}

void MoveNode::OnStop()
{
}

Boss3Pattern2Node::Boss3Pattern2Node()
	: m_fDangerDelay(1.f)
	, m_fDelaySpikeSpawn(0.1f)
	, m_fWaitSpikeDelete(4.5f)
	, m_fCurTime(0.f)
{
}

Boss3Pattern2Node::~Boss3Pattern2Node()
{
}

void Boss3Pattern2Node::OnStart()
{
	m_fCurTime = 0;
	// ���� ���� ��ġ�� ��ȯ
	for (int i = 0; i < 4; ++i)
	{
		DefaultMonster* pJDBlock = new DefaultMonster;
		pJDBlock->SetPos({ 128 + i * 240, 128 * 3 });
		pJDBlock->SetBlock((Vec2(32.f, 32.f)));
		pJDBlock->GetRigidbody2D()->SetUseGravity(false);
		SceneMgr::GetInst()->GetCurScene()->AddObject(pJDBlock, OBJECT_GROUP::OBJ);
	}

	Spike* pSpike = new Spike(m_fWaitSpikeDelete);
	SceneMgr::GetInst()->GetCurScene()->AddObject(pSpike, OBJECT_GROUP::OBJ);
}

NODE_STATE Boss3Pattern2Node::OnUpdate()
{
	m_fCurTime += fDT;
	if (m_fCurTime >= 6.f)
		return NODE_STATE::SUCCESS;

	return NODE_STATE::RUNNING;
}

void Boss3Pattern2Node::OnStop()
{
}