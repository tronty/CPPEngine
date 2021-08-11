
/* * * * * * * * * * * * * Author's note * * * * * * * * * * * *\
*   _       _   _       _   _       _   _       _     _ _ _ _   *
*  |_|     |_| |_|     |_| |_|_   _|_| |_|     |_|  _|_|_|_|_|  *
*  |_|_ _ _|_| |_|     |_| |_|_|_|_|_| |_|     |_| |_|_ _ _     *
*  |_|_|_|_|_| |_|     |_| |_| |_| |_| |_|     |_|   |_|_|_|_   *
*  |_|     |_| |_|_ _ _|_| |_|     |_| |_|_ _ _|_|  _ _ _ _|_|  *
*  |_|     |_|   |_|_|_|   |_|     |_|   |_|_|_|   |_|_|_|_|    *
*                                                               *
*                     http://www.humus.name                     *
*                                                                *
* This file is a part of the work done by Humus. You are free to   *
* use the code in any way you like, modified, unmodified or copied   *
* into your own work. However, I expect you to respect these points:  *
*  - If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  - For use in anything commercial, please request my approval.     *
*  - Share your work and ideas too as much as you can.             *
*                                                                *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "DemoApp.h"

#include <stdio.h>

DemoApp::DemoApp()
	: m_Device(nullptr)
	, m_CamSpeed(1.0f)
	, m_HasInputFocus(true)
	, m_CapturedMouse(false)
	, m_InvertMouse(false)
	, m_ShowFPS(true)
	, m_ShowProfile(false)
	, m_ShowPosition(false)
	, m_TakeScreenshot(false)
	, m_MouseSensibility(0.005f)
	, m_UploadBufferSize(0)
	, m_InitialUploadBufferSize(0)
{
	memset(m_Keys, 0, sizeof(m_Keys));

	/*bool loaded = */m_Config.Load("Settings.ini");
	//ASSERT(loaded);

	m_Config.SetSection("Mouse");
	m_InvertMouse = m_Config.GetBool("InvertMouse", m_InvertMouse);
	m_MouseSensibility = m_Config.GetFloat("MouseSensibility", m_MouseSensibility);

	m_Config.SetSection("Display");
	m_ShowFPS = m_Config.GetBool("ShowFPS", m_ShowFPS);
	m_ShowProfile = m_Config.GetBool("ShowProfile", m_ShowProfile);
}

DemoApp::~DemoApp()
{
	m_Config.CreateSection("Mouse");
	m_Config.SetBool("InvertMouse", m_InvertMouse);
	m_Config.SetFloat("MouseSensibility", m_MouseSensibility);

	m_Config.CreateSection("Display");
	m_Config.SetBool("ShowFPS", m_ShowFPS);
	m_Config.SetBool("ShowProfile", m_ShowProfile);

	if (m_Widgets.GoToFirst())
	{
		do
		{
			delete m_Widgets.GetCurrent();
		} while (m_Widgets.GoToNext());
	}

	m_Config.Save("Settings.ini");
}

