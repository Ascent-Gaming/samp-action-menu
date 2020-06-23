#include <a_samp>
#include "action-menu.inc"

main() {
	//
}

public OnPlayerSpawn(playerid) {
	ActionMenu_Show(playerid, "Title", "Subtitle", "Option One\nOption Two\nOption Three\nOption Four\nOption Five\nOption Six");
	return 1;
}

public ActionMenu_OnPlayerResponse(playerid, response, listitem, const inputtext[]) {
	if (!response)
		return 1;

	new str[128];
	format(str, sizeof str, "[%d] %s", listitem, inputtext);
	SendClientMessage(playerid, -1, str); // -1 = White
	return 1;
}