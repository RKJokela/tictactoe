#include <panel.h>
#include <time.h>
#include <random>
#include <string>

#define ORG_X	8
#define ORG_Y	5

#define TAB			'\t'
#define SPACEBAR	32
#define ENTER		13
#define QUIT		'q'
#define ESC			27
#define HELP		'h'
#define RESTART		'm'
#define ARROW_LT	KEY_LEFT
#define ARROW_RT	KEY_RIGHT
#define ARROW_UP	KEY_UP
#define ARROW_DN	KEY_DOWN

#define COMPUTER_DELAY	350
#define MISTAKE_RATE	70

enum Mark {
	NONE = 0,
	X = 1,
	O = 2
};
Mark playerMark = X;
int gameBoard[9];

enum GameState {
	MENU,
	INFO,
	PLAYING,
	ENDED,
	QUITTING
};
GameState currentState = MENU;
GameState returnState  = MENU;

enum Difficulty {
	STUPID_EASY = 0,
	SORTA_MEDIUM = 1,
	IMPOSSIBLE_HARD = 2
};
Difficulty currentDiff = IMPOSSIBLE_HARD;

// is game finished?
bool done;

// how many turns
int turnNumber;

// who won?
Mark winner;
int winningLine[3] = { 0, 0, 0 };

// displays help info
WINDOW* info;
PANEL* pan;

void draw_board(int x, int y, bool border = false);
bool main_loop();
GameState main_menu();
void init_info();
GameState info_screen();
GameState run_game();
bool player_turn(Mark ch);
bool computer_turn(Mark ch);
int  decide_move(Mark ch);
int  decide_move_X();
int  decide_move_O();
bool test_game_state(Mark ch, int pos);

int main()
{
	// initialization
	initscr();
	start_color();
	init_pair(1, COLOR_CYAN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_GREEN, COLOR_BLACK);
	noecho();
	raw();
	keypad(stdscr, TRUE);
	curs_set(0);
	// seed RNG
	srand(time(NULL));
	// create info screen
	info = newwin(0, 0, 0, 0);
	init_info();
	pan = new_panel(info);
	hide_panel(pan);
	// start at menu
	currentState = MENU;

	// main loop
	bool quit = false;
	while (!quit)
	{
		done = false;
		winner = NONE;
		turnNumber = 0;
		for (int i = 0; i < 9; i++)
			gameBoard[i] = 0;
		clear();
		//currentState = MENU;
		if (currentState == PLAYING)
			draw_board(ORG_X, ORG_Y);
		while (!done)
		{
			done = main_loop();
		}

		if (currentState == ENDED)
		{
			// output result
			if (winner)
			{
				attron(A_BOLD | COLOR_PAIR(2));
				for (int i = 0; i < 3; i++)
					mvaddch(ORG_Y + 2*(winningLine[i]/3), ORG_X + 2*(winningLine[i]%3), (winner == X ? 'X' : 'O'));
				attroff(A_BOLD | COLOR_PAIR(2));
				mvprintw(ORG_Y + 6, ORG_X, "YOU %s", (winner == playerMark ? "WIN" : "LOSE"));
			}
			else
				mvaddstr(ORG_Y + 6, ORG_X, "CAT'S GAME");
			mvaddstr(ORG_Y + 8, ORG_X, "Play again (");
			attron(A_BOLD | COLOR_PAIR(1)); addch('y'); attroff(A_BOLD | COLOR_PAIR(1));
			addstr("es/");
			attron(A_BOLD | COLOR_PAIR(1)); addch('n'); attroff(A_BOLD | COLOR_PAIR(1));
			addstr("o/");
			attron(A_BOLD | COLOR_PAIR(1)); addch('m'); attroff(A_BOLD | COLOR_PAIR(1));
			addstr("enu)?");
			int again = getch();
			quit = (again == 'n' || again == 'q');
			if (again == 'y' || again == ENTER)
				currentState = PLAYING;
			else
				currentState = MENU;
		}
		else if (currentState == QUITTING)
			quit = true;
	}

	// cleanup
	del_panel(pan);
	delwin(info);
	endwin();
	return 0;
}

