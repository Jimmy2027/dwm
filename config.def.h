/* See LICENSE file for copyright and license details. */

/* Constants */
#define TERMINAL "st"
#define TERMCLASS "St"

/* appearance */
static unsigned int borderpx = 1; /* border pixel of windows */
static unsigned int snap = 32;	  /* snap pixel */
static int showbar = 1;			  /* 0 means no bar */
static int topbar = 1;			  /* 0 means bottom bar */
static int swallowfloating = 0;	  /* 1 means swallow floating windows by default */
// static char font[]            = "monospace:size=10";
static char dmenufont[] = "monospace:size=10";
static char *fonts[] = {"monospace:size=10", "NotoColorEmoji:pixelsize=10:antialias=true:autohint=true"};
// static const char *fonts[]          = { font };
static char normbgcolor[] = "#000000";
static char normbordercolor[] = "#ffb86c";
static char normfgcolor[] = "#ff79c6"; /* color of numbers */
static char selfgcolor[] = "#ffb86c";
static char selbordercolor[] = "#282a36";
static char selbgcolor[] = "#282a36";
static char *colors[][3] = {
	/*               fg           bg           border   */
	[SchemeNorm] = {normfgcolor, normbgcolor, normbordercolor},
	[SchemeSel] = {selfgcolor, selbgcolor, selbordercolor},
};

/* tagging */
static const char *tags[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{"Gimp", NULL, NULL, 0, 1, -1},
	{NULL, NULL, "Event Tester", 0, 0, 0, 1, -1}, /* xev */
	//{ "Enpass",  NULL,       NULL,       1 << 6,       0,           -1 },
	/* class     instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{NULL, NULL, "Event Tester", 0, 0, 0, 1, -1},
	{"St", NULL, NULL, 0, 0, 1, 0, -1},
	{"Brave-browser", NULL, NULL, 1 << 1, 0, 1, -1},
	{"brave-browser", NULL, NULL, 1 << 1, 0, 1, -1},
	{"thunderbird", NULL, NULL, 1 << 4, 0, 1, -1},
	{"Spotify", NULL, NULL, 1 << 5, 0, 1, -1},
	{"Zotero", NULL, NULL, 1 << 6, 0, 1, -1},
	{"nextcloud", NULL, NULL, 1 << 6, 0, 1, -1},
	{"telegram-desktop", NULL, NULL, 1 << 7, 0, 1, -1},
	{"whatsapp-for-linux", NULL, NULL, 1 << 7, 0, 1, -1},
};

/* layout(s) */
static float mfact = 0.55;	/* factor of master area size [0.05..0.95] */
static int nmaster = 1;		/* number of clients in master area */
static int resizehints = 1; /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{"[]=", tile}, /* first entry is default */
	{"><>", NULL}, /* no layout function means floating behavior */
	{"[M]", monocle},
};

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY, TAG)                                          \
	{MODKEY, KEY, view, {.ui = 1 << TAG}},                         \
		{MODKEY | ControlMask, KEY, toggleview, {.ui = 1 << TAG}}, \
		{MODKEY | ShiftMask, KEY, tag, {.ui = 1 << TAG}},          \
		{MODKEY | ControlMask | ShiftMask, KEY, toggletag, {.ui = 1 << TAG}},

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd)                                           \
	{                                                        \
		.v = (const char *[]) { "/bin/sh", "-c", cmd, NULL } \
	}

