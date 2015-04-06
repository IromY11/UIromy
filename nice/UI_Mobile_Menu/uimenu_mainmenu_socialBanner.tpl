params =
{
    NAME="Actor_Template",
    Actor_Template =
    {
		UPDATELAYER = UpdateLayer.Menu,
        COMPONENTS =
		{
			{
				NAME = "W1W_MainMenu_SocialBanner_Template",
				W1W_MainMenu_SocialBanner_Template =
				{
					is2D = 1,
				},
			},	
			{
				NAME = "W1W_TouchSliderComponent_Template",
				W1W_TouchSliderComponent_Template =
				{
					slidingSpeed = 1.0,
					objectTag = "SocialBannerItem",
				}
			},			
        },
    },
}


includeReference("world/_WW1/Common/hud/UI_Pause/UIMenuPause_SoundComponent.ilu")