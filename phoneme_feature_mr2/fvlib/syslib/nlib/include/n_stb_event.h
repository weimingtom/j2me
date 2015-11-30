#ifndef __N_STB_EVENT_H__
#define __N_STB_EVENT_H__

#include "n_stb_mod.h"
#include "n_stb_service.h"
#include "n_stb_errorcode.h"

#define N_CTRL_DOWN  0X00000001
#define N_SHIFT_DOWN 0x00000002
#define N_ALT_DOWN   0x00000004
#define IS_CTRL_DOWN(x)  ((x) & N_CTRL_DOWN)
#define IS_SHIFT_DOWN(x) ((x) & N_SHIFT_DOWN)
#define IS_ALT_DOWN(x)   ((x) & N_ALT_DOWN)

typedef enum {
    N_EVT_IR = 0,
    N_EVT_FIP,
    N_EVT_KEYBOARD,
    N_EVT_FAKE_IR ,
    N_EVT_JOY_IR,
    N_EVT_MOUSE
} n_stb_evt_type_e;

typedef enum {
	N_MOUSE_BTN_DEFAULT = 0,
	N_MOUSE_BTN_LEFT,
	N_MOUSE_BTN_MIDDLE,
	N_MOUSE_BTN_RIGHT,
	N_MOUSE_BTN_NULL
} n_evt_mouse_e;

typedef enum {
	N_JOY_BTN_UP = 0,
	N_JOY_BTN_DOWN,
	N_JOY_BTN_LEFT,
	N_JOY_BTN_RIGHT,
	N_JOY_BTN_SELECT
} n_evt_joy_axis_state;

typedef enum {
	N_GAMEPAD_BTN_A=0,
	N_GAMEPAD_BTN_B,
	N_GAMEPAD_BTN_C,
	N_GAMEPAD_BTN_D,
	N_GAMEPAD_BTN_L1,
	N_GAMEPAD_BTN_R1,
	N_GAMEPAD_BTN_L2,
	N_GAMEPAD_BTN_R2,
	N_GAMEPAD_BTN_SELECT,
	N_GAMEPAD_BTN_START,
	N_GAMEPAD_BTN_LA,
	N_GAMEPAD_BTN_RA,
	N_GAMEPAD_BTN_MODE,
	N_GAMEPAD_BTN_TURBO,
	N_GAMEPAD_BTN_CLEAR,
	N_GAEMPAD_AXIS_POV0,
	N_GAMEPAD_AXIS_X,
	N_GAMEPAD_AXIS_Y,
	N_GAMEPAD_AXIS_Z,
	N_GAMEPAD_AXIS_Z_ROTATION,
	N_GAMEPAD_NULL
} n_evt_joy_e;

typedef enum {
	N_EVT_KB_ESC = 0,
	N_EVT_KB_F1,
	N_EVT_KB_F2,
	N_EVT_KB_F3,
	N_EVT_KB_F4,
	N_EVT_KB_F5,
	N_EVT_KB_F6,
	N_EVT_KB_F7,
	N_EVT_KB_F8,
	N_EVT_KB_F9,
	N_EVT_KB_F10,
	N_EVT_KB_F11,
	N_EVT_KB_F12,
	N_EVT_KB_A,
	N_EVT_KB_B,
	N_EVT_KB_C,
	N_EVT_KB_D,
	N_EVT_KB_E,
	N_EVT_KB_F,
	N_EVT_KB_G,
	N_EVT_KB_H,
	N_EVT_KB_I,
	N_EVT_KB_J,
	N_EVT_KB_K,
	N_EVT_KB_L,
	N_EVT_KB_M,
	N_EVT_KB_N,
	N_EVT_KB_O,
	N_EVT_KB_P,
	N_EVT_KB_Q,
	N_EVT_KB_R,
	N_EVT_KB_S,
	N_EVT_KB_T,
	N_EVT_KB_U,
	N_EVT_KB_V,
	N_EVT_KB_W,
	N_EVT_KB_X,
	N_EVT_KB_Y,
	N_EVT_KB_Z,
	N_EVT_KB_NUM1,
	N_EVT_KB_NUM2,
	N_EVT_KB_NUM3,
	N_EVT_KB_NUM4,
	N_EVT_KB_NUM5,
	N_EVT_KB_NUM6,
	N_EVT_KB_NUM7,
	N_EVT_KB_NUM8,
	N_EVT_KB_NUM9,
	N_EVT_KB_NUM0,
	N_EVT_KB_UP,
	N_EVT_KB_DOWN,
	N_EVT_KB_LEFT,
	N_EVT_KB_RIGHT,
	N_EVT_KB_SPACE,
	N_EVT_KB_BACK,
	N_EVT_KB_ENTER,
	N_EVT_KB_LEFT_SHIFT,
	N_EVT_KB_RIGHT_SHIFT,
	N_EVT_KB_LEFT_CTRL,
	N_EVT_KB_RIGHT_CTRL,
	N_EVT_KB_LEFT_ALT,
	N_EVT_KB_RIGHT_ALT,
	N_EVT_KB_TAB,
	N_EVT_KB_CAPSLOCK,
	N_EVT_KB_PRINTFSCREENSYSRQ,
	N_EVT_KB_SCROLLLOCK,
	N_EVT_KB_PAUSEBREAK,
	N_EVT_KB_HOME,
	N_EVT_KB_END,
	N_EVT_KB_INSERT,
	N_EVT_KB_PAGEUP,
	N_EVT_KB_DELETE,
	N_EVT_KB_PAGEDOWN,
	N_EVT_KB_NUMLOCK,
	N_EVT_KB_PLUS,
	N_EVT_KB_LEFT_MINUS,
	N_EVT_KB_MULTIPLY,
	N_EVT_KB_LEFT_DIVIDE,
	N_EVT_KB_EQUAL,
	N_EVT_KB_COMMA,
	N_EVT_KB_PERIOD,
	N_EVT_KB_SEMICOLON,
	N_EVT_KB_APOSTROPHE,
	N_EVT_KB_WINDOWS,
	N_EVT_KB_BACKSLASH,
	N_EVT_KB_OPEN_BRACKET,
	N_EVT_KB_CLOSE_BRACKET,
	N_EVT_KB_SINGAL_QUOTATION,
	N_EVT_KB_RIGHT_CLICK,
	N_EVT_LITTLE_KB_DEVIDE,
	N_EVT_LITTLE_KB_MULTIPLY,
	N_EVT_LITTLE_KB_MINUS,
	N_EVT_LITTLE_KB_PLUS,
	N_EVT_LITTLE_KB_ENTER,
	N_EVT_LITTLE_KB_DEL,
	N_EVT_LITTLE_KB_0,
	N_EVT_LITTLE_KB_1,
	N_EVT_LITTLE_KB_2,
	N_EVT_LITTLE_KB_3,
	N_EVT_LITTLE_KB_4,
	N_EVT_LITTLE_KB_5,
	N_EVT_LITTLE_KB_6,
	N_EVT_LITTLE_KB_7,
	N_EVT_LITTLE_KB_8,
	N_EVT_LITTLE_KB_9,
	N_EVT_KB_NULL,
	N_EVT_KB_MAX
} n_evt_keyboard_e;

