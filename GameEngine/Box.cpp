#include "Box.h"
#include"Water.h"
#include"Engine/GameObject/Camera.h"
#include"Engine/ResourceManager/ImageManager.h"
#include"Engine/ResourceManager/Model.h"
#include"Engine/Collider/BoxCollider.h"

Box::Box(GameObject* parent)
	:GameObject(parent,"Box"),
	frame_(0),
	gravity_(9.8),
	velocity_(0),
	density_(200),
	mass_(990),
	moveVec_(XMVectorSet(0,0,0,0)),
	hModel_(-1)
{
}

Box::~Box()
{
}

void Box::Initialize()
{	
	hPict_ = ImageManager::Load("Assets\\green.jpg");
	assert(hPict_ >= 0);
	//‰ŠúˆÊ’u
	transform_.position_.y = -400.0f;
	transform_.position_.z = 700.0f;	
	transform_.scale_ = { 500,500,99 };
	//“–‚½‚è”»’è
	BoxCollider* pCollision = new BoxCollider({ 0,0,0 }, transform_.scale_);
	AddCollider(pCollision);
	XMFLOAT3 size = transform_.scale_;
	//Ž¿—Ê:–§“x~‘ÌÏ
	volume_ = (size.x * size.y * size.z)*8.0f;
	mass_ = density_ * volume_;
	
	moveVec_ = XMVectorSet(0, -1, 0, 0);
	//ƒ‚ƒfƒ‹“Ç‚Ýž‚Ý
	hModel_ = ModelManager::Load("Assets\\MAYABox.fbx");
	assert(hModel_ >= 0);
}

void Box::Update()
{
	Camera::SetPosition({ 0,100,-200 });
	Camera::SetTarget({ 0,40,0 });
	velocity_ += M_S2<float>(gravity_);
	//transform_.position_ = StoreFloat3(XMLoadFloat3(&transform_.position_) + moveVec_ * velocity_);
}

void Box::Draw()
{
	ModelManager::SetTransform(hModel_, transform_);
	ModelManager::Draw(hModel_);
	ImageManager::Draw(hPict_);
}

void Box::Release()
{
}

void Box::OnCollision(GameObject* pTarget)
{
	transform_.position_ = StoreFloat3(XMLoadFloat3(&transform_.position_) - moveVec_ * velocity_);
	float p = ((Water*)pTarget)->GetDensity();
	XMFLOAT3 size = transform_.scale_;
	float yMax = (pTarget->GetPosition().y + pTarget->GetScale().y) - (transform_.position_.y - size.y);
	float V = yMax * (size.x * size.z)*4.0f;
	if (V >= volume_)
		V = volume_;

	float accel =  -(p * V * gravity_) / mass_;
	
	velocity_ += M_S2<float>(accel);
	velocity_ *= 0.98f;
	//velocity_ = M_S<float>(velocity_);
	transform_.position_ = StoreFloat3(XMLoadFloat3(&transform_.position_) + moveVec_ * velocity_);
}
