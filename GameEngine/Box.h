#pragma once
#include"Engine/GameObject/GameObject.h"
class Box : public GameObject
{
private:
	int frame_;
	//èdóÕ
	float gravity_;
	//ë¨ìx()
	float velocity_;
	//ñßìx(kg/m^3)
	float density_;
	//ëÃêœ(m^3)
	float volume_;
	//éøó (Kg)
	float mass_;

	XMVECTOR moveVec_;
	int hModel_;

public:
	Box(GameObject* parent);
	~Box();

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
	void OnCollision(GameObject* pTarget) override;
};

