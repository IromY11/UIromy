includeReference("world/_ww1/common/hud/ui_mainmenu/start_menu/Animation/start_menu_pack_chapterpacked.ilu")
params =
{
    NAME="Actor_Template",
    Actor_Template =
    {
		UPDATELAYER = UpdateLayer.Menu,
        COMPONENTS =
        {
        },
    },
}

component =
{
	NAME = "UIItemBasic_Template",
	UIItemBasic_Template =
	{
		is2D = 1,
		--ignoreBlink = 0,
		animUnselected = "Unselected",
		animSelected = "Selected",
		animLocked = "Locked",
		---animActivating = "",
		--ScaleUnselected = 0.8,
        --activatingScale = 1.0,        
		blinkScale = 1.0,        
		--colorBlendTime = 0.016,	
		--colorFactorUnselected = "0xFFDEDEDE", -- 0x88685c39
		--colorFactorSelected = "0xFFFFFFFF",
		--colorFactorLocked = "0xFFFFFFFF",
		WwisOnSelectedSound = "Play_Menu_Butt_Validate_Gen",
		WwisOnChangeSelection = "Play_Menu_Butt_Selected_Gen",	
		WwisOnSelectedLocked = "Play_Menu_Butt_Validate_Locked",	
   },	
}

appendTable(params.Actor_Template.COMPONENTS,{component})

includeReference("EngineData/Misc/Components/SoundComponent.ilu")
includeReference("world/_WW1/Common/hud/UI_Pause/Sound/EVT_UIMenuPause.ilu")

component =
{
	NAME="AnimLightComponent_Template",
	AnimLightComponent_Template =
	{
		draw2D = TRUE,
		animSet=
		{
			SubAnimSet_Template =
			{
				animPackage = animPackage,
				animations=
				{
					{
						SubAnim_Template=
						{
							friendlyName="Selected",
							name = "world/_WW1/common/hud/UI_MainMenu/Start_Menu/Animation/Play_Wide_Selected.anm",
							loop=1,
						},
					},
					{
						SubAnim_Template=
						{
							friendlyName="Unselected",
							name = "world/_WW1/common/hud/UI_MainMenu/Start_Menu/Animation/Play_Wide_Stand.anm",
							loop=1,
						},
					},	
					{
						SubAnim_Template=
						{
							friendlyName="Locked",
							name = "world/_WW1/common/hud/UI_MainMenu/Start_Menu/Animation/Locked.anm",
							loop=1,
						},
					},						
				},
			},
		},
		defaultAnimation = "Unselected",
	},
}

appendTable(params.Actor_Template.COMPONENTS,{component})

component =
{
    NAME = "UITextBox_Template",
	UITextBox_Template =
	{	
		styles =
		{
			{
				-- 0 Unused
				Style = {fontSet = font02,}
			},
			{
				-- 1 Selected button
				Style =
				{
					fontSet = font02,
                    fontPath = "EngineData/Misc/Fonts/font02.tfn", -- obsolete, do not copy paste
					fontSize = 50.000000,
					color = "0xFF000000",
					shadowColor = "0xFF000000",
					shadowOffset = vector2dNew(0,0),
					anchor = Anchor.MiddleCenter,
					hAlignment = Align.Left,
					vAlignment = Align.Middle,
				}
			},
			{
				-- 2 Unused
				Style =	{}
			},
			{
				--3 Unused
				Style = {}
			},
			{
				-- 4 Unselected button
				Style =
				{
					fontSet = font02,
                    fontPath = "EngineData/Misc/Fonts/font02.tfn", -- obsolete, do not copy paste
					fontSize = 30.000000,
					color = "0xFF000000",
					shadowColor = "0xFF000000",
					shadowOffset = vector2dNew(0,0),
					anchor = Anchor.MiddleLeft,
					hAlignment = Align.Left,
					vAlignment = Align.Middle,
				}
			},
			
		},
	},
}

appendTable(params.Actor_Template.COMPONENTS,{component})
component = {}