bool DemoApp::Create()
{
	m_Config.SetSection("Video");
	m_DeviceParams.m_FullscreenWidth  = m_Config.GetInteger("FullscreenWidth",  1024);
	m_DeviceParams.m_FullscreenHeight = m_Config.GetInteger("FullscreenHeight", 768);
	m_DeviceParams.m_WindowedWidth    = m_Config.GetInteger("WindowedWidth",    800);
	m_DeviceParams.m_WindowedHeight   = m_Config.GetInteger("WindowedHeight",   600);
	m_DeviceParams.m_MSAA       = m_Config.GetInteger("MSAA",    4);
	m_DeviceParams.m_Fullscreen = m_Config.GetBool("Fullscreen", false);
	m_DeviceParams.m_VSync      = m_Config.GetBool("VSync",      false);
	
	m_DeviceParams.m_Width  = m_DeviceParams.m_Fullscreen? m_DeviceParams.m_FullscreenWidth  : m_DeviceParams.m_WindowedWidth;
	m_DeviceParams.m_Height = m_DeviceParams.m_Fullscreen? m_DeviceParams.m_FullscreenHeight : m_DeviceParams.m_WindowedHeight;

	m_Device = CreateDevice(m_DeviceParams);
	if (!m_Device) return false;

	m_BlendSrcAlpha = CreateBlendState(m_Device, BF_SRC_ALPHA, BF_INV_SRC_ALPHA, BM_ADD, 0xF, false);

	Context context = GetMainContext(m_Device);
	BeginContext(context, m_InitialUploadBufferSize, "Startup", false);

	m_Primitives.CreateResources(m_Device, GetDefaultBlendState(m_Device), m_BlendSrcAlpha);
	m_Font.CreateResources(m_Device, "../Textures/Fonts/Consolas.dds", m_BlendSrcAlpha);
	m_Font.SetWindowSize(m_Device, m_DeviceParams.m_Width, m_DeviceParams.m_Height);

	const SSamplerDesc sampler = { FILTER_LINEAR, 1, AM_CLAMP, AM_CLAMP, AM_CLAMP };
	if ((m_LinearClamp = CreateSamplerTable(m_Device, m_Primitives.GetRootSignature(), 3, &sampler, 1)) == nullptr) return false;

	CheckBox::Init(m_Device, m_Primitives.GetRootSignature());

	InitGUI();

	return true;
}

void DemoApp::Destroy()
{
	m_Config.CreateSection("Video");
	m_Config.SetInteger("FullscreenWidth",  m_DeviceParams.m_FullscreenWidth);
	m_Config.SetInteger("FullscreenHeight", m_DeviceParams.m_FullscreenHeight);
	m_Config.SetInteger("WindowedWidth",    m_DeviceParams.m_WindowedWidth);
	m_Config.SetInteger("WindowedHeight",   m_DeviceParams.m_WindowedHeight);
	m_Config.SetInteger("MSAA",    m_DeviceParams.m_MSAA);
	m_Config.SetBool("Fullscreen", m_DeviceParams.m_Fullscreen);
	m_Config.SetBool("VSync",      m_DeviceParams.m_VSync);

	m_Font.DestroyResources(m_Device);
	m_Primitives.DestroyResources(m_Device);

	CheckBox::Destroy(m_Device);
	DestroyBlendState(m_Device, m_BlendSrcAlpha);
	DestroySamplerTable(m_Device, m_LinearClamp);

	DestroyDevice(m_Device);
}

