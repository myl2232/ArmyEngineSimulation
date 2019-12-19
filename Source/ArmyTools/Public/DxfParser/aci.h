
#ifndef DXF_ACI_COLOR
#define DXF_ACI_COLOR 1
// lookup table for autocad color index
struct aci {
	// some color positions
	enum {
		BLACK,
		RED,
		YELLOW,
		GREEN,
		CYAN,
		BLUE,
		MAGENTA,
		WHITE,
		USER_2,
		USER_3,
		BYLAYER = 256,
		MIN = 1,
		MAX = 255
	};
	static double table[256*3];
};
#endif
