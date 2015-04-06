params =
{
    
NAME = "Actor_Template",
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
	NAME = "TextureGraphicComponent_Template",
	TextureGraphicComponent_Template =
	{
		size = vector2dNew(400.0, 400.0),
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

component =
{
    NAME = "UITextBox_Template",
	UITextBox_Template =
	{	
		styles =
		{
			{
				Style =
				{
					fontSet = font02,
                    fontPath = "EngineData/Misc/Fonts/font02.tfn", -- obsolete, do not copy paste
					fontSize = 60.000000,
					color = "0xFFFFFFFF",
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
