#include "Transition.h"
#include <math.h>
#include <cstdlib> 
#include <ctime>


Transition::Transition(IND_Entity2d* pSource, IND_Entity2d* pDest, TransitionType type, int width, int height, float duration)
{
	m_pIndieLib				= CIndieLib::Instance();

	m_SourceEntity2d		= pSource;
	m_DestEntity2d			= pDest;
	m_OriginalSourceSurface	= m_SourceEntity2d->GetSurface();
	m_OriginalDestSurface	= m_DestEntity2d->GetSurface();
	m_fTime					= 0.0f;
	m_fDuration				= duration;

	m_Width					= width;
	m_Height				= height;
	m_Source_alpha			= 0;
	m_Destination_alpha		= 0;
	m_Source_x				= 0.0f;
	m_Source_y				= 0.0f;
	m_Source_x_scale		= 1.0f;
	m_Source_y_scale		= 1.0f;
	m_Source_rot			= 0.0f;
	m_Destination_x			= 0.0f;
	m_Destination_y			= 0.0f;
	m_Destination_x_scale	= 1.0f;
	m_Destination_y_scale	= 1.0f;
	m_Destination_rot		= 0.0f;

	m_nRows					= 16;
	m_nCols					= 16;
	m_fPercent				= 0.0f;

	// Initialize settings
	m_SourceEntity2d->SetTransparency(255);
	m_SourceEntity2d->SetPosition(0, 0, 0);
	m_SourceEntity2d->SetHotSpot(0,0);
	m_SourceEntity2d->SetRegion(0, 0, m_Width, m_Height);
	m_SourceEntity2d->SetScale(1, 1);
	m_SourceEntity2d->SetAngleXYZ(0, 0, 0);

	m_DestEntity2d->SetTransparency(255);
	m_DestEntity2d->SetPosition(0, 0, 0);
	m_DestEntity2d->SetHotSpot(0,0);
	m_DestEntity2d->SetRegion(0, 0, m_Width, m_Height);
	m_DestEntity2d->SetScale(1, 1);
	m_DestEntity2d->SetAngleXYZ(0, 0, 0);

	if (type == WAVE_ALPHA)
	{
		m_pIndieLib->SurfaceManager->Clone(&m_ClonedSourceSurface, m_OriginalSourceSurface);
		m_ClonedSourceSurface.SetGrid(8, 8);
		m_SourceEntity2d->SetSurface(&m_ClonedSourceSurface);

		m_pIndieLib->SurfaceManager->Clone(&m_ClonedDestSurface, m_OriginalDestSurface);
		m_ClonedDestSurface.SetGrid(8, 8);
		m_DestEntity2d->SetSurface(&m_ClonedDestSurface);

		m_NumBlocksX			= m_ClonedSourceSurface.GetBlocksX();
		m_NumBlocksY			= m_ClonedSourceSurface.GetBlocksY();
		m_WidthBlock			= m_ClonedSourceSurface.GetWidthBlock();
		m_HeightBlock			= m_ClonedSourceSurface.GetHeightBlock();

		m_Timer.Start();
	}

	
	if (type == RANDOM)
	{
		stx_srand((unsigned int)time(0));
		switch (stx_rand() % 27)
		{	
			case  0: m_Type = ALPHA2; break;
			case  1: m_Type = WAVE_ALPHA; break;
			case  2: m_Type = WIPE_LEFT; break;
			case  3: m_Type = WIPE_RIGHT; break;
			case  4: m_Type = WIPE_UP; break;
			case  5: m_Type = WIPE_DOWN; break;
			case  6: m_Type = SCROLL_LEFT; break;
			case  7: m_Type = SCROLL_RIGHT; break;
			case  8: m_Type = SCROLL_UP; break;
			case  9: m_Type = SCROLL_DOWN; break;
			case 10: m_Type = SQUEEZE_LEFT; break;
			case 11: m_Type = SQUEEZE_RIGHT; break;
			case 12: m_Type = SQUEEZE_UP; break;
			case 13: m_Type = SQUEEZE_DOWN; break;
			case 14: m_Type = SHRINK; break;
			case 15: m_Type = SHRINK_SPIN_CLOCKWISE; break;
			case 16: m_Type = SHRINK_SPIN_COUNTERCLOCKWISE; break;
			case 17: m_Type = SLIDE_LEFT; break;
			case 18: m_Type = SLIDE_RIGHT; break;
			case 19: m_Type = SLIDE_UP; break;
			case 20: m_Type = SLIDE_DOWN; break;
			case 21: m_Type = SLIDE_OUT_LEFT; break;
			case 22: m_Type = SLIDE_OUT_RIGHT; break;
			case 23: m_Type = SLIDE_OUT_UP; break;
			case 24: m_Type = SLIDE_OUT_DOWN; break;
			case 25: m_Type = EXPAND; break;
			case 26: m_Type = EXPAND_SPIN_CLOCKWISE; break;
			case 27: m_Type = EXPAND_SPIN_COUNTERCLOCKWISE; break;
			case 28: m_Type = FADEOUT; break;
		}
	}
	else
	{
		m_Type = type;
	}
}

