#include "grid.c"
#include <X11/XF86keysym.h>
/* appearance */
static const unsigned int borderpx = 2; /* border pixel of windows */
static const unsigned int gappx = 5;    /* gaps between windows */
static const unsigned int snap = 32;    /* snap pixel */
static const unsigned int minwsz =
    20;                           /* Minimal heigt of a client for smfact */
static const int showbar = 1;     /* 0 means no bar */
static const int topbar = 1;      /* 0 means bottom bar */
static const int horizpadbar = 0; /* horizontal padding for statusbar */
static const int vertpadbar = 0;  /* vertical padding for statusbar */
static const char *fonts[] = {
    "Mononoki Nerd Font:size=11:antialias=true:autohint=true",
    "Hack:size=8:antialias=true:autohint=true",
    "JoyPixels:size=10:antialias=true:autohint=true"};
static const char dmenufont[] = "monospace:size=10";
static const char bgselhid[] = "#282a36";
static const char border[] = "#444444";
static const char fgnorm[] = "#f1fa8c";
static const char fgsel[] = "#282a36";
static const char selhidfgbdborder[] = "#50fa7b";
static char selbordercolor[] = "#ff5555";

static const char *colors[][3] = {
    /*               fg         bg         border   */
    [SchemeNorm] = {fgnorm, bgselhid, border},
    [SchemeSel] = {fgsel, selhidfgbdborder, selbordercolor},
    [SchemeHid] = {selhidfgbdborder, bgselhid, selhidfgbdborder},

    [SchemeStatus] = {"#50fa7b", "#44475a",
                      "#000000"}, // Statusbar right {text,background,not used
    [SchemeTagsSel] = {"#282a36", "#ff5555",
                       "#000000"}, // Tagbar left selected {text,background,not
    [SchemeTagsNorm] =
        {fgnorm, "#44475a",
         "#000000"}, // Tagbar left unselected {text,background,not used but
    [SchemeInfoSel] =
        {fgsel, selhidfgbdborder,
         "#000000"}, // infobar middle  selected {text,background,not used but
    [SchemeInfoNorm] =
        {fgnorm, bgselhid,
         "#000000"}, // infobar middle  unselected {text,background,not used but

};

/* tagging */
static const char *tags[] = {"", "", "", "", "", "", "", "", ""};

static const Rule rules[] = {
    /* xprop(1):
     *	WM_CLASS(STRING) = instance, class
     *	WM_NAME(STRING) = title
     */
    /* class      instance    title       tags mask     isfloating   monitor */
    {"Gimp", NULL, NULL, 0, 1, -1},
    {"Firefox", NULL, NULL, 1 << 8, 0, -1},
};

/* layout(s) */
static const float mfact = 0.55;  /* factor of master area size [0.05..0.95] */
static const float smfact = 0.00; /* factor of tiled clients [0.00..0.95] */
static const int nmaster = 1;     /* number of clients in master area */
static const int resizehints =
    1; /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
    /* symbol     arrange function */
    {"[]=", tile},           /* first entry is default */
    {"><>", NULL},           /* no layout function means floating behavior */
    {"[M]", monocle},        /* Monocle*/
    {"|M|", centeredmaster}, /*  Centered Master */
    {">M>", centeredfloatingmaster}, /*  Centered Float */
    {"HHH", grid},                   /*  Grid */
    {"[D]", deck},                   /*  Deck  */
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY, TAG)                                                      \
  {MODKEY, KEY, view, {.ui = 1 << TAG}},                                       \
      {MODKEY | ControlMask, KEY, toggleview, {.ui = 1 << TAG}},               \
      {MODKEY | ShiftMask, KEY, tag, {.ui = 1 << TAG}},                        \
      {MODKEY | ControlMask | ShiftMask, KEY, toggletag, {.ui = 1 << TAG}},

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd)                                                             \
  {                                                                            \
    .v = (const char *[]) { "/bin/sh", "-c", cmd, NULL }                       \
  }

/* commands */
static char dmenumon[2] =
    "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = {"dmenu_run", "-m",  dmenumon,         "-fn",
                                 dmenufont,   "-nb", bgselhid,         "-nf",
                                 fgnorm,      "-sb", selhidfgbdborder, "-sf",
                                 fgsel,       NULL};
static const char *termcmd[] = {"st", NULL};

#include "mpdcontrol.c"

