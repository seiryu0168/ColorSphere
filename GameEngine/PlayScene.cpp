#include "PlayScene.h"
#include"Box.h"
//#include"Water.h"
#include"ObjectSetter.h"
#include"Test.h"
// �R���X�g���N�^
PlayScene::PlayScene(GameObject * parent)
	: SceneBase(parent, "PlayScene")
{
	//Instantiate<ObjectSetter>(this);
	Instantiate<Box>(this);
	//Instantiate<Water>(this);
}

//������
void PlayScene::Initialize()
{
}

//�X�V
void PlayScene::Update()
{
}

//�`��
void PlayScene::Draw()
{
}

//�J��
void PlayScene::Release()
{
	int a = 0;
}