void DemoApp::InitGUI()
{
	const float w = 400;
	const float h = 330;

	char str[32];

	m_ConfigDialog = new Dialog(0.5f * (m_DeviceParams.m_Width - w), 0.5f * (m_DeviceParams.m_Height - h), w, h, false, true);
	m_ConfigDialog->SetVisible(false);

	int tab = m_ConfigDialog->AddTab("Options");

	m_InvertMouseBox = new CheckBox(0, 0, 180, 36, "Invert mouse", m_InvertMouse);
	m_InvertMouseBox->SetListener(this);
	m_ConfigDialog->AddWidget(tab, m_InvertMouseBox);

	m_ConfigDialog->AddWidget(tab, new Label(0, 40, 192, 36, "Mouse sensitivity"));
	m_MouseSensibilitySlider = new Slider(0, 80, 300, 24, 0.0005f, 0.01f, m_MouseSensibility);
	m_MouseSensibilitySlider->SetListener(this);
	m_ConfigDialog->AddWidget(tab, m_MouseSensibilitySlider);

	m_ConfigDialog->AddWidget(tab, new Label(0, 120, 250, 36, "Resolution"));
	m_ResolutionList = new DropDownList(0, 160, 250, 36);
	uint display_mode_count = GetDisplayModeCount(m_Device);
	for (uint i = 0; i < display_mode_count; i++)
	{
		const DisplayMode &mode = GetDisplayMode(m_Device, i);

		const char *ratio = GetAspectRatioString(GetAspectRatio(mode.m_Width, mode.m_Height));

		sprintf(str, "(%s) %ux%u", ratio, mode.m_Width, mode.m_Height);
		int index = m_ResolutionList->AddItem(str);
		if (mode.m_Width == m_DeviceParams.m_FullscreenWidth && mode.m_Height == m_DeviceParams.m_FullscreenHeight)
		{
			m_ResolutionList->SelectItem(index);
		}
	}
	m_ResolutionList->SetListener(this);
	m_ResolutionList->SetEnabled(m_DeviceParams.m_Fullscreen);
	m_ConfigDialog->AddWidget(tab, m_ResolutionList);


	m_ConfigDialog->AddWidget(tab, new Label(260, 120, 192, 36, "MSAA"));
	m_AntiAlias = new DropDownList(260, 160, 100, 36);
	m_AntiAlias->AddItem("None");
	m_AntiAlias->SelectItem(0);

	bool has_msaa = false;
	for (int n = 2; n <= 8; n *= 2)
	{
		if (SupportsMSAAMode(m_Device, n))
		{
			sprintf(str, "%dx", n);
			int index = m_AntiAlias->AddItem(str);
			if (n == m_DeviceParams.m_MSAA)
				m_AntiAlias->SelectItem(index);
			has_msaa = true;
		}
	}

	m_AntiAlias->SetEnabled(has_msaa);
	m_AntiAlias->SetListener(this);
	m_ConfigDialog->AddWidget(tab, m_AntiAlias);


	m_FullscreenBox = new CheckBox(0, 200, 140, 36, "Fullscreen", m_DeviceParams.m_Fullscreen);
	m_FullscreenBox->SetListener(this);
	m_ConfigDialog->AddWidget(tab, m_FullscreenBox);

	m_VSyncBox = new CheckBox(0, 236, 140, 36, "VSync", m_DeviceParams.m_VSync);
	m_VSyncBox->SetListener(this);
	m_ConfigDialog->AddWidget(tab, m_VSyncBox);

	m_ApplyResolution = new PushButton(260, 220, 100, 36, "Apply");
	m_ApplyResolution->SetListener(this);
	m_ConfigDialog->AddWidget(tab, m_ApplyResolution);

/*	configureKeys = new PushButton(200, 6, 198, 32, "Configure keys");
	configureKeys->setListener(this);
	m_ConfigDialog->addWidget(tab, configureKeys);

	configureJoystick = new PushButton(200, 42, 198, 32, "Configure joystick");
	configureJoystick->setListener(this);
	m_ConfigDialog->addWidget(tab, configureJoystick);
*/



	m_Widgets.AddFirst(m_ConfigDialog);
}

void DemoApp::Start()
{
	m_Timer.Reset();

	ResetCamera();


	Context context = GetMainContext(m_Device);
	EndContext(context);

	SubmitContexts(m_Device, 1, &context);
	Finish(m_Device);
}

void DemoApp::Update()
{
	m_Timer.Update();

	if (m_HasInputFocus)
	{
		Controls();
		UpdateCamera();
	}
}

void DemoApp::Close()
{
	if (m_Device)
		PostMessage(GetWindow(m_Device), WM_CLOSE, 0, 0);
}

void DemoApp::ResetCamera()
{
	m_CamPos = float3(0.0f, 0.0f, 0.0f);
	m_CamDir = float3(0.0f, 0.0f, 0.0f);

	m_AngleX = 0.0f;
	m_AngleY = 0.0f;
}

void DemoApp::UpdateCamera()
{
	m_CamPos += m_CamSpeed * m_Timer.GetFrameTime() * m_CamDir;
}

