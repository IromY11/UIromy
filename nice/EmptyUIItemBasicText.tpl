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
		-- animUnselected = "Unselected",
		-- animSelected = "Selected",
		blinkScale = 1.0,
        -- blinkPeriod = 0.8,
		-- --colorFactorUnselected = "0xFFac1814",
		-- colorFactorSelected = "0xFFFF0000",
		-- colorBlendTime = 0.016,
		
        -- activatingScale = 1.0,
        -- activatingDuration = 0.0,
        -- activatingRebound = 1,	
		-- WwisOnSelectedSound = "Play_Menu_Butt_Validate_Gen",
		-- WwisOnChangeSelection = "Play_Menu_Butt_Selected_Gen",	
		-- WwisOnSelectedLocked = "Play_Menu_Butt_Validate_Locked",	
	},
}

appendTable(params.Actor_Template.COMPONENTS,{component})
component = {}

includeReference("world/_WW1/Common/hud/UI_Pause/IconMission_I_SoundComponent.ilu")
includeReference("World/Common/UI/Common/Components/UITextBox.ilu")

