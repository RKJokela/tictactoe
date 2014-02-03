tictactoe
=========

Unwinnable console-based Tic Tac Toe. Also contains 2 easier difficulty settings.

This is my first completed game! I am currently practicing game development using C++, starting with easy projects like Tic Tac Toe, Pong, etc. and working my way up from there.

As such, my focus is not on creating beautiful, elegent, efficient code or adding a ton of awesome features, but rather on actually completing a game, to the point where I have version 1.0 of an executable I can confidently call a working game that someone else could pick up and play successfully.

For this one especially, I've done a buch of coding "no-nos" like using tons of global variables, no OOP, and putting everything into one source file. I figure since Tic Tac Toe is so small, and my first try, doing all these "properly" is a little excessive, and I plan to slowly learn & incorporate code management techniques in the future.

Basically I just coded an "AI" (not really - it's such a small problem space that I just hard-coded in all the optimal decisions) that will always play perfectly, so it will win if the player makes any mistake, and tie otherwise, regardless of whether it goes first or second. I mainly used the map made by Randall Munroe at www.xkcd.com/832 to choose the correct plays.

Since this results in more of a demonstration of how Tic Tac Toe is flawed rather than an actual "game", I also added difficulty levels, so the player could actually win if not playing against the perfect computer opponent.
- "Stupidly Easy": The computer basically plays randomly, but will occasionally block your three-in-a-row. It probably won't beat you unless you try to lose.
- "Kinda Medium": For this one I just followed the rules for the unbeatable computer, but added in a random chance for it to play the wrong move. It will always complete its three-in-a-row if it gets a chance, and block yours if it can. So you have to set up a "double threat" situation in order to beat it.
- "Impossible Hard": This is the one I coded initially. It uses a different decision list depending if it's playing X or O. In situations where it has multiple optimal moves, it can choose one at random. It will however, always start in a corner when playing as X. No matter what you do, you can't do better than a Cat's Game. Super boring but I did this to figure out how to code it, not really to play it other than to test that it works.

Here is the minimal list of "features" I deemed necessary so I could call this a complete game:

VERSION 1.0
-----------
- Game menu at start allowing player to choose difficulty and whether X or O
- Info screen explaining what the keys do, accessible anytime
- Ability to go back to menu/restart at any point
- Cursor showing your potential move using your selected X/O marker
- Ending message stating whether you won, lost, or tied
- Highlighting winning 3-in-a-row in red upon win/loss
- Ability to replay the game using same settings, or go back to the menu
- Ability to quit at any time using Q or Escape keys

...that's it! My first "game"!

Used Curses and C Standard Libraries. All the code I wrote is in main.cpp.
