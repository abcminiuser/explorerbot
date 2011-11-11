enum RGB_Colour_t
{
	RGB_COLOUR_Off         = 0,
	RGB_COLOUR_Red         = (1 << 4),
	RGB_COLOUR_Green       = (1 << 5),
	RGB_COLOUR_Blue        = (1 << 6),
	RGB_COLOUR_Yellow      = (RGB_COLOUR_Red  | RGB_COLOUR_Green),
	RGB_COLOUR_Cyan        = (RGB_COLOUR_Blue | RGB_COLOUR_Green),			
	RGB_COLOUR_Magenta     = (RGB_COLOUR_Red  | RGB_COLOUR_Blue),
	RGB_COLOUR_White       = (RGB_COLOUR_Red  | RGB_COLOUR_Green | RGB_COLOUR_Blue),
};

enum RGB_Colour_Aliases_t
{
	RGB_ALIAS_Disconnected = RGB_COLOUR_White,
	RGB_ALIAS_Enumerating  = RGB_COLOUR_Yellow,
	RGB_ALIAS_Error        = RGB_COLOUR_Red,
	RGB_ALIAS_Ready        = RGB_COLOUR_Green,
	RGB_ALIAS_Connected    = RGB_COLOUR_Blue,
	RGB_ALIAS_Busy         = RGB_COLOUR_Magenta,
};