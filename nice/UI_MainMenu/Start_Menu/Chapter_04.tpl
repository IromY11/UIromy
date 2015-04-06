includeReference("world/_ww1/common/hud/ui_mainmenu/start_menu/Animation/start_menu.ilu")

params =
{
    NAME = "Actor_Template",
    Actor_Template =
    {
        COMPONENTS =
        {
            {
                NAME="AnimLightComponent_Template",
                AnimLightComponent_Template =
                {
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
										friendlyName="Idle",
										name= "world/_ww1/common/hud/ui_mainmenu/start_menu/Animation/Chapter_04.anm",
										loop=1,
									}
								}
							},
						},
					},
					defaultAnimation = "Idle",
                }
            },
        }
    }
}
