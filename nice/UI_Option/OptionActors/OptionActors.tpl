includeReference("world/_ww1/common/hud/ui_vh/ui_option/optionactors/Animation/OptionActors.ilu")

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
										name= "world/_ww1/common/hud/ui_vh/ui_option/optionactors/Animation/Language_Selected.anm",
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
