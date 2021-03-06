#include "mpdcontrol.c"
#include <X11/XF86keysym.h>
/* appearance */
#define XDO "xdotool key --window $(xdotool search --classname spotify-tui) "
static const unsigned int borderpx = 2; /* border pixel of windows */
static const unsigned int gappx = 5;    /* gaps between windows */
static const unsigned int snap = 5;     /* snap pixel */
static const unsigned int systraypinning =
    0; /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor
          X */
static const unsigned int systrayspacing = 2; /* systray spacing */
static const int systraypinningfailfirst =
    1; /* 1: if pinning fails, display systray on the first monitor, False:
          display systray on the last monitor*/
static const int showsystray = 1; /* 0 means no systray */
static const unsigned int minwsz =
    20;                           /* Minimal heigt of a client for smfact */
static const int showbar = 1;     /* 0 means no bar */
static const int topbar = 1;      /* 0 means bottom bar */
static const int horizpadbar = 2; /* horizontal padding for statusbar */
static const int vertpadbar = 2;  /* vertical padding for statusbar */

static const char *fonts[] = {
    "Mononoki Nerd Font:style=Bold:size=11:antialias=true:autohint=true",
    "Hack:size=8:antialias=true:autohint=true",
    "JoyPixels:size=10:antialias=true:autohint=true"};

static const char dmenufont[] = "monospace:size=10";

static const char norm_fg[] = "#a89984";
static const char norm_bg[] = "#282828";
static const char norm_border[] = "#928374";

static const char sel_fg[] = "#282828";
static const char sel_bg[] = "#d79921";
static const char sel_border[] = "#d79921";

static const char urg_fg[] = "#a89984";
static const char urg_bg[] = "#cc241d";
static const char urg_border[] = "#b8bb26";

static const char title_fg[] = "#b8bb26";
static const char title_bg[] = "#3A3A3A";

static const char col_borderbar[] = "#75715e";

static const char *colors[][3] = {
    /*               fg           bg         border                         */
    [SchemeNorm] = {norm_fg, norm_bg, norm_border}, // unfocused wins
    [SchemeSel] = {sel_fg, sel_bg, sel_border},     // the focused win
    //[SchemeUrg] =  { urg_fg,      urg_bg,    urg_border },
    [SchemeHid] = {title_fg, title_bg, norm_border},

    [SchemeStatus] = {norm_fg, norm_bg,
                      "#000000"}, // Statusbar right {text,background,not used
    [SchemeTagsSel] = {sel_fg, sel_bg,
                       "#000000"}, // Tagbar left selected {text,background,not
    [SchemeTagsNorm] =
        {norm_fg, norm_bg,
         "#000000"}, // Tagbar left unselected {text,background,not used but
    [SchemeInfoSel] =
        {sel_fg, sel_fg,
         "#000000"}, // infobar middle  selected {text,background,not used but
    [SchemeInfoNorm] =
        {norm_fg, norm_bg,
         "#000000"}, // infobar middle  unselected {text,background,not used but
};

/* tagging */
// static const char *tags[] = {"", "", "", "", "", "", "", "", ""};
static const char *tags[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};
// static const char *tags[] = {"Ⅰ", "Ⅱ", "Ⅲ", "Ⅳ", "Ⅴ", "Ⅵ", "Ⅶ", "Ⅷ", "Ⅸ"};

static const Rule rules[] = {
    /* xprop(1):
     *  WM_CLASS(STRING) = instance, class
     *  WM_NAME(STRING) = title
     */
    /* class      instance    title       tags mask     isfloating   monitor */
    {"Gimp", NULL, NULL, 0, 1, -1},
    {"Emacs", NULL, "capture", 0, 1, -1}};

/* layout(s) */
static const float mfact = 0.5;   /* factor of master area size [0.05..0.95] */
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
static const char *dmenucmd[] = {
    "dmenu_run", "-m",    dmenumon, "-fn",      dmenufont, "-nb",  sel_bg,
    "-nf",       norm_fg, "-sb",    sel_border, "-sf",     sel_fg, NULL};
