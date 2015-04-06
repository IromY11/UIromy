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

component = {}