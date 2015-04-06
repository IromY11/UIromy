params =
{
    NAME="Actor_Template",
    Actor_Template =
    {
		UPDATELAYER = UpdateLayer.Menu,
        COMPONENTS =
		{
			{
				NAME = "W1W_SocialMenu_Template",
				W1W_SocialMenu_Template =
				{
					is2D = 1,
				},
			},
			{
				NAME="AFXPostProcessComponent_Template",
				AFXPostProcessComponent_Template =
				{
					input =
					{
						InputDesc={name="blur", varType=AnimInputTypes.float}, 
					},
				},
			},					
        },
    },
}


includeReference("world/_WW1/Common/hud/UI_Pause/UIMenuPause_SoundComponent.ilu")