static const char *termcmd[] = {"st", NULL};

static Key keys[] = {
    /* modifier                     key        function        argument */

    // DMENU AND ROFI
    {MODKEY, XK_p, spawn, SHCMD("rofi -modi \"drun,run\" -show drun")},
    {MODKEY, XK_d, spawn, SHCMD("dmenu_run -c -l 15")},
    {MODKEY, XK_Return, spawn, SHCMD("tabbed -r 2 st -w ''")},

    {MODKEY, XK_b, togglebar, {0}},
    {MODKEY | ShiftMask, XK_q, quit, {.i = 23}},

    {MODKEY | ShiftMask, XK_h, setmfact, {.f = -0.05}},
    {MODKEY | ShiftMask, XK_l, setmfact, {.f = +0.05}},
    {MODKEY | ShiftMask, XK_j, setsmfact, {.f = +0.05}},
    {MODKEY | ShiftMask, XK_k, setsmfact, {.f = -0.05}},

    {MODKEY | ShiftMask, XK_Return, zoom, {0}},
    {MODKEY, XK_Tab, view, {0}},
    {MODKEY, XK_c, killclient, {0}},

    // LAYOUT, GAPS AND FLOAT MODE
    {MODKEY, XK_t, setlayout, {.v = &layouts[0]}},
    {MODKEY, XK_f, setlayout, {.v = &layouts[1]}},
    {MODKEY, XK_m, setlayout, {.v = &layouts[2]}},
    {MODKEY, XK_u, setlayout, {.v = &layouts[3]}},
    {MODKEY, XK_o, setlayout, {.v = &layouts[4]}},
    {MODKEY, XK_i, setlayout, {.v = &layouts[5]}},
    {MODKEY, XK_n, setlayout, {.v = &layouts[6]}},
    {MODKEY, XK_space, setlayout, {0}},
    {MODKEY, XK_x, setgaps, {.i = +3}},
    {MODKEY, XK_z, setgaps, {.i = -3}},

    // {MODKEY, XK_a, setgaps, {.i = 0}},
    {MODKEY | ShiftMask, XK_space, togglefloating, {0}},
    {MODKEY | ShiftMask, XK_f, togglefullscr, {0}},

    {MODKEY, XK_0, view, {.ui = ~0}},
    {MODKEY | ShiftMask, XK_0, tag, {.ui = ~0}},

    // MY KEYBINDINGS
    {MODKEY, XK_r, spawn, SHCMD("nautilus")},
    {MODKEY | ShiftMask, XK_r, spawn, SHCMD("st -e ranger")},
    {MODKEY | ShiftMask, XK_b, spawn, SHCMD("st -e bluetoothctl")},
    {MODKEY, XK_w, spawn, SHCMD("firefox")},
    {MODKEY, XK_e, spawn,
     SHCMD("emacsclient -c --eval "
           "\"(progn(+workspace:delete)(+workspace/switch-to-final))\" -q")},
    /* {MODKEY, XK_e, spawn, SHCMD("emacsclient -a \"\" -c -q")}, */
    {MODKEY, XK_s, spawn, SHCMD("tabbed surf -L -pe")},
    {MODKEY | ShiftMask, XK_s, spawn, SHCMD("st -n spotify-tui -e \"spt\"")},
    {MODKEY, XK_g, spawn,
     SHCMD("emacsclient -c -F '(quote (name . \"capture\"))' -e "
           "'(activate-capture-frame)'")},
    {0, XK_Print, spawn, SHCMD("flameshot gui")},
    {MODKEY, XK_v, spawn, SHCMD("copyq toggle")},
    // TAGKEYS
    TAGKEYS(XK_1, 0) TAGKEYS(XK_2, 1) TAGKEYS(XK_3, 2) TAGKEYS(XK_4, 3)
        TAGKEYS(XK_5, 4) TAGKEYS(XK_6, 5) TAGKEYS(XK_7, 6) TAGKEYS(XK_8, 7)
            TAGKEYS(XK_9, 8)

    // AUDIO AND BRIGHTNESS
    {0, XF86XK_AudioMute, spawn, SHCMD("pamixer -t")},
    {0, XF86XK_AudioRaiseVolume, spawn, SHCMD("pamixer --allow-boost -i 2")},
    {0, XF86XK_AudioLowerVolume, spawn, SHCMD("pamixer --allow-boost -d 2")},
    {0, XF86XK_AudioPrev, spawn, SHCMD(XDO "p")},
    {0, XF86XK_AudioNext, spawn, SHCMD(XDO "n")},
    {0, XF86XK_AudioPause, spawn, SHCMD(XDO "space")},
    {0, XF86XK_AudioPlay, spawn, SHCMD(XDO "space")},

    {0, XF86XK_MonBrightnessUp, spawn, SHCMD("light -A 10")},
    {0, XF86XK_MonBrightnessDown, spawn, SHCMD("light -U 10")},

    {0, XF86XK_RFKill, spawn, SHCMD("betterlockscreen -l")},

    {MODKEY, XK_h, focusdir, {.i = 0}}, // left
    {MODKEY, XK_l, focusdir, {.i = 1}}, // right
    {MODKEY, XK_k, focusdir, {.i = 2}}, // up
    {MODKEY, XK_j, focusdir, {.i = 3}}, // down
    /* MOVE AND RESIZE KEYBINDINGS*/
    {MODKEY, XK_Down, moveresize, {.v = "0x 25y 0w 0h"}},
    {MODKEY, XK_Up, moveresize, {.v = "0x -25y 0w 0h"}},
    {MODKEY, XK_Right, moveresize, {.v = "25x 0y 0w 0h"}},
    {MODKEY, XK_Left, moveresize, {.v = "-25x 0y 0w 0h"}},
    {MODKEY | ShiftMask, XK_Down, moveresize, {.v = "0x 0y 0w 15h"}},
    {MODKEY | ShiftMask, XK_Up, moveresize, {.v = "0x 0y 0w -15h"}},
    {MODKEY | ShiftMask, XK_Right, moveresize, {.v = "0x 0y 15w 0h"}},
    {MODKEY | ShiftMask, XK_Left, moveresize, {.v = "0x 0y -15w 0h"}},
    {MODKEY | ControlMask, XK_Up, moveresizeedge, {.v = "t"}},
    {MODKEY | ControlMask, XK_Down, moveresizeedge, {.v = "b"}},
    {MODKEY | ControlMask, XK_Left, moveresizeedge, {.v = "l"}},
    {MODKEY | ControlMask, XK_Right, moveresizeedge, {.v = "r"}},
    {MODKEY | ControlMask | ShiftMask, XK_Up, moveresizeedge, {.v = "T"}},
    {MODKEY | ControlMask | ShiftMask, XK_Down, moveresizeedge, {.v = "B"}},
    {MODKEY | ControlMask | ShiftMask, XK_Left, moveresizeedge, {.v = "L"}},
    {MODKEY | ControlMask | ShiftMask, XK_Right, moveresizeedge, {.v = "R"}},

    /* Multi Monitor*/
    {MODKEY, XK_comma, focusmon, {.i = -1}},
    {MODKEY, XK_period, focusmon, {.i = +1}},
    {MODKEY | ShiftMask, XK_comma, tagmon, {.i = -1}},
    {MODKEY | ShiftMask, XK_period, tagmon, {.i = +1}},

    {MODKEY, XK_comma, spawn,
     SHCMD("python3 /home/penguin/Desktop/cursor.py left")},
    {MODKEY, XK_period, spawn,
     SHCMD("python3 /home/penguin/Desktop/cursor.py right")},
    {MODKEY | ShiftMask, XK_comma, spawn,
     SHCMD("python3 /home/penguin/Desktop/cursor.py left")},
    {MODKEY | ShiftMask, XK_period, spawn,
     SHCMD("python3 /home/penguin/Desktop/cursor.py right")},
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
