includeReference("world/_ww1/common/hud/ui_mainmenu/start_menu/Animation/start_menu_pack_shoppacked.ilu")
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
	NAME = "UIComponent_Template",
	UIComponent_Template =
	{
		is2D = 1,
    },
}

appendTable(params.Actor_Template.COMPONENTS,{component})	
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
							friendlyName="Idle",
							name = "world/_ww1/common/hud/ui_mainmenu/start_menu/Animation/Postcard.anm",
							loop=1,
						},
					},			
				},
			},
		},
		defaultAnimation = "Idle",
	},
}

appendTable(params.Actor_Template.COMPONENTS,{component})
component = {}

