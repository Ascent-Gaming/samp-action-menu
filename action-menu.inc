#if defined _inc_action_menu
	#undef _inc_action_menu
#endif

#if defined _action_menu_included
    #endinput
#endif
#define _action_menu_included

/*
		----------------
		Credits

		BigETI - `pawn-memory` include; it's really quite beautiful, thank you <3.
		Southclaws - Inspiration from his `action-text` include from S&S.
		Y_Less (and others, you know) - YSI Library
		----------------
*/

#include <a_samp>
#include <memory>

#include <YSI_Coding\y_hooks>

#define INVALID_ACTION_MENU (ActionMenu:-1)

// Configuration
// --
// TextDraws
#if !defined ACMEN_MAX_DISPLAY_ROWS
	#define ACMEN_MAX_DISPLAY_ROWS (10)
#endif

#if !defined ACMEN_MAX_NUMBER_ROWS
	#define ACMEN_MAX_NUMBER_ROWS (45)
#endif

#if !defined ACMEN_MAX_ROW_LENGTH
	#define ACMEN_MAX_ROW_LENGTH (100)
#endif

#if !defined ACMEN_TXD_BACKGROUND
	#define ACMEN_TXD_BACKGROUND ("LD_DUAL:white")
#endif

#if !defined ACMEN_TXD_TITLE_BG
	#define ACMEN_TXD_TITLE_BG (ACMEN_TXD_BACKGROUND)
#endif

#if !defined ACMEN_TXD_ROW_BG
	#define ACMEN_TXD_ROW_BG (ACMEN_TXD_BACKGROUND)
#endif

#if !defined ACMEN_TXD_OFFSET_BG
	#define ACMEN_TXD_OFFSET_BG (ACMEN_TXD_BACKGROUND)
#endif

#if !defined ACMEN_TXD_OFFSET_UP
	#define ACMEN_TXD_OFFSET_UP ("LD_BEAT:up")
#endif

#if !defined ACMEN_TXD_OFFSET_DOWN
	#define ACMEN_TXD_OFFSET_DOWN ("LD_BEAT:down")
#endif

// Values
#if !defined ACMEN_START_POS
	#define ACMEN_START_POS (Float:150.00)
#endif

#if !defined ACMEN_OFFSET_VALUE
	#define ACMEN_OFFSET_VALUE (Float:20.000)
#endif

#if !defined ACMEN_TXD_OFFSET_X_UP
	#define ACMEN_TXD_OFFSET_X_UP (Float:502.000)
#endif

#if !defined ACMEN_TXD_OFFSET_X_DOWN
	#define ACMEN_TXD_OFFSET_X_DOWN (Float:568.000)
#endif

#if !defined ACMEN_TXD_OFFSET_Y
	#define ACMEN_TXD_OFFSET_Y (Float:3.000)
#endif

#if !defined ACMEN_TXD_OFFSET_SIZE_X
	#define ACMEN_TXD_OFFSET_SIZE_X (Float:14.000)
#endif

#if !defined ACMEN_TXD_OFFSET_SIZE_Y
	#define ACMEN_TXD_OFFSET_SIZE_Y (Float:14.000)
#endif

// Colors
#if !defined ACMEN_COLOR_PRIMARY
	#define ACMEN_COLOR_PRIMARY (0xFF8000FF)
#endif

#if !defined ACMEN_COLOR_BG
	#define ACMEN_COLOR_BG (0x000000FF)
#endif

#if !defined ACMEN_LISTITEM_BG_OPPACITY
	#define ACMEN_LISTITEM_BG_OPPACITY (0x000000BF)
#endif

#if !defined ACMEN_COLOR_TITLE
	#define ACMEN_COLOR_TITLE (ACMEN_COLOR_PRIMARY)
#endif

#if !defined ACMEN_COLOR_TITLE_OUTLINE
	#define ACMEN_COLOR_TITLE_OUTLINE  (0x000000FF)
#endif