void DemoApp::Controls()
{
	// Compute directional vectors from euler angles
	float cosX = cosf(m_AngleY);
	float sinX = sinf(m_AngleY);
	float cosY = cosf(m_AngleX);
	float sinY = sinf(m_AngleX);

	float3 dx(cosY, 0, sinY);
	float3 dy(-sinX * sinY,  cosX, sinX * cosY);
	float3 dz(-cosX * sinY, -sinX, cosX * cosY);

	m_CamDir = float3(0, 0, 0);

	if (m_Keys[VK_LEFT] || m_Keys['A'])
		m_CamDir -= dx;
	if (m_Keys[VK_RIGHT] || m_Keys['D'])
		m_CamDir += dx;
	if (m_Keys[VK_SHIFT])
		m_CamDir -= dy;
	if (m_Keys[VK_CONTROL])
		m_CamDir += dy;
	if (m_Keys[VK_DOWN] || m_Keys['S'])
		m_CamDir -= dz;
	if (m_Keys[VK_UP] || m_Keys['W'])
		m_CamDir += dz;

	float d = dot(m_CamDir, m_CamDir);
	if (d > 0.0f)
	{
		m_CamDir *= rsqrtf(d);
	}
}

void DemoApp::MakeFrame()
{
	Update();


	SProfileData profile_data[MAX_QUERY_COUNT];
	uint profile_data_count = GetProfileData(m_Device, profile_data);

	Context context = GetMainContext(m_Device);

	uint buffer_index = GetBackBufferIndex(m_Device);
	RenderSetup back_buffer_setup = GetBackBufferSetup(m_Device, buffer_index);

	BeginContext(context, m_UploadBufferSize, "Frame", m_ShowProfile);

		TransitionRenderSetup(context, back_buffer_setup, RS_PRESENT, RS_RENDER_TARGET);

		DrawFrame(context, buffer_index);
		DrawGUI(context, buffer_index, profile_data, profile_data_count);

		if (m_TakeScreenshot)
		{
		}

		TransitionRenderSetup(context, back_buffer_setup, RS_RENDER_TARGET, RS_PRESENT);

	EndContext(context);
	SubmitContexts(m_Device, 1, &context);

	if (m_TakeScreenshot)
	{
		m_TakeScreenshot = false;

		//Finish(m_Device);

	}


	Present(m_Device, m_DeviceParams.m_VSync);
}

void DemoApp::DrawGUI(Context context, uint buffer_index, const SProfileData* profile_data, uint profile_data_count)
{
	const float4x4 scale_bias
	(
		2.0f / (float) m_DeviceParams.m_Width, 0, 0, -1.0f,
		0, -2.0f / (float) m_DeviceParams.m_Height, 0,  1.0f,
		0, 0, 0, 0,
		0, 0, 0, 1
	);
	m_Primitives.SetMatrix(context, scale_bias);

	BeginRenderPass(context, "GUI", GetBackBufferRenderPass(m_Device), GetBackBufferSetup(m_Device, buffer_index));
	{
		if (m_Widgets.GoToLast())
		{
			// Draw widgets back to front
			do
			{
				Widget *widget = m_Widgets.GetCurrent();
				if (widget->IsDead())
				{
					// Remove dead widgets
					delete widget;
					m_Widgets.RemoveCurrent();
				}
				else if (widget->IsVisible())
				{
					widget->Draw(context, m_Primitives, m_Font, m_LinearClamp, m_BlendSrcAlpha/*, m_NoDepthTest*/);
				}
			} while (m_Widgets.GoToPrev());
		}

		if (m_ShowFPS || (m_ShowProfile && profile_data_count))
		{
			ScopeMarker(context, "FPS");
			if (m_ShowFPS)
			{
				static char str[16] = { '\0' };
				static int len = 0;
				static int n = 0;
				static float t = 0;
				t += m_Timer.GetFrameTime();
				n++;
				if (t > 0.1f)
				{
					len = sprintf(str, "%.0f", float(n) / t);
					t = 0;
					n = 0;
				}

				m_Font.DrawText(context, 10, 10, 40.0f, 40.0f, str, len);
			}
			if (m_ShowProfile && profile_data_count)
			{
				float freq = GetTimestampFrequency(m_Device);

				char str[256];
				uint64 time_stamp_stack[8];
				int stack = 0;

				float y = 50.0f + (profile_data_count) * 10.0f;
				for (int i = profile_data_count - 1; i >= 0; i--)
				{
					if (profile_data[i].m_Name)
					{
						--stack;
						ASSERT(stack >= 0);

						int64 time = time_stamp_stack[stack] - profile_data[i].m_TimeStamp;
						//ASSERT(time >= 0);

						int len = sprintf_s(str, "%s: %.3f", profile_data[i].m_Name, float(time) * freq);
						m_Font.DrawText(context, 10 + stack * 10.0f, y, 20.0f, 20.0f, str, len);
						y -= 20.0f;
					}
					else
					{
						ASSERT(stack < elementsof(time_stamp_stack));
						time_stamp_stack[stack] = profile_data[i].m_TimeStamp;
						++stack;
					}
				}
				ASSERT(stack == 0);
			}
		}

#ifdef DEBUG
		if (m_ShowPosition)
		{
			char str[64];
			int len = sprintf(str, "%.3f, %.3f, %.3f", m_CamPos.x, m_CamPos.y, m_CamPos.z);
			m_Font.DrawText(context, 200, 10, 20.0f, 20.0f, str, len);
			len = sprintf(str, "%.3f, %.3f", m_AngleX, m_AngleY);
			m_Font.DrawText(context, 200, 32, 20.0f, 20.0f, str, len);
		}
#endif
	}
	EndRenderPass(context, GetBackBufferSetup(m_Device, buffer_index));
}

