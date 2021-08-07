/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
/*--------------------------------------------------------------------- NVMH5 -|----------------------
Path:  SDK\DEMOS\Direct3D9\src\Glow\
File:  GlowProfiles.cpp

Copyright NVIDIA_Corporation 2003
TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED *AS IS* AND NVIDIA AND
AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA
OR ITS SUPPLIERS BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES WHATSOEVER
INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF
BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE USE OF OR INABILITY TO USE THIS
SOFTWARE, EVEN IF NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.


Comments:


-------------------------------------------------------------------------------|--------------------*/

#include "nvafx.h"
#include "GlowProfiles.h"
#include "shared\NV_Common.h"
#include "shared\NV_Error.h"

int GlowProfiles::GetNumProfiles()
{
	return( 8 );
}

// larger m_fRadiusScale leads to narrower gaussian
void GlowProfiles::GetProfile( int num_profile, vector< float > * pVecF )
{
	RET_IF( pVecF == 0 );
	RET_IF( num_profile >= GetNumProfiles() );
	pVecF->clear();
	m_vGProps.clear();
	GaussianProperty gp;

	switch( num_profile )
	{
	case 0 :
		gp.m_fRadiusScale		= 0.18f;
		gp.m_fAmplitude			= 0.06f;
		m_vGProps.push_back( gp );
		gp.m_fRadiusScale		= 4.5f;
		gp.m_fAmplitude			= 0.11f;
		m_vGProps.push_back( gp );
		m_nKernelSize = 27;
		m_fColorAttenuation = 0.90f;
		break;
	case 1 :
		gp.m_fRadiusScale		= 0.25f;
		gp.m_fAmplitude			= 0.06f;
		m_vGProps.push_back( gp );
		gp.m_fRadiusScale		= 3.0f;
		gp.m_fAmplitude			= 0.15f;
		m_vGProps.push_back( gp );
		m_nKernelSize = 17;
		m_fColorAttenuation = 1.60f;
		break;
	case 2:
		// Code in CalculateProfile() turns #2 into a checkerboard
		// pattern with gaps every 2 texels
		gp.m_fRadiusScale		= 0.5f;
		gp.m_fAmplitude			= 0.55f;
		m_vGProps.push_back( gp );
		m_nKernelSize = 28 * 3;
		m_fColorAttenuation = 0.379f;
		break;
	case 3:
		// really big kernel
		gp.m_fRadiusScale		= 0.18f;
		gp.m_fAmplitude			= 0.24f;
		m_vGProps.push_back( gp );
		gp.m_fRadiusScale		= 4.5f;
		gp.m_fAmplitude			= 0.41f;
		m_vGProps.push_back( gp );
		m_nKernelSize = 44;
		m_fColorAttenuation = 0.20f;
		break;
	case 4:
		// more of a uniform square block from each bright source
		gp.m_fRadiusScale		= 0.2f;
		gp.m_fAmplitude			= 0.3f;
		m_vGProps.push_back( gp );
		gp.m_fRadiusScale		= 4.5f;
		gp.m_fAmplitude			= 0.11f;
		m_vGProps.push_back( gp );
		m_nKernelSize = 28;
		m_fColorAttenuation = 0.33f;
		break;
	case 5:
		gp.m_fRadiusScale		= 0.11f;
		gp.m_fAmplitude			= 0.1f;
		m_vGProps.push_back( gp );
		gp.m_fRadiusScale		= 4.5f;
		gp.m_fAmplitude			= 0.11f;
		m_vGProps.push_back( gp );
		m_nKernelSize = 21;
		m_fColorAttenuation = 0.90f;
		break;
	case 6:
		gp.m_fRadiusScale		= 0.09f;
		gp.m_fAmplitude			= 0.3f;
		m_vGProps.push_back( gp );
		m_nKernelSize = 28;
		m_fColorAttenuation = 0.90f;
		break;
	case 7:
		gp.m_fRadiusScale		= 18.2f;
		gp.m_fOffset			= 6.5f;
		gp.m_fAmplitude			= 0.3f;
		m_vGProps.push_back( gp );
		gp.m_fRadiusScale		= 18.2f;
		gp.m_fOffset			= -6.5f;
		gp.m_fAmplitude			= 0.3f;
		m_vGProps.push_back( gp );
		m_nKernelSize = 24;
		m_fColorAttenuation = 1.0f;
		break;

	default:
		FMsg("GlowProfiles::GetProfile Profile number too high!\n");
		break;
	}
	CalculateProfile( num_profile, pVecF ); 
}