void draw_board(int x, int y, bool border)
{
	mvaddstr(y++, x, " | | ");
	move(y++, x); hline(ACS_HLINE, 5);
	mvaddstr(y++, x, " | | ");
	move(y++, x); hline(ACS_HLINE, 5);
	mvaddstr(y++, x, " | | ");
	mvaddstr(LINES - 1, 4, "Press 'h' for help");
}

// true: Game is done
bool main_loop()
{
	switch (currentState)
	{
	case INFO:
		currentState = info_screen();
		break;
	case MENU:
		currentState = main_menu();
		break;
	case PLAYING:
		currentState = run_game();
		if (currentState != PLAYING)
			return true;
		break;
	case QUITTING:
		return true;
	}
	return false;
}

GameState main_menu()
{
	// strings and variables
	static std::string	diffs[3];
	diffs[0] = "    STUPID EASY    \n";
	diffs[1] = "   KINDA* MEDIUM   \n";
	diffs[2] = "  IMPOSSIBLE HARD  \n";
	static std::string	xo[2];
	xo[0] =    "  X (goes first)   \n";
	xo[1] =    "  O (goes second)  \n";
	static std::string	descs[4];
	descs[0] = "The computer is a moron.";
	descs[1] = "The computer can make mistakes.\n";
	descs[3] = "	(*when it feels like it)";
	descs[2] = "The only winning move is not to play.";

	static int selection = 1;
	static int numOptions = 3;
	static bool menuScreen2 = false;

	// clear screen
	clear();

	mvaddstr(0, 4, "Tic Tac Toe v1.0, by Randy Jokela");
	if (!menuScreen2)
		mvaddstr(4, 8, "SELECT DIFFICULTY:");
	else
		mvaddstr(4, 8, "SELECT PLAYER:");

	for (int i = 0; i < numOptions; i++)
	{
		std::string *str = menuScreen2 ? xo : diffs;
		// set highlight
		if (i == selection)
			standout();
		mvaddstr(8 + i, 8, str[i].c_str());
		standend();
	}

	if (!menuScreen2)
	{
		mvaddstr(18, 8, descs[selection].c_str());
		if (selection == 1)
			addstr(descs[3].c_str());
	}

	refresh();

	// get input, updating screen until selection is made
	int input = getch();
	switch (input)
	{
	case ENTER:
		if (!menuScreen2)
		{
			currentDiff = (Difficulty)selection;
			menuScreen2 = true;
			numOptions = 2;
			selection = ((int)playerMark + 1)%2;
			return MENU;
		}
		playerMark = selection ? O : X;
		clear();
		draw_board(ORG_X, ORG_Y);
		menuScreen2 = false;
		numOptions = 3;
		selection = currentDiff;
		return PLAYING;
	case HELP:
		returnState = MENU;
		return INFO;
	case ESC:
	case QUIT:
		return QUITTING;
	// clever fall through
	case ARROW_UP:
		selection = (selection + numOptions - 2)%numOptions;
	case ARROW_DN:
		selection = (selection + 1)%numOptions;
	default:
		return MENU;
	}

	return QUITTING;
}

void init_info()
{
	mvwaddstr(info, 0, 4, "Tic Tac Toe v1.0, by Randy Jokela\n\n\n\n\n\n"

"        CONTROLS:\n"
"        - arrow keys, tab, or spacebar to move cursor\n"
"        - Enter to place X/O at cursor\n"
"        - 'm' to go back to the menu and restart game\n"
"        - 'q' or Escape anytime to quit\n"
"        - 'h' to reach this screen\n\n"

"    --Press any key to continue--\n" );
}

GameState info_screen()
{
	show_panel(pan);
	wrefresh(info);
	getch();
	hide_panel(pan);
	return returnState;
}

GameState run_game()
{
	// ASSIGN PLAYERS - X GOES FIRST
	bool (*x_turn)(Mark) = (playerMark == X) ? player_turn : computer_turn;
	bool (*o_turn)(Mark) = (playerMark == O) ? player_turn : computer_turn;

	// take advantage of short-circuiting
	if ( (x_turn(X) || turnNumber == 9 || o_turn(O)) && currentState == PLAYING )
		return ENDED;

	// player may quit mid-game - allow turn functions to change state
	return currentState;
}


