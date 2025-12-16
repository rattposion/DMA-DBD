#include "pch.h"
#include "GUI.h"
#include "entity.h"
#include "Form.h"
#include "Button.h"
#include "ColourPicker.h"
#include "Label.h"
#include "tab.h"
#include "TabController.h"
#include "Toggle.h"
#include "Slider.h"
#include "DropDown.h"
#include "ComboBox.h"
#include "KeyBind.h"
#include "TabListBox.h"
#include "TabListBoxController.h"
#include "TextBox.h"
#include "ConfigUtilities.h"
#include "Makcu.h"

int SelectedTab = 1;
int SelectedSubTab = 0;
int TabCount = 0;
int KeyBindClipBoard = 0;
EntityVector MenuEntity;
std::shared_ptr<TabListBoxController> LogBox;
bool MenuOpen = true;
D2D1::ColorF ColourPickerClipBoard = Colour(255,255,255);
D2D1::ColorF ColourPick = Colour(0, 150, 255, 255);
std::wstring ScreenWidth = std::to_wstring(Configs.Overlay.Width);
std::wstring ScreenHeight = std::to_wstring(Configs.Overlay.Height);
void CreateGUI()
{
	MenuEntity = std::make_shared<Container>();
	auto form = std::make_shared<Form>(100, 100.0f, 300, 200, 2, 30, LIT(L"DMA Dead By Daylight"), false);
	{
		auto tabcontroller = std::make_shared<TabController>();
		form->Push(tabcontroller);

		auto playeresptab = std::make_shared<Tab>(LIT(L"Player ESP"), 5, 5, &SelectedTab, 0, 20);
		{
			auto name = std::make_shared<Toggle>(100, 5, LIT(L"Name"), &Configs.Survivor.Name);
			playeresptab->Push(name);
			auto textcolour = std::make_shared<ColourPicker>(160, 6, &Configs.Survivor.TextColour);
			playeresptab->Push(textcolour);
			auto distance = std::make_shared<Toggle>(100, 25, LIT(L"Distance"), &Configs.Survivor.Distance);
			playeresptab->Push(distance);
			auto maxdistance = std::make_shared<Slider<int>>(100, 45, 150, LIT(L"Max Distance"), LIT(L"m"), 0, 1000, &Configs.Survivor.MaxDistance);
			playeresptab->Push(maxdistance);
			auto textsize = std::make_shared<Slider<int>>(100, 70, 150, LIT(L"Text Size"), LIT(L"px"), 4, 16, &Configs.Survivor.FontSize);
			playeresptab->Push(textsize);
		}
		tabcontroller->Push(playeresptab);
		auto killeresptab = std::make_shared<Tab>(LIT(L"Killer ESP"), 5, 30, &SelectedTab, 0, 20);
		{
			auto name = std::make_shared<Toggle>(100, 5, LIT(L"Name"), &Configs.Killer.Name);
			killeresptab->Push(name);
			auto textcolour = std::make_shared<ColourPicker>(160, 6, &Configs.Killer.TextColour);
			killeresptab->Push(textcolour);
			auto distance = std::make_shared<Toggle>(100, 25, LIT(L"Distance"), &Configs.Killer.Distance);
			killeresptab->Push(distance);
			auto maxdistance = std::make_shared<Slider<int>>(100, 45, 150, LIT(L"Max Distance"), LIT(L"m"), 0, 1000, &Configs.Killer.MaxDistance);
			killeresptab->Push(maxdistance);
			auto textsize = std::make_shared<Slider<int>>(100, 70, 150, LIT(L"Text Size"), LIT(L"px"), 4, 16, &Configs.Killer.FontSize);
			killeresptab->Push(textsize);
		}
		tabcontroller->Push(killeresptab);
		auto overlaytab = std::make_shared<Tab>(LIT(L"Overlay"), 5, 55, &SelectedTab, 0, 20);
		{
			auto overrideresolution = std::make_shared<Toggle>(100, 5, LIT(L"Override W2S Resolution"), &Configs.Overlay.OverrideResolution);
			overlaytab->Push(overrideresolution);
			auto screenwidth = std::make_shared<TextBox>(100, 35, LIT(L"Screen Width"), &ScreenWidth);
			screenwidth->SetValueChangedEvent([]()
				{
					try
					{
						Configs.Overlay.Width = std::stoi(ScreenWidth);
					}
					catch (std::exception ex)
					{
					}
				});
			overlaytab->Push(screenwidth);
			auto screenheight = std::make_shared<TextBox>(100, 70, LIT(L"Screen Height"), &ScreenHeight);
			screenheight->SetValueChangedEvent([]()
				{
					try
					{
						Configs.Overlay.Height = std::stoi(ScreenHeight);
					}
					catch (std::exception ex)
					{
					}
				});
			overlaytab->Push(screenheight);


		}
		tabcontroller->Push(overlaytab);

		auto skillchecktab = std::make_shared<Tab>(LIT(L"SkillCheck"), 5, 80, &SelectedTab, 0, 20);
		{
			auto enabled = std::make_shared<Toggle>(100, 5, LIT(L"Enabled"), &Configs.SkillCheck.Enabled);
			skillchecktab->Push(enabled);

            auto autoenabled = std::make_shared<Toggle>(100, 25, LIT(L"Auto SkillCheck (Makcu)"), &Configs.SkillCheck.AutoSkillCheck);
            skillchecktab->Push(autoenabled);

            auto autokey = std::make_shared<KeyBind>(100, 45, LIT(L"Key"), &Configs.SkillCheck.AutoSkillCheckKey);
            skillchecktab->Push(autokey);

            auto usemakcu = std::make_shared<Toggle>(100, 65, LIT(L"Use Makcu"), &Configs.SkillCheck.UseMakcu);
            skillchecktab->Push(usemakcu);

            auto comport = std::make_shared<Slider<int>>(100, 85, 150, LIT(L"COM Port (0=Auto)"), LIT(L""), 0, 20, &Configs.SkillCheck.ComPort);
            skillchecktab->Push(comport);

			auto size = std::make_shared<Slider<int>>(100, 105, 150, LIT(L"Size"), LIT(L"px"), 100, 500, &Configs.SkillCheck.Size);
			skillchecktab->Push(size);

			auto maincolour = std::make_shared<ColourPicker>(160, 125, &Configs.SkillCheck.MainColour);
            // Label for colour
			// We might need a label or just place it. The picker usually has a small box.
            // Let's just add pickers for now.
			skillchecktab->Push(maincolour);

            // Add labels/pickers for other colours
            auto successcolour = std::make_shared<ColourPicker>(190, 125, &Configs.SkillCheck.SuccessColour);
            skillchecktab->Push(successcolour);

            auto bonuscolour = std::make_shared<ColourPicker>(220, 125, &Configs.SkillCheck.BonusColour);
            skillchecktab->Push(bonuscolour);

             auto cursorcolour = std::make_shared<ColourPicker>(250, 125, &Configs.SkillCheck.CursorColour);
            skillchecktab->Push(cursorcolour);

            // Labels to explain colours
             auto lMain = std::make_shared<Label>(LIT(L"Main"), 160, 145);
             skillchecktab->Push(lMain);
             auto lSuccess = std::make_shared<Label>(LIT(L"Succ"), 190, 145);
             skillchecktab->Push(lSuccess);
             auto lBonus = std::make_shared<Label>(LIT(L"Bonu"), 220, 145);
             skillchecktab->Push(lBonus);
             auto lCursor = std::make_shared<Label>(LIT(L"Curs"), 250, 145);
             skillchecktab->Push(lCursor);

		}
		tabcontroller->Push(skillchecktab);

		auto configtab = std::make_shared<Tab>(LIT(L"Config"), 5, 105, &SelectedTab, 0, 20);
		{
			auto saveconfig = std::make_shared<Button>(100, 5, LIT(L"Save"), []()
				{


					SaveConfig(L"Default.json");
					CreateGUI(); // reinit/ reload
					SelectedTab = 1;

				});
			configtab->Push(saveconfig);

			auto loadconfig = std::make_shared<Button>(165, 5, LIT(L"Load"), []()
				{


					LoadConfig(L"Default.json");
					CreateGUI(); // reinit/ reload
					SelectedTab = 1;

				});
			configtab->Push(loadconfig);
		}
		tabcontroller->Push(configtab);

        auto debugtab = std::make_shared<Tab>(LIT(L"Debug"), 5, 130, &SelectedTab, 0, 20);
        {
             LogBox = std::make_shared<TabListBoxController>(100, 5, 190, 190);
             debugtab->Push(LogBox);
        }
        tabcontroller->Push(debugtab);
	}

	MenuEntity->Push(form);
	MenuEntity->Draw();
	MenuEntity->Update();
}