Transition::~Transition( )
{
	// Return the surface to manager back to the original
	if (m_Type == WAVE_ALPHA)
	{
		m_SourceEntity2d->SetSurface(m_OriginalSourceSurface);
		m_DestEntity2d->SetSurface(m_OriginalDestSurface);
		m_Timer.Stop();
	}
}

bool Transition::IsComplete()
{
	return (m_fTime > m_fDuration);
}

void Transition::Tick(float fElapsedTime)
{
	float time;

	m_fTime += fElapsedTime;
	m_fPercent = m_fTime / m_fDuration;
	if (m_fPercent > 1.0f)
		m_fPercent = 1.0f;

	switch(m_Type)
	{
	case ALPHA2:
        m_Source_alpha = 255 - (int)(255 * m_fPercent);
		m_Destination_alpha = (int)(255 * m_fPercent);
		m_SourceEntity2d->SetTransparency( m_Source_alpha);
		m_DestEntity2d->SetTransparency(m_Destination_alpha);
		break;

	case WAVE_ALPHA:
		m_Source_alpha = 255 - (int)(255 * m_fPercent);
		m_Destination_alpha = (int)(255 * m_fPercent);
		m_SourceEntity2d->SetTransparency( m_Source_alpha);
		m_DestEntity2d->SetTransparency(m_Destination_alpha);
 
		// Update grid vertices for making a "wave" effect
		time = m_Timer.GetTicks() / 1000.0f;
		
		for (int i = 1; i < m_NumBlocksX; i++)
			for (int j = 1; j < m_NumBlocksY; j++)
				m_DestEntity2d->GetSurface()->SetVertexPos (j, 
										  i, 
										  (int) ((j * m_HeightBlock + cosf (time * 10 + (i + j) / 2) * 5)), 
										  (int) ((i * m_WidthBlock  + sinf (time * 10 + (i + j) / 2) * 5)));


		break;

	case WIPE_LEFT:
		m_SourceEntity2d->SetRegion(0, 0, (int)(m_Width - m_Width * m_fPercent), m_Height);
		m_DestEntity2d->SetRegion(0, 0, m_Width, m_Height);
		m_SourceEntity2d->SetPosition(0, 0, 1);
		break;

	case WIPE_RIGHT:
		m_SourceEntity2d->SetRegion(0, 0, m_Width, m_Height);
		m_DestEntity2d->SetRegion(0, 0, (int)(m_Width * m_fPercent), m_Height);
		m_DestEntity2d->SetPosition(0, 0, 1);
		break;

	case WIPE_UP:
		m_SourceEntity2d->SetRegion(0, 0, m_Width, (int)(m_Height * (1.0f - m_fPercent)));
		m_DestEntity2d->SetRegion(0, 0, m_Width, m_Height);
		m_SourceEntity2d->SetPosition(0, 0, 1);
		break;

	case WIPE_DOWN:
		m_DestEntity2d->SetRegion(0, 0, m_Width, (int)(m_Height * m_fPercent));
		m_SourceEntity2d->SetRegion(0, 0, m_Width, m_Height);
		m_DestEntity2d->SetPosition(0, 0, 1);
		break;

	case SCROLL_LEFT:
		m_Source_x = -m_Width * m_fPercent;
		m_Source_y = 0;
		m_Destination_x = m_Width - m_Width * m_fPercent;
		m_Destination_y = 0;
		m_SourceEntity2d->SetPosition(m_Source_x, m_Source_y, 0);
		m_DestEntity2d->SetPosition(m_Destination_x, m_Destination_y, 0);
		break;
	
	case SCROLL_RIGHT:
        m_Source_x = m_Width * m_fPercent;
		m_Source_y = 0;
		m_Destination_x = m_Width * m_fPercent - m_Width;
		m_Destination_y = 0;
		m_SourceEntity2d->SetPosition(m_Source_x, m_Source_y, 0);
		m_DestEntity2d->SetPosition(m_Destination_x, m_Destination_y, 0);
		break;

	case SCROLL_UP:
		m_Source_x = 0;
		m_Source_y = - m_Height * m_fPercent;
		m_Destination_x = 0;
		m_Destination_y = m_Height - m_Height * m_fPercent;
		m_SourceEntity2d->SetPosition(m_Source_x, m_Source_y, 0);
		m_DestEntity2d->SetPosition(m_Destination_x, m_Destination_y, 0);
		break;
	
	case SCROLL_DOWN:
		m_Source_x = 0;
		m_Source_y = m_Height * m_fPercent;
		m_Destination_x = 0;
		m_Destination_y = m_Height * m_fPercent - m_Height;
		m_SourceEntity2d->SetPosition(m_Source_x, m_Source_y, 0);
		m_DestEntity2d->SetPosition(m_Destination_x, m_Destination_y, 0);
		break;

	case SQUEEZE_LEFT:
		m_Source_x_scale = (1.0f - m_fPercent);
		m_Source_x = 0;
		m_Destination_x_scale = m_fPercent;
		m_Destination_x = m_Width - m_Destination_x_scale * m_Width;
		m_SourceEntity2d->SetScale(m_Source_x_scale, m_Source_y_scale);
		m_SourceEntity2d->SetPosition(m_Source_x, m_Source_y, 0);
		m_DestEntity2d->SetScale(m_Destination_x_scale, m_Destination_y_scale);
		m_DestEntity2d->SetPosition(m_Destination_x, m_Destination_y, 0);
		break;

	case SQUEEZE_RIGHT:
		m_Source_x_scale = (1.0f - m_fPercent);
		m_Source_x = m_Width - m_Source_x_scale * m_Width;
		m_Destination_x_scale = m_fPercent;
		m_SourceEntity2d->SetScale(m_Source_x_scale, m_Source_y_scale);
		m_SourceEntity2d->SetPosition(m_Source_x, m_Source_y, 0);
		m_DestEntity2d->SetScale(m_Destination_x_scale, m_Destination_y_scale);
		break;

	case SQUEEZE_UP:
		m_Source_y_scale = (1.0000001f - m_fPercent);
		m_Destination_y_scale = m_fPercent;
		m_Destination_y = m_Height - m_Height * m_fPercent;
		m_SourceEntity2d->SetScale(m_Source_x_scale, m_Source_y_scale);
		m_SourceEntity2d->SetPosition(m_Source_x, m_Source_y, 0);
		m_DestEntity2d->SetScale(m_Destination_x_scale, m_Destination_y_scale);
		m_DestEntity2d->SetPosition(m_Destination_x, m_Destination_y, 0);
		break;

	case SQUEEZE_DOWN:
		m_Source_y_scale = (1.0000001f - m_fPercent);
		m_Source_y = m_Height - m_Source_y_scale * m_Height;
		m_Destination_y_scale = m_fPercent;
		m_SourceEntity2d->SetScale(m_Source_x_scale, m_Source_y_scale);
		m_SourceEntity2d->SetPosition(m_Source_x, m_Source_y, 0);
		m_DestEntity2d->SetScale(m_Destination_x_scale, m_Destination_y_scale);
		m_DestEntity2d->SetPosition(m_Destination_x, m_Destination_y, 0);
		break;

	case SHRINK:
		m_SourceEntity2d->SetHotSpot(0.5f, 0.5f);
		m_Source_x_scale = (1.0f - m_fPercent);
		m_Source_y_scale = (1.0f - m_fPercent);
		m_Source_x = (float)m_Width / 2;
		m_Source_y = (float)m_Height / 2;
		m_SourceEntity2d->SetPosition(m_Source_x, m_Source_y, 0);
		m_SourceEntity2d->SetScale(m_Source_x_scale, m_Source_y_scale);
		break;

	case SHRINK_SPIN_CLOCKWISE:
		m_Source_rot = m_fPercent * 360 * 2;  // 2 revolutions
		m_Source_x_scale = (1.0f - m_fPercent);
		m_Source_y_scale = (1.0f - m_fPercent);
		m_Source_x = (float)m_Width / 2;
		m_Source_y = (float)m_Height / 2;
		m_SourceEntity2d->SetHotSpot(0.5f, 0.5f);
		m_SourceEntity2d->SetPosition(m_Source_x, m_Source_y, 1);
		m_SourceEntity2d->SetScale(m_Source_x_scale, m_Source_y_scale);
		m_SourceEntity2d->SetAngleXYZ(0, 0, m_Source_rot);
		break;

	case SHRINK_SPIN_COUNTERCLOCKWISE:
		m_Source_rot = -m_fPercent * 360 * 2;  // 2 revolutions
		m_Source_x_scale = (1.0f - m_fPercent);
		m_Source_y_scale = (1.0f - m_fPercent);
		m_Source_x = (float)m_Width / 2;
		m_Source_y = (float)m_Height / 2;
		m_SourceEntity2d->SetHotSpot(0.5f, 0.5f);
		m_SourceEntity2d->SetPosition(m_Source_x, m_Source_y, 1);
		m_SourceEntity2d->SetScale(m_Source_x_scale, m_Source_y_scale);
		m_SourceEntity2d->SetAngleXYZ(0, 0, m_Source_rot);
		break;

	case SLIDE_LEFT:
		m_Destination_x = (m_Width - m_Width * m_fPercent);
		m_DestEntity2d->SetPosition(m_Destination_x, m_Destination_y, 1);
		break;

	case SLIDE_RIGHT:
		m_Source_x = m_Width * m_fPercent - m_Width;
		m_DestEntity2d->SetPosition(m_Source_x, m_Source_y, 1);
		break;

	case SLIDE_UP:
		m_Destination_y = m_Height - (m_Height * m_fPercent);
		m_DestEntity2d->SetPosition(m_Destination_x, m_Destination_y, 1);
		break;

	case SLIDE_DOWN:
		m_Destination_y = m_Height * m_fPercent - m_Height;
		m_DestEntity2d->SetPosition(m_Destination_x, m_Destination_y, 1);
		break;

	case SLIDE_OUT_LEFT:
		m_Source_x = - (m_Width * m_fPercent);
		m_SourceEntity2d->SetPosition(m_Source_x, m_Source_y, 1);
		break;

	case SLIDE_OUT_RIGHT:
		m_Source_x = m_Width * m_fPercent;
		m_SourceEntity2d->SetPosition(m_Source_x, m_Source_y, 1);
		break;

	case SLIDE_OUT_UP:
		m_Destination_y =  - (m_Height * m_fPercent);
		m_SourceEntity2d->SetPosition(m_Destination_x, m_Destination_y, 1);
		break;

	case SLIDE_OUT_DOWN:
		m_Source_y = m_Height * m_fPercent;
		m_SourceEntity2d->SetPosition(m_Source_x, m_Source_y, 1);
		break;

	case EXPAND:
		m_Destination_x_scale = m_fPercent;
		m_Destination_y_scale = m_fPercent;
		m_Destination_x = (float)(m_Width / 2);
		m_Destination_y = (float)(m_Height / 2);
		m_DestEntity2d->SetHotSpot(0.5f, 0.5f);
		m_DestEntity2d->SetPosition(m_Destination_x, m_Destination_y, 1);
		m_DestEntity2d->SetScale(m_Destination_x_scale, m_Destination_y_scale);
		break;

	case EXPAND_SPIN_CLOCKWISE:
		m_Destination_rot = m_fPercent * 360 * 2;  // 2 revolutions
		m_Destination_x_scale = m_fPercent;
		m_Destination_y_scale = m_fPercent;
		m_Destination_x = (float)(m_Width / 2);
		m_Destination_y = (float)(m_Height / 2);
		m_DestEntity2d->SetHotSpot(0.5f, 0.5f);
		m_DestEntity2d->SetPosition(m_Destination_x, m_Destination_y, 1);
		m_DestEntity2d->SetScale(m_Destination_x_scale, m_Destination_y_scale);
		m_DestEntity2d->SetAngleXYZ(0, 0, m_Destination_rot);
		break;

	case EXPAND_SPIN_COUNTERCLOCKWISE:
		m_Destination_rot = -m_fPercent * 360 * 2;  // 2 revolutions
		m_Destination_x_scale = m_fPercent;
		m_Destination_y_scale = m_fPercent;
		m_Destination_x = (float)(m_Width / 2);
		m_Destination_y = (float)(m_Height / 2);
		m_DestEntity2d->SetHotSpot(0.5f, 0.5f);
		m_DestEntity2d->SetPosition(m_Destination_x, m_Destination_y, 1);
		m_DestEntity2d->SetScale(m_Destination_x_scale, m_Destination_y_scale);
		m_DestEntity2d->SetAngleXYZ(0, 0, m_Destination_rot);
		break;

	default : break;

	}// end switch

	
}

float Transition::GetPercentComplete()
{
	return m_fPercent;
}