bool DemoApp::OnKey(const uint key, const bool pressed)
{
	bool processed = false;

	if (!m_CapturedMouse)
	{
		if (m_Widgets.GoToFirst())
		{
			do
			{
				Widget *widget = m_Widgets.GetCurrent();
				if (widget->IsVisible() || widget->IsCapturing())
				{
					m_Widgets.MoveCurrentToTop();
					processed = widget->OnKey(key, pressed);
					break;
				}
			} while (m_Widgets.GoToNext());
		}
	}

	if (!processed)
	{
		if (pressed)
		{
			processed = true;
			if (key == KEY_ESCAPE)
			{
				if (!m_CapturedMouse || (m_DeviceParams.m_Fullscreen && m_CapturedMouse))
					Close();
				else
					CaptureMouse(false);
			}
			else if (key == /*fpsKey*/KEY_SPACE)
			{
				if (m_ShowFPS)
				{
					if (m_ShowProfile)
					{
						m_ShowFPS = false;
						m_ShowProfile = false;
					}
					else
					{
						m_ShowProfile = true;
					}
				}
				else
				{
					m_ShowFPS = true;
				}
			}
			else if (key == KEY_P)
			{
				m_ShowPosition = !m_ShowPosition;
			}
			else if (key == KEY_ENTER)
			{
				if (m_Keys[KEY_ALT])
				{
					// Toggle fullscreen
					m_DeviceParams.m_Fullscreen = !m_DeviceParams.m_Fullscreen;
					ApplyDisplayMode(false);
				}
				else
				{
					ResetCamera();
				}
			}
			else if (key == /*optionsKey*/KEY_F1)
			{
				if (m_ConfigDialog->IsVisible())
				{
					m_ConfigDialog->SetVisible(false);
					//if (keysDialog) keysDialog->setVisible(false);
					//if (joystickDialog) joystickDialog->setVisible(false);
				}
				else
				{
					CaptureMouse(false);
					m_ConfigDialog->SetVisible(true);
					//if (keysDialog) keysDialog->setVisible(true);
					//if (joystickDialog) joystickDialog->setVisible(true);
				}
			}
			else if (key == KEY_F9)
			{
				m_TakeScreenshot = true;
			}
			else
			{
				processed = false;
			}
		}
	}

	if (key < elementsof(m_Keys))
		m_Keys[key] = pressed;

	return processed;
}