void SetFormPriority()
{
	// This sorts the host container (containerptr) which contains forms, as long as a form isn't parented to another form then this will allow it to draw over when clicked.
	// I swear to god if i need to make this work for forms inside forms for some odd reason in the future then i am going to throw a monitor out the window.
	std::sort(MenuEntity->GetContainer().begin(), MenuEntity->GetContainer().end(),
	          [](child a, child b) { return b->GetLastClick() < a->GetLastClick(); }
	);
}

float LastOpen = 0;

void Render()
{
	if (IsKeyClicked(VK_INSERT) && LastOpen < clock() * 0.00001f)
	{
		LastOpen = (clock() * 0.00001f) + 0.002f;
		MenuOpen = !MenuOpen;
	}

    // Update Logs
    if (LogBox)
    {
        static size_t LastLogCount = 0;
        std::vector<std::wstring> logs = MakcuInstance->GetLogs();
        if (logs.size() > LastLogCount)
        {
            for (size_t i = LastLogCount; i < logs.size(); i++)
            {
                 auto item = std::make_shared<TabListBox>(logs[i]);
                 LogBox->PushBack(item);
            }
            LastLogCount = logs.size();
        }
    }

	MenuEntity->Draw();
	MenuEntity->GetContainer()[0]->Update(); // only allow stretching,dragging and other update stuff if it is the main form, prevents dragging and sizing the wrong forms.
	SetFormPriority();
}
