includeReference("world/_ww1/common/hud/ui_mainmenu/screen/Animation/Screen.ilu")

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
										name= "world/_ww1/common/hud/ui_mainmenu/screen/Animation/Ep1_Sq01_Selected.anm",
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