bool DemoApp::OnMouseButton(const int x, const int y, const MouseButton button, const bool pressed)
{
	if (!m_CapturedMouse)
	{
		if (m_Widgets.GoToFirst())
		{
			do
			{
				Widget *widget = m_Widgets.GetCurrent();
				if (widget->IsEnabled() && widget->IsVisible() && (widget->IsInWidget(x, y) || widget->IsCapturing()))
				{
					m_Widgets.MoveCurrentToTop();
					return widget->OnMouseButton(x, y, button, pressed);
				}
			} while (m_Widgets.GoToNext());
		}

		if (button == MOUSE_LEFT && pressed)
		{
			CaptureMouse(true);
			return true;
		}
	}
	return false;
}

bool DemoApp::OnMouseWheel(const int x, const int y, const int scroll)
{
	if (!m_CapturedMouse)
	{
		if (m_Widgets.GoToFirst())
		{
			do
			{
				Widget *widget = m_Widgets.GetCurrent();
				if (widget->IsEnabled() && widget->IsVisible() && (widget->IsInWidget(x, y) || widget->IsCapturing()))
				{
					m_Widgets.MoveCurrentToTop();
					return widget->OnMouseWheel(x, y, scroll);
				}
			} while (m_Widgets.GoToNext());
		}
	}
	return false;
}

bool DemoApp::OnMouseMove(const int x, const int y)
{
	if (!m_CapturedMouse)
	{
		if (m_Widgets.GoToFirst())
		{
			do
			{
				Widget *widget = m_Widgets.GetCurrent();
				if (widget->IsEnabled() && widget->IsVisible() && (widget->IsInWidget(x, y) || widget->IsCapturing()))
				{
					return widget->OnMouseMove(x, y);
				}
			} while (m_Widgets.GoToNext());
		}
	}
	return false;
}

bool DemoApp::OnSize(const uint width, const uint height)
{
	DebugString("OnSize(%d, %d);", width, height);

	if (!m_DeviceParams.m_ModeChangeInProgress)
	{
		Finish(m_Device);
		DestroyRenderSetups();
	}

	if (!Resize(m_Device, m_DeviceParams, width, height))
		return false;

	if (!m_DeviceParams.m_ModeChangeInProgress)
		CreateRenderSetups();

	m_Font.SetWindowSize(m_Device, width, height);

	return true;
}

void DemoApp::UpdateMouse(const int dx, const int dy)
{
	if (m_CapturedMouse)
	{
		m_AngleX -= m_MouseSensibility * dx;
		m_AngleY += m_MouseSensibility * (m_InvertMouse? -dy : dy);

		m_AngleY = clamp(m_AngleY, -1.57f, 1.57f);
	}
}

void DemoApp::CaptureMouse(const bool capture)
{
#ifdef WIN32
	if (capture != m_CapturedMouse)
	{
		ShowCursor((BOOL) !capture);

		if (capture)
		{
			POINT point;
			GetCursorPos(&point);

			RECT rect;
			rect.left   = point.x;
			rect.right  = point.x;
			rect.top    = point.y;
			rect.bottom = point.y;
			ClipCursor(&rect);
		}
		else
		{
			ClipCursor(nullptr);
		}


		m_CapturedMouse = capture;
	}
#endif
}

void DemoApp::OnCheckBoxClicked(CheckBox* checkbox)
{
	if (checkbox == m_InvertMouseBox)
	{
		m_InvertMouse = m_InvertMouseBox->IsChecked();
	}
	else if (checkbox == m_FullscreenBox)
	{
		m_ResolutionList->SetEnabled(m_FullscreenBox->IsChecked());
	}
	else if (checkbox == m_VSyncBox)
	{
		m_DeviceParams.m_VSync = m_VSyncBox->IsChecked();
	}
}

void DemoApp::OnSliderChanged(Slider* slider)
{
	m_MouseSensibility = m_MouseSensibilitySlider->GetValue();
}

void DemoApp::OnDropDownChanged(DropDownList* list)
{
}

