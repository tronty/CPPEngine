/*
    Bastet - tetris clone with embedded bastard block chooser
    (c) 2005-2009 Federico Poloni <f.polonithirtyseven@sns.it> minus 37

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UI_HPP
#define UI_HPP

#include "Well.hpp"
#include "BlockPosition.hpp"
#include "BlockChooser.hpp"
#include "Config.hpp"

#include <string>
#include <Framework3/IRenderer.h>

struct WINDOW
{
	WINDOW()
	{
		height=0;
		width=0;
		x=0;
		y=0;
		borderwin=0;
	}
	WINDOW(unsigned int aheight, unsigned int awidth, unsigned int ay, unsigned int ax)
	{
		height=aheight;
		width=awidth;
		x=ax;
		y=ay;
		borderwin=0;
	}
	WINDOW(WINDOW* aborderwin, unsigned int aheight, unsigned int awidth, unsigned int ay, unsigned int ax)
	{
		borderwin=aborderwin;
		height=aheight;
		width=awidth;
		x=ax;
		y=ay;
	}
	virtual ~WINDOW(){}
	WINDOW* borderwin;
	unsigned int height;
	unsigned int width;
	unsigned int x;
	unsigned int y;
};

inline void getbegyx(WINDOW* aborder,unsigned int ay,unsigned int ax)
{
		ax=aborder->x;
		ay=aborder->y;
}
inline void getmaxyx(WINDOW* aborder,unsigned int ay,unsigned int ax)
{
	// ???
}

#define ERR 0
inline box(WINDOW* aborder,unsigned int ay,unsigned int ax)
{
	// ???
}
#define wattrset
#define mvwaddch
#define initscr
#define has_colors
#define initscr
#define endwin
#define curs_set
#define keypad
#define nodelay
#define COLOR_PAIR
#define wmove
#define whline
#define wrefresh
#define stdscr
#define getch
#define wclrtobot
#define mvwprintw
#define COLOR_PAIR
#define COLOR_BLACK
#define COLOR_RED
#define COLOR_YELLOW
#define COLOR_GREEN
#define COLOR_CYAN
#define COLOR_MAGENTA
#define COLOR_BLUE
#define COLOR_WHITE

namespace Bastet{
  typedef std::pair<int,int> Score; //(points, lines)
  Score &operator +=(Score &a, const Score &b);

  class BorderedWindow{
  private:
    WINDOW *_window;
    WINDOW *_border;
  public:
    BorderedWindow(int height, int width, int y=-1, int x=-1); ///w and h are "inner" dimensions, excluding the border. y and x are "outer", including the border. y=-1,x=-1 means "center"
    ~BorderedWindow();
    operator WINDOW *(); //returns the inner window
    void RedrawBorder();
    int GetMinX(); ///these are including border
    int GetMinY();
    int GetMaxX();
    int GetMaxY();
    void DrawDot(const Dot &d, Color c);
  };

  class Curses{
  public:
    Curses();
  };

  class Ui{
  public:
    Ui();
    void MessageDialog(const std::string &message); //shows msg, ask for "space"
    std::string InputDialog(const std::string &message); //asks for a string
    int KeyDialog(const std::string &message); //asks for a single key
    int MenuDialog(const std::vector<std::string> &choices); //asks to choose one, returns index
    void RedrawStatic(); //redraws the "static" parts of the screen
    void RedrawWell(const Well *well, BlockType falling, const BlockPosition &pos);
    void ClearNext(); //clear the next block display
    void RedrawNext(BlockType next); //redraws the next block display
    void RedrawScore();
    void CompletedLinesAnimation(const LinesCompleted &completed);
    void DropBlock(BlockType b, Well *w); //returns <score,lines>
    
    void ChooseLevel();
    void Play(BlockChooser *bc);
    void HandleHighScores(difficulty_t diff); ///if needed, asks name for highscores
    void ShowHighScores(difficulty_t diff);
    void CustomizeKeys();
  private:
    //    difficulty_t _difficulty; //unused for now
    int _level;
    int _points;
    int _lines;
    Curses _curses;
    BorderedWindow _wellWin;
    BorderedWindow _nextWin;
    BorderedWindow _scoreWin;
    /**
     * this is a kind of "well" structure to store the colors used to draw the blocks.
     */
    typedef boost::array<Color,WellWidth> ColorWellLine; 
    typedef boost::array<ColorWellLine,RealWellHeight> ColorWell;
    ColorWell _colors;
  };
}

#endif //UI_HPP
