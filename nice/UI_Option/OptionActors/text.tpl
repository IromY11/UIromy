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
    NAME = "UITextBox_Template",
	UITextBox_Template =
	{	
		draw2DNoScreenRatio = TRUE,
		styles =
		{
			{
				-- 0 Unused
				Style = {fontSet = font02,}
			},
			{
				-- 1 Selected button
				Style =
				{
					fontSet = font02,
                    fontPath = "EngineData/Misc/Fonts/font02.tfn", -- obsolete, do not copy paste
					fontSize = 30.000000,
					color = "0xFF000000",
					shadowColor = "0xFF000000",
					shadowOffset = vector2dNew(0,0),
					anchor = Anchor.MiddleCenter,
					hAlignment = Align.Left,
					vAlignment = Align.Middle,
				}
			},
			{
				-- 2 Unused

				Style =
				{
					fontSet = font02,
                    fontPath = "EngineData/Misc/Fonts/font02.tfn", -- obsolete, do not copy paste
					fontSize = 30.000000,
					color = "0xFF000000",
					shadowColor = "0xFF000000",
					shadowOffset = vector2dNew(0,0),
					anchor = Anchor.MiddleLeft,
					hAlignment = Align.Left,
					vAlignment = Align.Middle,
				}
			},
			{
				--3 Unused
				Style = {}
			},
			{
				-- 4 Unselected button
				Style =
				{
					fontSet = font02,
                    fontPath = "EngineData/Misc/Fonts/font02.tfn", -- obsolete, do not copy paste
					fontSize = 30.000000,
					color = "0xFF000000",
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

