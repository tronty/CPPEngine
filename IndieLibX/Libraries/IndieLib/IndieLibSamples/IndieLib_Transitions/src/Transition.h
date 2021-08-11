#ifndef __TRANSITIONS_H__
#define __TRANSITIONS_H__

#include <CIndieLib.h>

enum TransitionType
{
	RANDOM,
	ALPHA2,
	WAVE_ALPHA,
	WIPE_LEFT,
	WIPE_RIGHT,
	WIPE_UP,
	WIPE_DOWN,
	SCROLL_LEFT,
	SCROLL_RIGHT,
	SCROLL_UP,
	SCROLL_DOWN,
	SQUEEZE_LEFT,
	SQUEEZE_RIGHT,
	SQUEEZE_UP,
	SQUEEZE_DOWN,
	SHRINK,
	SHRINK_SPIN_CLOCKWISE,
	SHRINK_SPIN_COUNTERCLOCKWISE,
	SLIDE_LEFT,
	SLIDE_RIGHT,
	SLIDE_UP,
	SLIDE_DOWN,
	SLIDE_OUT_LEFT,
	SLIDE_OUT_RIGHT,
	SLIDE_OUT_UP,
	SLIDE_OUT_DOWN,
	EXPAND,
	EXPAND_SPIN_CLOCKWISE,
	EXPAND_SPIN_COUNTERCLOCKWISE,
	FADEOUT
};

class Transition
{
public:
			 Transition(IND_Entity2d* pSource, IND_Entity2d* pDest, TransitionType type, int width, int height, float duration = 1);
	virtual ~Transition( );

	void	Tick		( float fElapsedTime );
	bool	IsComplete	( );
	float	GetPercentComplete ( );

private:
	IND_Entity2d*	m_SourceEntity2d;
	IND_Entity2d*	m_DestEntity2d;



	int				m_Width, m_Height;
	TransitionType	m_Type;
	IND_Timer		m_Timer;
	int				m_Source_alpha;
	int				m_Destination_alpha;
	float 			m_Source_x;
	float 			m_Source_y;
	float 			m_Source_x_scale;
	float 			m_Source_y_scale;
	float 			m_Source_rot;
	float 			m_Destination_x;
	float 			m_Destination_y;
	float 			m_Destination_x_scale;
	float 			m_Destination_y_scale;
	float 			m_Destination_rot;

	float			m_fTime;
	float			m_fDuration;
	int				m_nRows;
	int				m_nCols;
	float			m_fPercent;

	IND_Surface		m_ClonedSourceSurface;
	IND_Surface		m_ClonedDestSurface;
	IND_Surface*	m_OriginalSourceSurface;
	IND_Surface*	m_OriginalDestSurface;
	int				m_NumBlocksX;
	int				m_NumBlocksY;
	int				m_WidthBlock;
	int				m_HeightBlock;

	CIndieLib *		m_pIndieLib;
};
#endif //__TRANSITIONS_H__