/*
	Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
/*--------------------------------------------------------------------- NVMH5 -|----------------------
Path:  Sdk\Demos\Direct3D9\src\Glow\
File:  GlowProfiles.h

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

#ifndef H_D3DGLOWPROFILES_H
#define H_D3DGLOWPROFILES_H

#include <vector>
using namespace std;

// width and amplitude of a Gaussian 
struct GaussianProperty
{
	float m_fRadiusScale;
	float m_fOffset;
	float m_fAmplitude;
	GaussianProperty()
	{
		m_fRadiusScale	= 0.0f;
		m_fOffset		= 0.0f;
		m_fAmplitude	= 0.0f;
	}
};


class GlowProfiles
{
public:
	enum KernelType
	{
		KT_HORIZONTAL,
		KT_VERTICAL,
		KT_2DAREA,
		KT_FORCEDWORD = 0xFFFFFFF
	};

	// Half-texel texture coordinate offsets to sample from texel centers
	// Set these before you call GetProfile(..)
	float	m_fTexCoordOffsetX;
	float	m_fTexCoordOffsetY;
	float	m_fSourceResX;
	float	m_fSourceResY;
	float	m_fCoefLowerThreshold;		// elements with coeficients lower than this 
										// are eliminated in GetProfile( .., coefs, coords, type );
	float	m_fColorAttenuation;		// each kernel coefficient is multiplied by this

	int		GetNumProfiles();
	void	GetProfile( int num_profile, vector< float > * pVecF );
	void	GetProfile( int num_profile, 
						vector< D3DXFROMWINEVECTOR4 > * pV4Coefs,
						vector< D3DXFROMWINEVECTOR4 > * pV4Coords,
						KernelType type );
	GlowProfiles()
	{
		m_fCoefLowerThreshold = 0.01f;
		m_fTexCoordOffsetX = 0.0f;
		m_fTexCoordOffsetY = 0.0f;
		m_fSourceResX = m_fSourceResY = 0.0f;
	}
protected:
	vector< GaussianProperty >	m_vGProps;
	int							m_nKernelSize;

	void CalculateProfile( int num_profile, vector< float > * pVecF );
};

#endif