/* commands */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
static char dmenumon[2] = "0";		 /* component of dmenucmd, manipulated in spawn() */
// static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *dmenucmd[] = {"dmenu_run", "-m", dmenumon, NULL};
// static const char *dmenucmd[] = { "dmenu_run", NULL };
static const char *termcmd[] = {"st", "-f", "monospace:size=13", NULL};
static const char *spotify[] = {"spotify", NULL};
static const char *brave[] = {"$BROWSER", NULL};
static const char *passmenu[] = {"passdmenu.py", NULL};
static const char *dmenu_bluetooth[] = {"dmenu_bluetooth", NULL};
static const char *dkill[] = {"dkill", NULL};
static const char *dmenu_wifi[] = {"dmenu_wifi.py", NULL};
static const char *slock[] = {"slock", NULL};
static const char *firefox[] = {"firefox-bin", NULL};
static const char *telegram[] = {"telegram-desktop", NULL};
static const char *slack[] = {"slack", NULL};

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
	//		{ "font",               STRING,  &font },
	{"dmenufont", STRING, &dmenufont},
	{"normbgcolor", STRING, &normbgcolor},
	{"normbordercolor", STRING, &normbordercolor},
	{"normfgcolor", STRING, &normfgcolor},
	{"selbgcolor", STRING, &selbgcolor},
	{"selbordercolor", STRING, &selbordercolor},
	{"selfgcolor", STRING, &selfgcolor},
	{"borderpx", INTEGER, &borderpx},
	{"snap", INTEGER, &snap},
	{"showbar", INTEGER, &showbar},
	{"topbar", INTEGER, &topbar},
	{"nmaster", INTEGER, &nmaster},
	{"resizehints", INTEGER, &resizehints},
	{"mfact", FLOAT, &mfact},
};

/** Function to shift the current view to the left/right
 *
 * @param: "arg->i" stores the number of tags to shift right (positive value)
 *          or left (negative value)
 */
void shiftview(const Arg *arg)
{
	Arg shifted;

	if (arg->i > 0) // left circular shift
		shifted.ui = (selmon->tagset[selmon->seltags] << arg->i) | (selmon->tagset[selmon->seltags] >> (LENGTH(tags) - arg->i));

	else // right circular shift
		shifted.ui = selmon->tagset[selmon->seltags] >> (-arg->i) | selmon->tagset[selmon->seltags] << (LENGTH(tags) + arg->i);

	view(&shifted);
}

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd)                                           \
	{                                                        \
		.v = (const char *[]) { "/bin/sh", "-c", cmd, NULL } \
	}