typedef enum {
	N_IRK_BACK = 0,
	N_IRK_DELETE,
	N_IRK_SELECT,
	N_IRK_UP,
	N_IRK_DOWN,
	N_IRK_LEFT,
	N_IRK_RIGHT,
	N_IRK_NUM0,
	N_IRK_NUM1,
	N_IRK_NUM2,
	N_IRK_NUM3,
	N_IRK_NUM4,
	N_IRK_NUM5,
	N_IRK_NUM6,
	N_IRK_NUM7,
	N_IRK_NUM8,
	N_IRK_NUM9,
	N_IRK_POUND,
	N_IRK_STAR,
	N_IRK_IME,
	N_IRK_SOFT_KEYBOARD2,
	N_IRK_POWER,
	N_IRK_CHANNEL_UP,
	N_IRK_CHANNEL_DOWN,
	N_IRK_SWITCH,
	N_IRK_CHANNEL_POS,
	N_IRK_VOLUME_UP,
	N_IRK_VOLUME_DOWN,
	N_IRK_VOLUME_MUTE,
	N_IRK_VOLUME_TRACK,
	N_IRK_PLAY,
	N_IRK_FASTFORWARD,
	N_IRK_REWIND,
	N_IRK_GO_END,
	N_IRK_GO_BEGINNING,
	N_IRK_STOP,
	N_IRK_POS,
	N_IRK_PORTAL,
	N_IRK_RED,
	N_IRK_GREEN,
	N_IRK_YELLOW,
	N_IRK_BLUE,
	N_IRK_GREY,
	N_IRK_HELP,
	N_IRK_PAGE_UP,
	N_IRK_PAGE_DOWN,
	N_IRK_FAVORITE,
	N_IRK_VK_F1,
	N_IRK_VK_F2,
	N_IRK_VK_F3,
	N_IRK_VK_F4,
	N_IRK_FACTORY_TEST,
	N_IRK_FACTORY_AGING,
	N_IRK_IPTV_VIRTUAL,
	N_IRK_CLOUDUNION,
	N_IRK_LOCAL,
	N_IRK_NULL,
	N_IRK_MAX
} n_ir_e;

typedef enum
{
	N_DEV_FIRST_KEY,
	N_DEV_REPEAT_KEY,
	N_DEV_RELEASE_KEY
} n_dev_key_status;


/*
 * module event
 */

/**
 \param[in]	id	int, module id
 \param[in]	stat	n_mod_stat_e, module status
*/
#define F_EVT_MODULE_STATUS	"F_EVT_MODULE_STATUS"

/**
 \param[in]	id	int, module id
 \param[in]	error	n_stb_error_code_e, module error
*/
#define F_EVT_STB_ERROR	"F_EVT_STB_ERROR"

/*
 * network event
 */
/**
 \param[in]	stat	f_net_stat_e, net status
*/
#define F_EVT_NET_STATUS	"F_EVT_NET_STATUS"

/*
 * player event
 */
#define F_EVT_STREAM_STAT	"F_EVT_STREAM_STAT"
#define F_EVT_MEDIA_STAT	"F_EVT_MEDIA_STAT"
#define F_EVT_PLAYMODE_STAT	"F_EVT_PLAYMODE_STAT"
#define F_EVT_MEDIA_BEGINNING   "F_EVT_MEDIA_BEGINNING"

int n_int_mp_media_event(int id, int mp_id, int evt);
int n_int_mp_playmode_update(int id, int mp_id, int playmode, int scale, int reason);
int n_int_mp_media_beginning(int id, int mp_id, const char *media_code, const char *entry_id);
int n_stb_notify_module_status(int stat);
int n_stb_notify_module_error(int err);
int n_stb_mod_cmd(int mod_id, int cmd);

#endif

