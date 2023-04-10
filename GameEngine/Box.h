#pragma once
#include"Engine/GameObject/GameObject.h"
class Box : public GameObject
{
private:
	int frame_;
	//�d��
	float gravity_;
	//���x()
	float velocity_;
	//���x(kg/m^3)
	float density_;
	//�̐�(m^3)
	float volume_;
	//����(Kg)
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

