#pragma once
#include "Entity.h"

class TabController : public Container
{
public:
	TabController();
	void Update() override;
	void Draw() override;
};
