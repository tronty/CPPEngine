/*
	Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#pragma once
//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
enum
{
IDC_TOGGLEFULLSCREEN,
IDC_TOGGLEREF,
IDC_CHANGEDEVICE,
IDC_LAST
};

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
class GlowDemo;
GlowDemo *				g_pDemo;

CModelViewerCamera      g_Camera;               // A model viewing camera
bool					g_bShowHelp = false;
bool					g_bShowUI = true;
