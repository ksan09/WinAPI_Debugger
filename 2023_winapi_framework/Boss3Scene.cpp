#include "pch.h"
#include "Boss3Scene.h"
#include "Object.h"
#include "Core.h"
#include "Player.h"
#include "Block.h"
#include "DefaultMonster.h"
#include "Ground.h"
#include "Monster.h"
#include "KeyMgr.h"
#include "CollisionMgr.h"
#include "ResMgr.h"
#include "Rigidbody2D.h"
#include "Boss3.h"
#include "Spike.h"
#include <time.h>

void Boss3Scene::Init()
{
	Object* pObj = new Player;
	pObj->SetPos((Vec2({ WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 })));
	pObj->SetScale(Vec2(100.f, 100.f));
	AddObject(pObj, OBJECT_GROUP::PLAYER);

	Block* pBlock1 = new Ground;
	pBlock1->SetPos((Vec2(WINDOW_WIDTH / 2 - 4, WINDOW_HEIGHT - 32)));
	pBlock1->SetBlock((Vec2(WINDOW_WIDTH, 160)));
	AddObject(pBlock1, OBJECT_GROUP::OBJ);

	Block* pBlock2 = new Block;
	pBlock2->SetPos((Vec2(192, WINDOW_HEIGHT - 160)));
	pBlock2->SetBlock((Vec2(32.f, 32.f)));
	AddObject(pBlock2, OBJECT_GROUP::OBJ);

	Block* pBlock3 = new Block;
	pBlock3->SetPos((Vec2(WINDOW_WIDTH - 192, WINDOW_HEIGHT - 160)));
	pBlock3->SetBlock((Vec2(32.f, 32.f)));
	AddObject(pBlock3, OBJECT_GROUP::OBJ);

	Boss3* pBoss3 = new Boss3(pObj);
	pBoss3->SetPos(Vec2(32, 128));
	AddObject(pBoss3, OBJECT_GROUP::BOSS);

	srand((unsigned int)time(nullptr));

	//

	Vec2 vResolution = Core::GetInst()->GetResolution();

	// 사운드 세팅
	ResMgr::GetInst()->LoadSound(L"SpikeUp", L"Sound\\SpikeSpawn.wav", false);
	ResMgr::GetInst()->LoadSound(L"Bullet", L"Sound\\BulletShoot.wav", false);
	ResMgr::GetInst()->LoadSound(L"MobShot", L"Sound\\MobShot.wav", false);


	// 충돌체크해야되는것들을 설정하자.
	CollisionMgr::GetInst()->CheckGroup(OBJECT_GROUP::PLAYER_DIR_COL, OBJECT_GROUP::OBJ);
	CollisionMgr::GetInst()->CheckGroup(OBJECT_GROUP::OBJ, OBJECT_GROUP::PLAYER);

	CollisionMgr::GetInst()->CheckGroup(OBJECT_GROUP::PLAYER_DIR_COL, OBJECT_GROUP::BOSS);
	CollisionMgr::GetInst()->CheckGroup(OBJECT_GROUP::OBJ, OBJECT_GROUP::BOSS);
	CollisionMgr::GetInst()->CheckGroup(OBJECT_GROUP::PLAYER, OBJECT_GROUP::BOSS);

	CollisionMgr::GetInst()->CheckGroup(OBJECT_GROUP::OBJ, OBJECT_GROUP::BOTTOM_COL);

	CollisionMgr::GetInst()->CheckGroup(OBJECT_GROUP::OBJ, OBJECT_GROUP::OBJ);
}

void Boss3Scene::Update()
{


	Scene::Update();
	//if(KEY_DOWN(KEY_TYPE::ENTER))
	//	// 씬 변경
}

void Boss3Scene::Render(HDC _dc)
{
	Scene::Render(_dc);
}

void Boss3Scene::Release()
{
	Scene::Release();
	CollisionMgr::GetInst()->CheckReset();
}
