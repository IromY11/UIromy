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
	NAME = "UIComponent_Template",
	UIComponent_Template =
	{
		is2D = 1,
    },
}

appendTable(params.Actor_Template.COMPONENTS,{component})	
component =
{
	NAME = "TextureGraphicComponent_Template",
	TextureGraphicComponent_Template =
	{
	draw2DNoScreenRatio = TRUE,
		size = vector2dNew(1024.0, 520.0),
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