bool player_turn(Mark ch)
{
	int playerMove = -1;
	turnNumber++;
	mvprintw(0, 0, "%s's turn %d", (ch == X ? "X" : "O"), (turnNumber + 1)/2);
	refresh();
	/*
		input loop
		start cursor on first empty box
		can press space to move to next empty, or enter to confirm
	*/
	bool moveDone = false;
	move(LINES-1, COLS-1);
	chtype playerChar = (ch == X ? 'X' : 'O') | A_BOLD | COLOR_PAIR(3);
	curs_set(2);
	do
	{
		do
		{
			playerMove++;
			if (playerMove > 8)
				playerMove -= 9;
		} while (gameBoard[playerMove] != 0);
		addch(' ');
		move(ORG_Y + 2*(playerMove/3), ORG_X + 2*(playerMove%3));
		addch(playerChar);
		echochar(8);
		int input = getch();
		switch (input)
		{
		case ARROW_LT:
			playerMove = (playerMove + 7)%9;
			while (gameBoard[playerMove+1])
			{
				playerMove--;
				if (playerMove < -1)
					playerMove += 9;
			}
			break;
		case ARROW_UP:
			playerMove = (playerMove + 5)%9;
			break;
		case ARROW_DN:
			playerMove = (playerMove + 2)%9;
			break;
		case ARROW_RT:
		case TAB:
		case SPACEBAR:
			// loop
			break;
		case ENTER:
			// make the move, update board, exit loop
			addch( (ch == X ? 'X' : 'O') );
			gameBoard[playerMove] = ch;
			moveDone = true;
			refresh();
			break;
		case HELP:
			playerMove--;
			info_screen();
			break;
		case RESTART:
			currentState = MENU;
			curs_set(0);
			return true;
		case ESC:
		case QUIT:
			currentState = QUITTING;
			curs_set(0);
			return true;
		default:
			break;
		}
	} while (!moveDone);

	// Did the game end? (if yes, current player must be winner)
	if (test_game_state(ch, playerMove))
	{
		winner = ch;
		curs_set(0);
		return true;
	}
	curs_set(0);
	return false;
}

// same as player turn, except instead of input loop, we have AI logic loop
bool computer_turn(Mark ch)
{
	int computerMove = -1;
	turnNumber++;
	mvprintw(0, 0, "%s's turn %d", (ch == X ? "X" : "O"), (turnNumber + 1)/2);
	refresh();
	napms(COMPUTER_DELAY);

	// pick a move
	computerMove = decide_move(ch);
	// draw the move
	mvaddch( ORG_Y + 2*(computerMove/3), ORG_X + 2*(computerMove%3), (ch == X ? 'X' : 'O') );
	refresh();
	// update the board
	gameBoard[computerMove] = ch;

	// Did the game end? (if yes, current player must be winner)
	if (test_game_state(ch, computerMove))
	{
		winner = ch;
		return true;
	}
	return false;
}

