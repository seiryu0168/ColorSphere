#pragma once
#include"Engine/GameObject/GameObject.h"
class Water : public GameObject
{
private:
	//–§“x(kg/m^3)
	float density_;
	int hModel_;


public:
	Water(GameObject* parent);
	~Water();

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
	float GetDensity() { return density_; }
	//void OnCollision(GameObject* pTarget);
};