static Key keys[] = {
    /* modifier                     key        function        argument */
    {MODKEY, XK_d, spawn, SHCMD("rofi -modi \"drun,run\" -show drun")},
    // {MODKEY, XK_d, spawn, {.v = dmenucmd}},
    {MODKEY, XK_Return, spawn, {.v = termcmd}},
    {MODKEY, XK_b, togglebar, {0}},
    {MODKEY, XK_j, focusstack, {.i = +1}},
    {MODKEY, XK_k, focusstack, {.i = -1}},
    /*	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 }
       },*/
    {MODKEY | ShiftMask, XK_h, setmfact, {.f = -0.05}},
    {MODKEY | ShiftMask, XK_l, setmfact, {.f = +0.05}},
    {MODKEY | ShiftMask, XK_j, setsmfact, {.f = +0.05}},
    {MODKEY | ShiftMask, XK_k, setsmfact, {.f = -0.05}},
    {MODKEY | ShiftMask, XK_Return, zoom, {0}},
    {MODKEY, XK_Tab, view, {0}},
    {MODKEY, XK_c, killclient, {0}},
    {MODKEY, XK_t, setlayout, {.v = &layouts[0]}},
    {MODKEY, XK_f, setlayout, {.v = &layouts[1]}},
    {MODKEY, XK_m, setlayout, {.v = &layouts[2]}},
    {MODKEY, XK_u, setlayout, {.v = &layouts[3]}},
    {MODKEY, XK_o, setlayout, {.v = &layouts[4]}},
    {MODKEY, XK_i, setlayout, {.v = &layouts[5]}},
    {MODKEY, XK_n, setlayout, {.v = &layouts[6]}},
    {MODKEY, XK_space, setlayout, {0}},
    {MODKEY | ShiftMask, XK_space, togglefloating, {0}},
    {MODKEY | ShiftMask, XK_f, togglefullscr, {0}},
    {MODKEY, XK_0, view, {.ui = ~0}},
    {MODKEY | ShiftMask, XK_0, tag, {.ui = ~0}},
    {MODKEY, XK_comma, focusmon, {.i = -1}},
    {MODKEY, XK_period, focusmon, {.i = +1}},
    {MODKEY | ShiftMask, XK_comma, tagmon, {.i = -1}},
    {MODKEY | ShiftMask, XK_period, tagmon, {.i = +1}},
    {MODKEY, XK_minus, setgaps, {.i = -1}},
    {MODKEY, XK_equal, setgaps, {.i = +1}},
    {MODKEY | ShiftMask, XK_equal, setgaps, {.i = 0}},
    {MODKEY, XK_r, spawn, SHCMD("nautilus")},
    {MODKEY, XK_h, spawn, SHCMD("st -e gotop")},
    {MODKEY | ShiftMask, XK_b, spawn, SHCMD("st -e bluetoothctl")},
    {MODKEY, XK_w, spawn, SHCMD("brave")},
    {MODKEY | ShiftMask, XK_q, quit, {0}},
    {MODKEY, XK_x, setgaps, {.i = +3}},
    {MODKEY, XK_z, setgaps, {.i = -3}},
    {MODKEY, XK_a, setgaps, {.i = 0}},
    {MODKEY, XK_e, spawn, SHCMD("emacs")},
    {MODKEY | ShiftMask, XK_r, spawn, SHCMD("dm-tool switch-to-greeter")},
    TAGKEYS(XK_1, 0) TAGKEYS(XK_2, 1) TAGKEYS(XK_3, 2) TAGKEYS(XK_4, 3)
        TAGKEYS(XK_5, 4) TAGKEYS(XK_6, 5) TAGKEYS(XK_7, 6) TAGKEYS(XK_8, 7)
            TAGKEYS(XK_9, 8){0, XF86XK_AudioMute, spawn, SHCMD("pamixer -t")},
    {0, XF86XK_AudioRaiseVolume, spawn, SHCMD("pamixer --allow-boost -i 3")},
    {0, XF86XK_AudioLowerVolume, spawn, SHCMD("pamixer --allow-boost -d 3")},
    {0, XF86XK_MonBrightnessUp, spawn, SHCMD("light -A 15")},
    {0, XF86XK_MonBrightnessDown, spawn, SHCMD("light -U 15")},
    {0, XK_Print, spawn, SHCMD("flameshot gui")},
    {MODKEY, XK_F1, mpdchange, {.i = -1}},
    {MODKEY, XK_F2, mpdchange, {.i = +1}},
    {MODKEY, XK_Escape, mpdcontrol, {0}},
    {0, XF86XK_AudioPlay, spawn, SHCMD("mpc play")},
    {0, XF86XK_AudioStop, spawn, SHCMD("mpc stop")},
    {0, XF86XK_AudioRewind, spawn, SHCMD("mpc seek -10")},
    {0, XF86XK_AudioForward, spawn, SHCMD("mpc seek +10")},
    {0, XF86XK_RFKill, spawn, SHCMD("slock")},
    /* MOVE AND RESIZE KEYBINDINGS*/
    {MODKEY, XK_Down, moveresize, {.v = "0x 25y 0w 0h"}},
    {MODKEY, XK_Up, moveresize, {.v = "0x -25y 0w 0h"}},
    {MODKEY, XK_Right, moveresize, {.v = "25x 0y 0w 0h"}},
    {MODKEY, XK_Left, moveresize, {.v = "-25x 0y 0w 0h"}},
    {MODKEY | ShiftMask, XK_Down, moveresize, {.v = "0x 0y 0w 25h"}},
    {MODKEY | ShiftMask, XK_Up, moveresize, {.v = "0x 0y 0w -25h"}},
    {MODKEY | ShiftMask, XK_Right, moveresize, {.v = "0x 0y 25w 0h"}},
    {MODKEY | ShiftMask, XK_Left, moveresize, {.v = "0x 0y -25w 0h"}},
    {MODKEY | ControlMask, XK_Up, moveresizeedge, {.v = "t"}},
    {MODKEY | ControlMask, XK_Down, moveresizeedge, {.v = "b"}},
    {MODKEY | ControlMask, XK_Left, moveresizeedge, {.v = "l"}},
    {MODKEY | ControlMask, XK_Right, moveresizeedge, {.v = "r"}},
    {MODKEY | ControlMask | ShiftMask, XK_Up, moveresizeedge, {.v = "T"}},
    {MODKEY | ControlMask | ShiftMask, XK_Down, moveresizeedge, {.v = "B"}},
    {MODKEY | ControlMask | ShiftMask, XK_Left, moveresizeedge, {.v = "L"}},
    {MODKEY | ControlMask | ShiftMask, XK_Right, moveresizeedge, {.v = "R"}},
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
 * ClkClientWin, or ClkRootWin */
static Button buttons[] = {
    /* click                event mask      button          function argument
     */
    {ClkLtSymbol, 0, Button1, setlayout, {0}},
    {ClkLtSymbol, 0, Button3, setlayout, {.v = &layouts[2]}},
    {ClkWinTitle, 0, Button1, togglewin, {0}},
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