#define is_corner(n)	((n)%2 == 0 && (n) != 4)
#define is_edge(n)		((n)%2 == 1)
#define is_center(n)	((n) == 4)
int decide_move(Mark ch)
{
	/* PROCESS:
		1. examine board for 2-in-a-row (either player); move in 3rd spot if so
		2. otherwise, depends whether X or O?
	*/
	int validMoves[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	for (int i = 9, j = 0; i--;)
	{
		if (gameBoard[i] == 0)
			validMoves[j++] = i;
	}

	// if it's the last move, no other choice, just return
	if (turnNumber == 9)
		return validMoves[0];

	// checking for a winning move; if so, play it
	for (int i = 0; i + turnNumber <= 9; i++)
	{
		if (test_game_state(ch, validMoves[i]))
			return validMoves[i];
	}

	// Easy mode: most of the time, just pick a random spot and go there
	if (currentDiff == STUPID_EASY && rand()%100 < MISTAKE_RATE)
	{
		return validMoves[rand()%(10 - turnNumber)];
	}

	// checking for blocking opponent's winning move; if so, play it
	for (int i = 0; i + turnNumber <= 9; i++)
	{
		if (test_game_state((ch == X ? O : X), validMoves[i]))
			return validMoves[i];
	}

	// now we will do logic based on who we are and turn number
	return ( ch == X ? decide_move_X() : decide_move_O() );
}

int decide_move_X()
{
	if (currentDiff == STUPID_EASY)
	{
		int move = rand()%9;
		while (gameBoard[move])
			move = rand()%9;
		return move;
	}
	static int myMoves[4];
	static int oppMoves[2];
	int curMove = 0;
	switch (turnNumber)
	{
	// always play a corner first
	case 1:
		myMoves[0] = 0; myMoves[1] = 0; myMoves[2] = 0; myMoves[3] = 0;
		oppMoves[0] = 0; oppMoves[1] = 0;
		curMove = 2*(rand()%4);
		if (curMove == 4) curMove = 8;
		myMoves[0] = curMove;
		return curMove;
	case 3:
		// where did O move?
		for (oppMoves[0] = -1; gameBoard[++oppMoves[0]] != O;);
		if (currentDiff == SORTA_MEDIUM && rand()%100 < MISTAKE_RATE)
		{	// make a mistake
			int move = rand()%9;
			while (gameBoard[move])
				move = rand()%9;
			myMoves[1] = move;
			return move;
		}
		// edge: take center
		else if (is_edge(oppMoves[0]))
		{
			myMoves[1] = 4;
			return 4;
		}
		// corner: take adj. corner
		else if (is_corner(oppMoves[0]))
		{
			switch (myMoves[0])
			{
			case 0:
				if (!gameBoard[2])
					curMove = 2;
				else
					curMove = 6;
				break;
			case 2:
				if (!gameBoard[8])
					curMove = 8;
				else
					curMove = 0;
				break;
			case 6:
				if (!gameBoard[0])
					curMove = 0;
				else
					curMove = 8;
				break;
			case 8:
				if (!gameBoard[6])
					curMove = 6;
				else
					curMove = 2;
				break;
			}
			myMoves[1] = curMove;
			return curMove;
		}
		// center: take opp. corner
		else if (is_center(oppMoves[0]))
		{
			switch (myMoves[0])
			{
			case 0:
				curMove = 8; break;
			case 2:
				curMove = 6; break;
			case 6:
				curMove = 2; break;
			case 8:
				curMove = 0; break;
			}
			myMoves[1] = curMove;
			return curMove;
		}
	case 5:
		// find opponent's second move
		for (oppMoves[1] = 9; --oppMoves[1];)
		{
			if (gameBoard[oppMoves[1]] == O && oppMoves[1] != oppMoves[0])
				break;
		}
		if (currentDiff == IMPOSSIBLE_HARD || rand()%100 >= MISTAKE_RATE)
		{
			if (is_corner(oppMoves[0]) && is_edge(oppMoves[1]))
				// take last corner ftw
				for (int i = 8; i >= 0; i -= 2)
				{
					if (!gameBoard[i] && i != 4)
						return i;
				}
			if (is_corner(oppMoves[1]) && is_edge(oppMoves[0]))
			{
				// take connecting edge
				switch (myMoves[0])
				{
				case 0:
				case 6:
					if (!gameBoard[myMoves[0]+1])
						return myMoves[0] + 1;
					return 3;
				case 2:
				case 8:
					if (!gameBoard[myMoves[0]-1])
						return myMoves[0] - 1;
					return 5;
				}
			}
		}
		if (!gameBoard[4])
			return 4;
	case 7:
	default:
		for (int i = 0; i < 9; i++)
			if (!gameBoard[i])
				return i;
	}
}

int decide_move_O()
{
	if (currentDiff == STUPID_EASY)
	{
		int move = rand()%9;
		while (gameBoard[move])
			move = rand()%9;
		return move;
	}
	static int myMoves[4];
	static int enemyMoves[4];
	int curMove = 0;
	switch (turnNumber)
	{
	case 2:
		myMoves[0] = 0; myMoves[1] = 0; myMoves[2] = 0; myMoves[3] = 0;
		enemyMoves[0] = 0; enemyMoves[1] = 0; enemyMoves[2] = 0; enemyMoves[3] = 0;
		// find where X went
		for (enemyMoves[0] = 8; !gameBoard[enemyMoves[0]]; --enemyMoves[0]);
		if (currentDiff == SORTA_MEDIUM && rand()%100 < MISTAKE_RATE)
		{	// make mistake
			curMove = rand()%9;
			while (gameBoard[curMove])
				curMove = rand()%9;
		}
		// Take center if open, else take corner
		else if (!gameBoard[4])
			curMove = 4;
		else
		{
			curMove = 2*(rand()%4);
			if (curMove == 4)
				curMove += 4;
		}
		myMoves[0] = curMove;
		return curMove;
	case 4:
		// find where X went
		for (enemyMoves[1] = 8; (gameBoard[enemyMoves[1]] != X || enemyMoves[1] == enemyMoves[0]); --enemyMoves[1]);
		if (currentDiff == SORTA_MEDIUM && rand()%100 < MISTAKE_RATE)
		{	// make mistake
			curMove = rand()%9;
			while (gameBoard[curMove])
				curMove = rand()%9;
		}
		else if (is_corner(myMoves[0]))
			// take another corner
			// to reach here, X must have played in opposite corner from us
			// (otherwise, blocking would be forced)
			for (curMove = 8; (gameBoard[curMove] || !is_corner(curMove)); curMove--);
		else // We took center
		{	
			// X went corners = take any edge
			if ( is_corner(enemyMoves[0]) && is_corner(enemyMoves[1]) )
			{
				curMove = 1 + 2*rand()%4;
			}
			// X took 1 corner = go for opp corner
			else if ( is_corner(enemyMoves[0]) || is_corner(enemyMoves[1]) )
			{
				int xCorner = is_corner(enemyMoves[0]) ? enemyMoves[0] : enemyMoves[1];
				int sign = (xCorner > 4) ? -1 : 1;
				int offset = (xCorner%8) ? 4 : 8;
				curMove = xCorner + sign*offset;
			}
			// X took 2 sides
			else if (enemyMoves[0] + enemyMoves[1] == 8)
			{
				// X took opp edges = take any edge
				for (curMove = 7; gameBoard[curMove]; curMove -= 2);
			}
			else
			{
				// X took adj edges = take the adj corner
				curMove = enemyMoves[0] + enemyMoves[1] - 4;
			}
		}
		myMoves[1] = curMove;
		return curMove;
	case 6:
	default:
		for (int i = 0; i < 9; i++)
			if (!gameBoard[i])
				return i;
	}
}

// test whether the player in "ch" has won (or will win) the game using latest move "pos"
// return true if game is over (won by current player)
bool test_game_state(Mark ch, int pos)
{
	// number the positions 0-8
	// horizontal win: 012, 345, or 678
	// vertical win: 036, 147, or 258
	// diagonal win: 048 or 246
	bool myMoves[9];
	for (int i = 9; i--;)
	{
		myMoves[i] = (gameBoard[i] == ch);
	}
	myMoves[pos] = true;
	// test horizontal
	int horiz = pos - pos%3;
	if (myMoves[horiz] && myMoves[horiz+1] && myMoves[horiz+2])
	{
		winningLine[0] = horiz; winningLine[1] = horiz + 1; winningLine[2] = horiz + 2;
		return true;
	}
	// test vertical
	int vert = pos%3;
	if (myMoves[vert] && myMoves[vert+3] && myMoves[vert+6])
	{
		winningLine[0] = vert; winningLine[1] = vert + 3; winningLine[2] = vert + 6;
		return true;
	}
	// test diagonal
	if (myMoves[4])
	{
		if (myMoves[0] && myMoves[8])
		{
			winningLine[0] = 0; winningLine[1] = 4; winningLine[2] = 8;
		}
		else if (myMoves[2] && myMoves[6])
		{
			winningLine[0] = 2; winningLine[1] = 4; winningLine[2] = 6;
		}
		else
			return false;
		return true;
	}	
	return false;
}
