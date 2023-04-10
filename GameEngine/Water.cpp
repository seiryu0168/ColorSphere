#include "Water.h"
#include"Engine/ResourceManager/Model.h"
#include"Engine/Collider/BoxCollider.h"
Water::Water(GameObject* parent)
	:GameObject(parent,"Water"),
	hModel_(-1),
	density_(997)
{
}

Water::~Water()
{
}

void Water::Initialize()
{
	transform_.position_ = { 0,-100,200 };
	transform_.scale_ = { 100,100,100 };
	BoxCollider* pCollision = new BoxCollider({ 0,0,0 }, transform_.scale_*2);
	AddCollider(pCollision);
	hModel_ = ModelManager::Load("Assets\\WaterBlock.fbx");
	assert(hModel_ >= 0);
}

void Water::Update()
{
}

void Water::Draw()
{
	ModelManager::SetTransform(hModel_, transform_);
	ModelManager::Draw(hModel_);
}

void Water::Release()
{
}
