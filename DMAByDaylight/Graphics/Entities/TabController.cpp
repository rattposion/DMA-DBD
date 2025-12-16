#include "pch.h"
#include "TabController.h"
#include "Drawing.h"

TabController::TabController()
{
	SetVisible(true);
}

void TabController::Update()
{
	if (!Parent)
		SetVisible(false);

	if (!IsVisible())
		return;
	ParentPos = Parent->GetParentPos();
	Pos.x = 0;
	Pos.y = 0;
	Size = { 100, Parent->GetSize().y - 30 };
	Container::Update();
}

void TabController::Draw()
{
	if (!IsVisible())
		return;

	Container::Draw();
}
