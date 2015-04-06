params =
{
    
NAME = "Actor_Template",
Actor_Template =
    {
        SCALE = vector2dNew(1.000000,1.000000),
        RANK = -1.000000,
        STARTPAUSED = 0,
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
		ScaleUnselected = 1,
        activatingScale = 1.0,        
		blinkScale = 1.0,        
		--colorBlendTime = 0.016,	
		--colorFactorUnselected = "0xFFFFFFFF", -- 0x88685c39
		--colorFactorSelected = "0xFFfffdf7",
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
	NAME = "TextureGraphicComponent_Template",
	TextureGraphicComponent_Template =
	{
		draw2DNoScreenRatio = TRUE,
		size = vector2dNew(850.0, 425.0),
		posOffset = vector2dNew(0.000000,0.000000),
		angleOffset = 0.000000,
		patchLevel = 0,
		patchHLevel = 2,
		patchVLevel = 2,
		visualAABB =
		{
			AABB =
			{
				MIN = vector2dNew(0.000000,0.000000),
				MAX = vector2dNew(0.000000,0.000000),
			},
		},
		draw2D = TRUE,
	},
}

appendTable(params.Actor_Template.COMPONENTS,{component})
component = {}