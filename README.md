# MisaMino-Tetrio
MisaMino with more features. Tetrio attack table is on by default, others need to be set in the setting.ini.
1. Tetrio attack table. (Check [here](https://chouhy.github.io/Tetrio-Attack-Table/) for detail)
2. Garbage cap (in tetrio, you will at most accept 8 lines of garbage at a time, which is garbage cap = 8)
3. Delayed attack (time in ms) (in tetrio or jstris, the garbage you sent will not show on opponent's garbage buffer right away) (you can still cancel these delayed garbage with your attack)
4. Undo (backspace)
5. Generate .ttrm replay file which can be played on Tetrio! (F11) (In settings.ini, be sure that [AI] move=2 or larger)

<b>Notice:</b> That enable delayed attack, turn-based, and undo all at the same time does not make sense, so something bizarre might happen.<br>
<b>Notice2:</b> Once you generate replay file, you cannot undo an already finished game.

Future todos:<br>
passthrough(?

Download link: 
You can find the link in the release page
https://github.com/chouhy/MisaMino-Tetrio/releases

Library:
Json for C++
https://github.com/nlohmann/json
