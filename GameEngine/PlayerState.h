#pragma once

template<class C>
class PlayerState
{
public:
	virtual ~PlayerState() {}
	
	//初期化(状態が変わるごとに呼ばれる)
	virtual void Init(C& player) {}

	virtual void Update(C& player) {}
};