// This function does not eliminate kernel elements with low coefficients.
// The GetProfile() that returns element coordinates does eliminate elements with low coefficients
void GlowProfiles::CalculateProfile( int num_profile, vector< float > * pVecF )
{
	RET_IF( pVecF == 0 );
	RET_IF( num_profile >= GetNumProfiles() );
	pVecF->clear();

	float cent = (((float)m_nKernelSize) - 1.0f ) / 2.0f;
	float x, tmp, radi, coefficient;

	int i;
	size_t n;
	for( i=0; i < m_nKernelSize; i++ )
	{
		coefficient = 0.0f;
		for( n=0; n < m_vGProps.size(); n++ )
		{
			x = ((float)i) - cent + m_vGProps.at(n).m_fOffset;
			radi = (x * x) / (cent * cent);
			tmp = radi * m_vGProps.at(n).m_fRadiusScale;
			tmp = (float) ( 1.0f / exp( tmp ) );
			coefficient += tmp * m_vGProps.at(n).m_fAmplitude;
		}

		// If flare type 2, make a flare that skips some texels, for a sparkly effect
		// This makes 3 texels, followed by a 3 texel gap.
		if( num_profile == 2 )
		{	
			if( (i%6) > 2 )
				coefficient = 0.0f;
		}
		pVecF->push_back( coefficient * m_fColorAttenuation );
	}
}

void GlowProfiles::GetProfile( int num_profile, vector< D3DXFROMWINEVECTOR4 > * pV4Coefs, 
								vector< D3DXFROMWINEVECTOR4 > * pV4Coords, KernelType type )
{
	RET_IF( pV4Coefs == 0 );
	RET_IF( pV4Coords == 0 );
	RET_IF( num_profile >= GetNumProfiles() );
	vector< float > vf;
	GetProfile( num_profile, &vf );
	pV4Coefs->clear();
	pV4Coords->clear();

	size_t i, j;
	float v;
	float x, y;
	int num_elems;
	num_elems = (int)(vf.size());
	float start_coord;
	start_coord = -(floor( num_elems / 2.0f ));

	switch( type )
	{
	case KT_HORIZONTAL:
		y = 0.0f;
		for( i=0; i < vf.size(); i++ )
		{
			x = (float)(start_coord + i)/m_fSourceResX + m_fTexCoordOffsetX;
			v = vf.at(i);
			if( v > m_fCoefLowerThreshold )
			{
				pV4Coefs->push_back( D3DXFROMWINEVECTOR4( v, v, v, v ));
				pV4Coords->push_back( D3DXFROMWINEVECTOR4( x, y, 0.0f, 0.0f ));
			}
		}
		break;
	case KT_VERTICAL:
		x = 0.0f;
		for( i=0; i < vf.size(); i++ )
		{
			y = (float)(start_coord + i)/m_fSourceResY + m_fTexCoordOffsetY;
			v = vf.at(i);
			if( v > m_fCoefLowerThreshold )
			{
				pV4Coefs->push_back( D3DXFROMWINEVECTOR4( v, v, v, v ));
				pV4Coords->push_back( D3DXFROMWINEVECTOR4( x, y, 0.0f, 0.0f ));
			}
		}
		break;
	case KT_2DAREA:
		for( j=0; j < vf.size(); j++ )
		{
			y = (float)(start_coord + j)/m_fSourceResY + m_fTexCoordOffsetY;
			for( i=0; i < vf.size(); i++ )
			{
				x = (float)(start_coord + i)/m_fSourceResX + m_fTexCoordOffsetX;
				v = vf.at(i);
				if( v > m_fCoefLowerThreshold )
				{
					pV4Coefs->push_back( D3DXFROMWINEVECTOR4( v, v, v, v ));
					pV4Coords->push_back( D3DXFROMWINEVECTOR4( x, y, 0.0f, 0.0f ));
				}
			}
		}
		break;
	default:
		break;
	}
}