#if !defined ACMEN_COLOR_SUBTITLE
	#define ACMEN_COLOR_SUBTITLE (0xFFFFFFFF)
#endif

#if !defined COLOR_LIST_ITEM_BG
	#define COLOR_LIST_ITEM_BG (0x000000BF)
#endif

#if !defined ACMEN_COLOR_LISTITEM
	#define ACMEN_COLOR_LISTITEM (0xFFFFFFFF)
#endif

// Forward Declarations
// --
forward ActionMenu_OnPlayerResponse(playerid, response, listitem, const inputtext[]);
forward ActionMenu_Show(playerid, const title[], const subtitle[], const info[], bool:hideOnSelect = true);
forward ActionMenu_Hide(playerid);
forward _ActionMenu_ClenupTDs(playerid);
forward _ActionMenu_Shift(playerid, offset);
forward _ActionMenu_GetListItemValue(playerid, textdrawIndex, &listitemIndex, value[], valueSize = sizeof value);
forward PlayerText:_ActionMenu_CreateTitleBGTD(playerid);
forward PlayerText:_ActionMenu_CreateTitleTD(playerid, const title[]);
forward PlayerText:_ActionMenu_CreateSubtitleTD(playerid, const subtitle[]);
forward PlayerText:_ActionMenu_CreateRowBGTD(playerid, offset);
forward PlayerText:_ActionMenu_CreateRowTD(playerid, offset, const row[]);
forward Text:_ActionMenu_CreateDownBG();
forward Text:_ActionMenu_CreateDownArrow();
forward Text:_ActionMenu_CreateUpBG();
forward Text:_ActionMenu_CreateUpArrow();

enum {
	ACMEN_ERR_NONE = 0,
	ACMEN_ERR_INVALID_ACTIONMENU,
	ACMEN_ERR_INVALID_PLAYER,
	ACMEN_ERR_INVALID_DATA,
	ACMEN_ERR_MENU_SHOWN,
}

static
	AM_PlayerOffset[MAX_PLAYERS],
	AM_PlayerMaxOffset[MAX_PLAYERS],
	Pointer:AM_PlayerListitem[MAX_PLAYERS];

// Textdraws
// --
static
	Text:TD_AM_Down_BG,
	Text:TD_AM_Down_Arrow,
	Text:TD_AM_Up_BG,
	Text:TD_AM_Up_Arrow;

static
	PlayerText:TD_AM_TitleBG[MAX_PLAYERS],
	PlayerText:TD_AM_Title[MAX_PLAYERS],
	PlayerText:TD_AM_Subtitle[MAX_PLAYERS],
	PlayerText:TD_AM_Row[MAX_PLAYERS][ACMEN_MAX_DISPLAY_ROWS],
	PlayerText:TD_AM_RowBG[MAX_PLAYERS][ACMEN_MAX_DISPLAY_ROWS],
	PlayerText:TD_AM_Marker[MAX_PLAYERS];

