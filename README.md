# SA:MP Action Menu

[![sampctl](https://img.shields.io/badge/sampctl-action--menu-2f2f2f.svg?style=for-the-badge)](https://github.com/Ascent-Gaming/samp-action-menu) ![version](https://img.shields.io/github/v/release/ascent-gaming/samp-action-menu?sort=semver&style=for-the-badge)

## Installation

Simply install to your project:

```bash
sampctl package install Ascent-Gaming/samp-action-menu
```

Include in your code and begin using the library:

```pawn
#include <action-menu>
```

## Usage

 ```PAWN
/*
  playerid - The player to show the menu for.
  title[] - A title shown above the menu.
  subtitle[] - A subtitle shown below the title.
  info[] - A string containing a list of menu-items. Use \n to signify a new item.
  bool:hideOnSelect - Does not impact functionality at this time.
*/
forward ActionMenu_Show(playerid, const title[], const subtitle[], const info[], bool:hideOnSelect = true);
forward ActionMenu_Hide(playerid);
 ```
 
 ```PAWN
/*
  playerid - The player who has given a response to a menu.
  bool:response - Weather or not the player has selected a listitem.
    True - The player made a selection
    False - The player did not make a selection.
   listitem - The `index` of the selected item. Can be higher than the number of items currently being displayed.
   inputtext - The `info` of the selected item.
*/
forward ActionMenu_OnPlayerResponse(playerid, response, listitem, const inputtext[]);
```

### Configuration

Nearly the entire menu can be configured using `#define's` to override what would be the module's default values. The following are most notable:

* `ACMEN_MAX_DISPLAY_ROWS`: *Default Value: 10* - Control the maximum number of `listItems` displayed at any one moment in time. Controls for *scrolling* the menu are automatically repositioned.

* `ACMEN_TXD_*`: *Default Value: See the [source code](https://github.com/Ascent-Gaming/samp-action-menu/blob/master/action-menu.inc#L42)* - Override the `TXDs` used for an specific part of the interface. Fully supports `0.3.DL` custom textures.

* `ACMEN_COLOR_PRIMARY`: *Default Value: `0xFF8000FF`* - Specify a color that is used for the `title` text and as the SA:MP *hover* color. See the [source code](https://github.com/Ascent-Gaming/samp-action-menu/blob/master/action-menu.inc#L95) for more `_COLOR_` options.

## Testing

<!--
Depending on whether your package is tested via in-game "demo tests" or
y_testing unit-tests, you should indicate to readers what to expect below here.
-->

To test, simply run the package:

```bash
sampctl package run
```
