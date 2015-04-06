params =
{
    NAME="Actor_Template",
    Actor_Template =
    {
		UPDATELAYER = UpdateLayer.Menu,
        COMPONENTS =
		{
			{
				NAME = "W1W_LocalCrossPromoAdMenu_Template",
				W1W_LocalCrossPromoAdMenu_Template =
				{
					is2D = 1,
				},
			},
			{
				NAME = "W1W_TouchSliderComponent_Template",
				W1W_TouchSliderComponent_Template =
				{
					slidingSpeed = 1.0,
					isOpen = FALSE,
					autoOpen = TRUE,
				}
			},				
		},
    },
}


includeReference("world/_WW1/Common/hud/UI_Pause/UIMenuPause_SoundComponent.ilu")