// Hooks
// --
hook OnGameModeInit() {
	TD_AM_Down_BG = _ActionMenu_CreateDownBG();
	TD_AM_Down_Arrow = _ActionMenu_CreateDownArrow();
	//
	TD_AM_Up_BG = _ActionMenu_CreateUpBG();
	TD_AM_Up_Arrow = _ActionMenu_CreateUpArrow();
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerConnect(playerid) {
	AM_PlayerListitem[playerid] = MEM_NULLPTR;
	AM_PlayerOffset[playerid] = -1;
	AM_PlayerMaxOffset[playerid] = 0;
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerDisconnect(playerid, reason) {
	AM_PlayerListitem[playerid] = MEM_NULLPTR;
	AM_PlayerOffset[playerid] = -1;
	AM_PlayerMaxOffset[playerid] = 0;
	_ActionMenu_ClenupTDs(playerid);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerClickTD(playerid, Text:textid) {
	if (AM_PlayerOffset[playerid] != -1) {
		if (textid == TD_AM_Down_BG && AM_PlayerOffset[playerid] < AM_PlayerMaxOffset[playerid]) {
			_ActionMenu_Shift(playerid, ++AM_PlayerOffset[playerid]);
			return Y_HOOKS_BREAK_RETURN_1;
		}

		else if (textid == TD_AM_Up_BG && AM_PlayerOffset[playerid] != 0) {
			_ActionMenu_Shift(playerid, --AM_PlayerOffset[playerid]);
			return Y_HOOKS_BREAK_RETURN_1;
		}

		return Y_HOOKS_CONTINUE_RETURN_1;
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerClickPlayerTD(playerid, PlayerText:playertextid) {
	if (AM_PlayerOffset[playerid] != -1) {
		new loopMax = ACMEN_MAX_DISPLAY_ROWS,
				listitem[ACMEN_MAX_ROW_LENGTH],
				selectedIndex,
				listitemIndex;

		if (AM_PlayerMaxOffset[playerid] == 0) {
			loopMax = AM_PlayerMaxOffset[playerid];
		}

		for(new i = 0; i < loopMax; i++) {
			if (playertextid == TD_AM_RowBG[playerid][i]) {
				selectedIndex = i;
				break;
			}

			continue;
		}

		_ActionMenu_GetListItemValue(playerid, selectedIndex, listitemIndex, listitem);

		CallRemoteFunction("ActionMenu_OnPlayerResponse", "ddds", playerid, true, listitemIndex, listitem);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

// Forward Implementations
// --
stock ActionMenu_Show(playerid, const title[], const subtitle[], const info[], bool:hideOnSelect = true) {
	#pragma unused hideOnSelect

	if (!IsPlayerConnected(playerid))
		return ACMEN_ERR_INVALID_PLAYER;

	if (isnull(title) || isnull(subtitle) || isnull(info))
		return ACMEN_ERR_INVALID_DATA;

	if (AM_PlayerOffset[playerid] != -1)
		return ACMEN_ERR_MENU_SHOWN;

	// Title Textdraw
	TD_AM_TitleBG[playerid] = _ActionMenu_CreateTitleBGTD(playerid);
	PlayerTextDrawShow(playerid, TD_AM_TitleBG[playerid]);

	TD_AM_Title[playerid] = _ActionMenu_CreateTitleTD(playerid, title);
	PlayerTextDrawShow(playerid, TD_AM_Title[playerid]);

	TD_AM_Subtitle[playerid] = _ActionMenu_CreateSubtitleTD(playerid, subtitle);
	PlayerTextDrawShow(playerid, TD_AM_Subtitle[playerid]);

	// Pointer and Info listitem extraction
	AM_PlayerListitem[playerid] = MEM_new(ACMEN_MAX_ROW_LENGTH);

	new
		count = 0,
		pos = 0,
		listitem[ACMEN_MAX_ROW_LENGTH];

	new
		Pointer:listitemPointer;

	while(pos != -1) {
		new
			next = strfind(info, "\n", false, pos);

		strmid(listitem, info, pos, (next == -1 ? strlen(info) : next), ACMEN_MAX_ROW_LENGTH);

		pos = (next == -1) ? -1 : (next + 1);

		listitemPointer = MEM_new_arr(listitem, ACMEN_MAX_ROW_LENGTH);

		// If info[] contains more than 100 elements, resize the allocated memory to account for the change.
		if (count > ACMEN_MAX_NUMBER_ROWS) {
			new rowCount = (ACMEN_MAX_NUMBER_ROWS + (count - ACMEN_MAX_NUMBER_ROWS));

			new Pointer:tmpPointer = MEM_new(rowCount);
			MEM_copy(tmpPointer, AM_PlayerListitem[playerid], rowCount);

			MEM_delete(AM_PlayerListitem[playerid]);

			AM_PlayerListitem[playerid] = tmpPointer;
		}

		// Create rows to be displayed
		if (count < ACMEN_MAX_DISPLAY_ROWS) {
			TD_AM_RowBG[playerid][count] = _ActionMenu_CreateRowBGTD(playerid, count);
			PlayerTextDrawShow(playerid, TD_AM_RowBG[playerid][count]);

			TD_AM_Row[playerid][count] = _ActionMenu_CreateRowTD(playerid, count, listitem);
			PlayerTextDrawShow(playerid, TD_AM_Row[playerid][count]);
		}

		MEM_set_val(AM_PlayerListitem[playerid], count++, _:listitemPointer);
	}

	if (count >= ACMEN_MAX_DISPLAY_ROWS) {
		TextDrawShowForPlayer(playerid, TD_AM_Down_BG);
		TextDrawShowForPlayer(playerid, TD_AM_Up_BG);

		TextDrawShowForPlayer(playerid, TD_AM_Down_Arrow);
		TextDrawShowForPlayer(playerid, TD_AM_Up_Arrow);
	}

	new countDifference = (count - ACMEN_MAX_DISPLAY_ROWS);
	AM_PlayerMaxOffset[playerid] = (count >= 0 ? countDifference : 0);
	AM_PlayerOffset[playerid] = 0;

	SelectTextDraw(playerid, ACMEN_COLOR_PRIMARY);
	return ACMEN_ERR_NONE;
}

stock ActionMenu_Hide(playerid) {
	_ActionMenu_ClenupTDs(playerid);
	AM_PlayerListitem[playerid] = MEM_NULLPTR;
	AM_PlayerMaxOffset[playerid] = 0;
	AM_PlayerOffset[playerid] = -1;
	MEM_delete(AM_PlayerListitem[playerid]);
	CancelSelectTextDraw(playerid);
	return ACMEN_ERR_NONE;
}

// Internal Functions
// --
stock _ActionMenu_ClenupTDs(playerid) {
	TextDrawHideForPlayer(playerid, TD_AM_Down_BG);
	TextDrawHideForPlayer(playerid, TD_AM_Down_Arrow);
	TextDrawHideForPlayer(playerid, TD_AM_Up_BG);
	TextDrawHideForPlayer(playerid, TD_AM_Up_Arrow);
	PlayerTextDrawDestroy(playerid, TD_AM_TitleBG[playerid]);
	PlayerTextDrawDestroy(playerid, TD_AM_Title[playerid]);
	PlayerTextDrawDestroy(playerid, TD_AM_Subtitle[playerid]);
	PlayerTextDrawDestroy(playerid, TD_AM_Marker[playerid]);
	for (new i = 0; i < ACMEN_MAX_DISPLAY_ROWS; i++) {
		if (TD_AM_RowBG[playerid][i]) {
			PlayerTextDrawDestroy(playerid, TD_AM_RowBG[playerid][i]);
		}

		if (TD_AM_Row[playerid][i]) {
			PlayerTextDrawDestroy(playerid, TD_AM_Row[playerid][i]);
		}
	}
	return ACMEN_ERR_NONE;
}

stock _ActionMenu_Shift(playerid, offset) {
	if (offset > AM_PlayerMaxOffset[playerid] || offset < 0)
		return ACMEN_ERR_INVALID_DATA;

	new shown = 0,
			Pointer:pointer = AM_PlayerListitem[playerid],
			Pointer:tmpListitemPointer = MEM_NULLPTR,
			str[ACMEN_MAX_ROW_LENGTH];

	MEM_foreach(index : pointer) {
		if ((index - offset) < 0)
			continue;

		if (shown >= ACMEN_MAX_DISPLAY_ROWS)
			continue;

		tmpListitemPointer = Pointer:MEM_get_val(pointer, index);

		if (tmpListitemPointer == MEM_NULLPTR)
			continue;

		MEM_get_arr(tmpListitemPointer, _, str);

		new textdrawIndex = (index - offset);

		PlayerTextDrawSetString(playerid, TD_AM_Row[playerid][textdrawIndex], str);

		shown++;
	}
	return ACMEN_ERR_NONE;
}

stock _ActionMenu_GetListItemValue(playerid, textdrawIndex, &listitemIndex, value[], valueSize = sizeof value) {
	if (!IsPlayerConnected(playerid))
		return ACMEN_ERR_INVALID_PLAYER;

	new Pointer:pointer = AM_PlayerListitem[playerid];

	if (pointer == MEM_NULLPTR)
		return ACMEN_ERR_INVALID_DATA;

	new offset = AM_PlayerOffset[playerid];

	listitemIndex = (textdrawIndex + offset);

	new Pointer:tmpListitemPointer = Pointer:MEM_get_val(pointer, listitemIndex);

	if (tmpListitemPointer == MEM_NULLPTR)
		return ACMEN_ERR_INVALID_DATA;

	MEM_get_arr(tmpListitemPointer, _, value, valueSize);
	return ACMEN_ERR_NONE;
}

stock PlayerText:_ActionMenu_CreateTitleBGTD(playerid) {
	new PlayerText:tmpText = CreatePlayerTextDraw(playerid, 473.000000, 120.000000, ACMEN_TXD_BACKGROUND);
	PlayerTextDrawFont(playerid, tmpText, 4);
	PlayerTextDrawLetterSize(playerid, tmpText, 0.600000, 2.000000);
	PlayerTextDrawTextSize(playerid, tmpText, 135.000000, 30.000000);
	PlayerTextDrawSetOutline(playerid, tmpText, 1);
	PlayerTextDrawSetShadow(playerid, tmpText, 0);
	PlayerTextDrawAlignment(playerid, tmpText, 1);
	PlayerTextDrawColor(playerid, tmpText, ACMEN_COLOR_BG);
	PlayerTextDrawBackgroundColor(playerid, tmpText, 255);
	PlayerTextDrawBoxColor(playerid, tmpText, 50);
	PlayerTextDrawUseBox(playerid, tmpText, 1);
	PlayerTextDrawSetProportional(playerid, tmpText, 1);
	PlayerTextDrawSetSelectable(playerid, tmpText, 0);
	return tmpText;
}

stock PlayerText:_ActionMenu_CreateTitleTD(playerid, const title[]) {
	new PlayerText:tmpText = CreatePlayerTextDraw(playerid, 466.000000, 111.000000, title);
	PlayerTextDrawFont(playerid, tmpText, 0);
	PlayerTextDrawLetterSize(playerid, tmpText, 0.479166, 1.750000);
	PlayerTextDrawTextSize(playerid, tmpText, 615.000000, 126.000000);
	PlayerTextDrawSetOutline(playerid, tmpText, 1);
	PlayerTextDrawSetShadow(playerid, tmpText, 0);
	PlayerTextDrawAlignment(playerid, tmpText, 1);
	PlayerTextDrawColor(playerid, tmpText, ACMEN_COLOR_TITLE);
	PlayerTextDrawBackgroundColor(playerid, tmpText, 255);
	PlayerTextDrawBoxColor(playerid, tmpText, 50);
	PlayerTextDrawUseBox(playerid, tmpText, 0);
	PlayerTextDrawSetProportional(playerid, tmpText, 1);
	PlayerTextDrawSetSelectable(playerid, tmpText, 0);
	return tmpText;
}

stock PlayerText:_ActionMenu_CreateSubtitleTD(playerid, const subtitle[]) {
	new PlayerText:tmpText = CreatePlayerTextDraw(playerid, 477.000000, 129.000000, subtitle);
	PlayerTextDrawFont(playerid, tmpText, 1);
	PlayerTextDrawLetterSize(playerid, tmpText, 0.266665, 1.349997);
	PlayerTextDrawTextSize(playerid, tmpText, 604.000000, 134.500000);
	PlayerTextDrawSetOutline(playerid, tmpText, 0);
	PlayerTextDrawSetShadow(playerid, tmpText, 0);
	PlayerTextDrawAlignment(playerid, tmpText, 1);
	PlayerTextDrawColor(playerid, tmpText, ACMEN_COLOR_SUBTITLE);
	PlayerTextDrawBackgroundColor(playerid, tmpText, 255);
	PlayerTextDrawBoxColor(playerid, tmpText, 50);
	PlayerTextDrawUseBox(playerid, tmpText, 0);
	PlayerTextDrawSetProportional(playerid, tmpText, 1);
	PlayerTextDrawSetSelectable(playerid, tmpText, 0);
	return tmpText;
}

stock PlayerText:_ActionMenu_CreateRowBGTD(playerid, offset) {
	new
		Float:offsetValue = floatmul(float(offset), ACMEN_OFFSET_VALUE),
		Float:offsetPos = floatadd(ACMEN_START_POS, offsetValue);

	new PlayerText:tmpText = CreatePlayerTextDraw(playerid, 473.000000, offsetPos, ACMEN_TXD_BACKGROUND);
	PlayerTextDrawFont(playerid, tmpText, 4);
	PlayerTextDrawLetterSize(playerid, tmpText, 0.600000, 2.000000);
	PlayerTextDrawTextSize(playerid, tmpText, 135.000000, 20.000000);
	PlayerTextDrawSetOutline(playerid, tmpText, 0);
	PlayerTextDrawSetShadow(playerid, tmpText, 0);
	PlayerTextDrawAlignment(playerid, tmpText, 1);
	PlayerTextDrawColor(playerid, tmpText, COLOR_LIST_ITEM_BG);
	PlayerTextDrawBackgroundColor(playerid, tmpText, 255);
	PlayerTextDrawBoxColor(playerid, tmpText, 50);
	PlayerTextDrawUseBox(playerid, tmpText, 1);
	PlayerTextDrawSetProportional(playerid, tmpText, 1);
	PlayerTextDrawSetSelectable(playerid, tmpText, true);
	return tmpText;
}

stock PlayerText:_ActionMenu_CreateRowTD(playerid, offset, const row[]) {
	new
		Float:offsetValue = floatmul(float(offset), ACMEN_OFFSET_VALUE),
		Float:offsetPos = floatadd(ACMEN_START_POS, offsetValue),
		Float:offsetFinal = floatadd(offsetPos, 4.000);

	new PlayerText:tmpText = CreatePlayerTextDraw(playerid, 477.000000, offsetFinal, row);
	PlayerTextDrawFont(playerid, tmpText, 1);
	PlayerTextDrawLetterSize(playerid, tmpText, 0.233333, 1.200000);
	PlayerTextDrawTextSize(playerid, tmpText, 604.500000, 14.000000);
	PlayerTextDrawSetOutline(playerid, tmpText, 0);
	PlayerTextDrawSetShadow(playerid, tmpText, 1);
	PlayerTextDrawAlignment(playerid, tmpText, 1);
	PlayerTextDrawColor(playerid, tmpText, ACMEN_COLOR_LISTITEM);
	PlayerTextDrawBackgroundColor(playerid, tmpText, 255);
	PlayerTextDrawBoxColor(playerid, tmpText, -2686926);
	PlayerTextDrawUseBox(playerid, tmpText, 0);
	PlayerTextDrawSetProportional(playerid, tmpText, 1);
	PlayerTextDrawSetSelectable(playerid, tmpText, 0);
	return tmpText;
}

stock Text:_ActionMenu_CreateDownBG() {
	new Float:yOffset = floatmul(ACMEN_MAX_DISPLAY_ROWS, ACMEN_OFFSET_VALUE);
	new Float:yPos = floatadd(ACMEN_START_POS, yOffset);

	new Text:tmpText = TextDrawCreate(541.000000, yPos, ACMEN_TXD_BACKGROUND);
	TextDrawFont(tmpText, 4);
	TextDrawLetterSize(tmpText, 0.600000, 2.000000);
	TextDrawTextSize(tmpText, 67.000000, 20.000000);
	TextDrawSetOutline(tmpText, 1);
	TextDrawSetShadow(tmpText, 0);
	TextDrawAlignment(tmpText, 1);
	TextDrawColor(tmpText, 255);
	TextDrawBackgroundColor(tmpText, 255);
	TextDrawBoxColor(tmpText, 50);
	TextDrawUseBox(tmpText, 1);
	TextDrawSetProportional(tmpText, 1);
	TextDrawSetSelectable(tmpText, 1);
	return tmpText;
}

stock Text:_ActionMenu_CreateDownArrow() {
	new Float:yOffset = floatmul(ACMEN_MAX_DISPLAY_ROWS, ACMEN_OFFSET_VALUE);
	new Float:bgYPos = floatadd(ACMEN_START_POS, yOffset);
	new Float:yPos = floatadd(ACMEN_TXD_OFFSET_Y, bgYPos);

	new Text:tmpText = TextDrawCreate(ACMEN_TXD_OFFSET_X_DOWN, yPos, ACMEN_TXD_OFFSET_DOWN);
	TextDrawFont(tmpText, 4);
	TextDrawLetterSize(tmpText, 0.558332, 1.500000);
	TextDrawTextSize(tmpText, ACMEN_TXD_OFFSET_SIZE_X, ACMEN_TXD_OFFSET_SIZE_Y);
	TextDrawSetOutline(tmpText, 1);
	TextDrawSetShadow(tmpText, 0);
	TextDrawAlignment(tmpText, 2);
	TextDrawColor(tmpText, -1);
	TextDrawBackgroundColor(tmpText, -1);
	TextDrawBoxColor(tmpText, -206);
	TextDrawUseBox(tmpText, 0);
	TextDrawSetProportional(tmpText, 1);
	TextDrawSetSelectable(tmpText, 0);
	return tmpText;
}

stock Text:_ActionMenu_CreateUpBG() {
	new Float:yOffset = floatmul(ACMEN_MAX_DISPLAY_ROWS, ACMEN_OFFSET_VALUE);
	new Float:yPos = floatadd(ACMEN_START_POS, yOffset);

	new Text:tmpText = TextDrawCreate(473.000000, yPos, ACMEN_TXD_BACKGROUND);
	TextDrawFont(tmpText, 4);
	TextDrawLetterSize(tmpText, 0.600000, 2.000000);
	TextDrawTextSize(tmpText, 67.000000, 20.000000);
	TextDrawSetOutline(tmpText, 1);
	TextDrawSetShadow(tmpText, 0);
	TextDrawAlignment(tmpText, 1);
	TextDrawColor(tmpText, 255);
	TextDrawBackgroundColor(tmpText, 255);
	TextDrawBoxColor(tmpText, 50);
	TextDrawUseBox(tmpText, 1);
	TextDrawSetProportional(tmpText, 1);
	TextDrawSetSelectable(tmpText, 1);
	return tmpText;
}

stock Text:_ActionMenu_CreateUpArrow() {
	new Float:yOffset = floatmul(ACMEN_MAX_DISPLAY_ROWS, ACMEN_OFFSET_VALUE);
	new Float:bgYPos = floatadd(ACMEN_START_POS, yOffset);
	new Float:yPos = floatadd(ACMEN_TXD_OFFSET_Y, bgYPos);

	new Text:tmpText = TextDrawCreate(ACMEN_TXD_OFFSET_X_UP, yPos, ACMEN_TXD_OFFSET_UP);
	TextDrawFont(tmpText, 4);
	TextDrawLetterSize(tmpText, 0.558332, 1.500000);
	TextDrawTextSize(tmpText, ACMEN_TXD_OFFSET_SIZE_X, ACMEN_TXD_OFFSET_SIZE_Y);
	TextDrawSetOutline(tmpText, 1);
	TextDrawSetShadow(tmpText, 0);
	TextDrawAlignment(tmpText, 2);
	TextDrawColor(tmpText, -1);
	TextDrawBackgroundColor(tmpText, -1);
	TextDrawBoxColor(tmpText, -206);
	TextDrawUseBox(tmpText, 0);
	TextDrawSetProportional(tmpText, 1);
	TextDrawSetSelectable(tmpText, 0);
	return tmpText;
}