static Key keys[] = {
	/* modifier                     key        function        argument */
	{MODKEY, XK_p, spawn, {.v = dmenucmd}},
	{MODKEY | ShiftMask, XK_Return, spawn, {.v = termcmd}},
	{MODKEY, XK_s, spawn, {.v = spotify}},
	{MODKEY, XK_w, spawn, {.v = brave}},
	{MODKEY, XK_z, spawn, {.v = passmenu}},
	{MODKEY | ShiftMask, XK_l, spawn, {.v = slock}},
	{MODKEY | ShiftMask, XK_b, spawn, {.v = dmenu_bluetooth}},
	{MODKEY | ShiftMask, XK_w, spawn, {.v = dmenu_wifi}},
	{MODKEY, XK_n, spawn, SHCMD(TERMINAL " -e vim -c VimwikiIndex")},
	{MODKEY | ShiftMask, XK_d, spawn, SHCMD(TERMINAL "-e vim -c VimwikiMakeDiaryNote")},
	{MODKEY | ShiftMask, XK_t, spawn, SHCMD(TERMINAL " -e vim /home/hendrik/Nextcloud/Notes/todo.md")},
	{MODKEY | ShiftMask, XK_k, spawn, {.v = dkill}},
	{MODKEY | ShiftMask, XK_a, spawn, SHCMD(TERMINAL " -e vim ~/docsrc/shortcuts/shortcuts.md && pandoc ~/docsrc/shortcuts/shortcuts.md -o ~/docsrc/shortcuts/shortcuts.pdf")},
	{MODKEY | ShiftMask, XK_s, spawn, SHCMD("zathura ~/docsrc/shortcuts/shortcuts.pdf")},
	{MODKEY | ShiftMask, XK_w, spawn, SHCMD("/opt/whatsapp-desktop/WhatsApp")},
	{MODKEY, XK_udiaeresis, spawn, SHCMD("flameshot gui")},
	{MODKEY, XK_u, spawn, SHCMD("/opt/whatsapp-desktop/WhatsApp")},
	{MODKEY, XK_u, spawn, SHCMD("telegram-desktop")},
	{MODKEY, XK_u, spawn, SHCMD("slack")},
	{MODKEY, XK_F, spawn, SHCMD("bbg")},
	{MODKEY, XK_r, spawn, SHCMD(TERMINAL " -e lf")},
	{MODKEY | ShiftMask, XK_r, spawn, SHCMD(TERMINAL " -e htop")},
	{MODKEY, XK_minus, spawn, SHCMD("pamixer --allow-boost -d 5; kill -44 $(pidof dwmblocks)")},
	{MODKEY | ShiftMask, XK_minus, spawn, SHCMD("pamixer --allow-boost -d 15; kill -44 $(pidof dwmblocks)")},
	{MODKEY, XK_period, spawn, SHCMD("pamixer --allow-boost -i 5; kill -44 $(pidof dwmblocks)")},
	{MODKEY | ShiftMask, XK_period, spawn, SHCMD("pamixer --allow-boost -i 15; kill -44 $(pidof dwmblocks)")},
	{MODKEY | ShiftMask, XK_m, spawn, SHCMD("pamixer -t; kill -44 $(pidof dwmblocks)")},
	{MODKEY | ShiftMask, XK_Up, spawn, SHCMD("xbacklight -inc 25")},
	{MODKEY | ShiftMask, XK_Down, spawn, SHCMD("xbacklight -dec 25")},
	{MODKEY, XK_F5, spawn, SHCMD("spt playback --toggle")},
	{MODKEY, XK_F4, spawn, SHCMD("spt playback --previous")},
	{MODKEY, XK_F6, spawn, SHCMD("spt playback --next")},
	{MODKEY, XK_b, togglebar, {0}},
	{MODKEY, XK_j, focusstack, {.i = +1}},
	{MODKEY, XK_k, focusstack, {.i = -1}},
	// { MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	// { MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{MODKEY, XK_h, setmfact, {.f = -0.05}},
	{MODKEY, XK_l, setmfact, {.f = +0.05}},
	{MODKEY, XK_Return, zoom, {0}},
	{MODKEY, XK_Tab, view, {0}},
	{MODKEY | ShiftMask, XK_c, killclient, {0}},
	{MODKEY, XK_t, setlayout, {.v = &layouts[0]}},
	{MODKEY, XK_f, setlayout, {.v = &layouts[1]}},
	{MODKEY, XK_m, setlayout, {.v = &layouts[2]}},
	{MODKEY, XK_space, setlayout, {0}},
	{MODKEY | ShiftMask, XK_space, togglefloating, {0}},
	//{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{MODKEY | ShiftMask, XK_0, tag, {.ui = ~0}},
	{MODKEY, XK_comma, focusmon, {.i = -1}},
	{MODKEY | ShiftMask, XK_comma, tagmon, {.i = -1}},
	{MODKEY | ShiftMask, XK_period, tagmon, {.i = +1}},
	{MODKEY | ShiftMask, XK_Right, shiftview, {.i = +1}},
	{MODKEY | ShiftMask, XK_Left, shiftview, {.i = -1}},
	TAGKEYS(XK_1, 0)
		TAGKEYS(XK_2, 1)
			TAGKEYS(XK_3, 2)
				TAGKEYS(XK_4, 3)
					TAGKEYS(XK_5, 4)
						TAGKEYS(XK_6, 5)
							TAGKEYS(XK_7, 6)
								TAGKEYS(XK_8, 7)
									TAGKEYS(XK_9, 8){MODKEY | ShiftMask, XK_q, quit, {0}},
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	//{ ClkStatusText,        0,              Button1,        sigdwmblocks,   {.i = 1} },
	{ClkLtSymbol, 0, Button1, setlayout, {0}},
	{ClkLtSymbol, 0, Button3, setlayout, {.v = &layouts[2]}},
	{ClkWinTitle, 0, Button2, zoom, {0}},
	{ClkStatusText, 0, Button2, spawn, {.v = termcmd}},
	{ClkClientWin, MODKEY, Button1, movemouse, {0}},
	{ClkClientWin, MODKEY, Button2, togglefloating, {0}},
	{ClkClientWin, MODKEY, Button3, resizemouse, {0}},
	{ClkTagBar, 0, Button1, view, {0}},
	{ClkTagBar, 0, Button3, toggleview, {0}},
	{ClkTagBar, MODKEY, Button1, tag, {0}},
	{ClkTagBar, MODKEY, Button3, toggletag, {0}},
};