void DemoApp::OnButtonClicked(PushButton* button)
{
	if (button == m_ApplyResolution)
	{
		m_DeviceParams.m_Fullscreen = m_FullscreenBox->IsChecked();

		if (m_DeviceParams.m_Fullscreen)
		{
			int index = m_ResolutionList->GetSelectedItem();
			if (index >= 0)
			{
				const DisplayMode& mode = GetDisplayMode(m_Device, index);
				m_DeviceParams.m_FullscreenWidth  = mode.m_Width;
				m_DeviceParams.m_FullscreenHeight = mode.m_Height;
			}
		}

		const char *txt = m_AntiAlias->GetSelectedText();
		int msaa = max(atoi(txt), 1);
		bool msaa_changed = (m_DeviceParams.m_MSAA != msaa);
		m_DeviceParams.m_MSAA = msaa;

		ApplyDisplayMode(msaa_changed);
	}
/*	else if (button == configureKeys)
	{
		float w = 480;
		float h = 410;

		keysDialog = new Dialog(0.5f * (width - w), 0.5f * (height - h), w, h, true, false);
		keysDialog->setColor(vec4(0.2f, 0.3f, 1.0f, 0.8f));

		int tab = keysDialog->addTab("Keys");
		keysDialog->addWidget(tab, new KeyWaiterButton(210,  60, 100, 40, "Left",  &leftKey));
		keysDialog->addWidget(tab, new KeyWaiterButton(340,  60, 100, 40, "Right", &rightKey));
		keysDialog->addWidget(tab, new KeyWaiterButton(260,  10, 130, 40, "Forward",  &forwardKey));
		keysDialog->addWidget(tab, new KeyWaiterButton(260, 110, 130, 40, "Backward", &backwardKey));

		keysDialog->addWidget(tab, new KeyWaiterButton(10, 30, 180, 40, "Up/Jump",     &upKey));
		keysDialog->addWidget(tab, new KeyWaiterButton(10, 90, 180, 40, "Down/Crouch", &downKey));

		keysDialog->addWidget(tab, new KeyWaiterButton(10, 180, 190, 35, "Reset camera",   &resetKey));
		keysDialog->addWidget(tab, new KeyWaiterButton(10, 220, 190, 35, "Toggle FPS",     &fpsKey));
		keysDialog->addWidget(tab, new KeyWaiterButton(10, 260, 190, 35, "Options dialog", &optionsKey));
		keysDialog->addWidget(tab, new KeyWaiterButton(10, 300, 190, 35, "Screenshot",     &screenshotKey));

		widgets.addFirst(keysDialog);
	}
	else if (button == configureJoystick)
	{
		float w = 270;
		float h = 320;

		joystickDialog = new Dialog(0.5f * (width - w), 0.5f * (height - h), w, h, true, false);
		joystickDialog->setColor(vec4(0.2f, 0.3f, 1.0f, 0.8f));

		int tab = joystickDialog->addTab("Joystick");
		joystickDialog->addWidget(tab, new AxisWaiterButton(10, 10, 220, 35, "Left/Right", &xStrafeAxis, &invertXStrafeAxis));
		joystickDialog->addWidget(tab, new AxisWaiterButton(10, 50, 220, 35, "Up/Down", &yStrafeAxis, &invertYStrafeAxis));
		joystickDialog->addWidget(tab, new AxisWaiterButton(10, 90, 220, 35, "Forward/backward", &zStrafeAxis, &invertZStrafeAxis));

		joystickDialog->addWidget(tab, new AxisWaiterButton(10,  150, 105, 35, "Pitch", &xTurnAxis, &invertXTurnAxis));
		joystickDialog->addWidget(tab, new AxisWaiterButton(125, 150, 105, 35, "Yaw",   &yTurnAxis, &invertYTurnAxis));

		joystickDialog->addWidget(tab, new ButtonWaiterButton(30, 220, 180, 35, "Options dialog", &optionsButton));

		widgets.addFirst(joystickDialog);
	}*/
}

void DemoApp::ApplyDisplayMode(bool recreate_pipelines)
{
	Finish(m_Device);

	if (recreate_pipelines)
		DestroyPipelines();
	DestroyRenderSetups();

	SetDisplayMode(m_Device, m_DeviceParams);

	CreateRenderSetups();
	if (recreate_pipelines)
		CreatePipelines();
}
