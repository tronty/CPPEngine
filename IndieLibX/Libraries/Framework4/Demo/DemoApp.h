
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

#pragma once

#include "../Platform.h"
#include "../Config.h"
#include "../Graphics/Renderer.h"
#include "../Util/Timer.h"
#include "../Util/Queue.h"
#include "../Math/Vector.h"

#include "../GUI/Dialog.h"
#include "../GUI/CheckBox.h"
#include "../GUI/Label.h"
#include "../GUI/Slider.h"
#include "../GUI/DropDownList.h"


/*
template <typename X>
struct NulledPointer
{
public:
	NulledPointer() { ptr = nullptr; }
	NulledPointer(X*&& p) { ptr = p; }
	~NulledPointer() { DestroyTexture(app->GetDevice(), ptr); }

	force_inline void operator=(X* p) { ptr = p; }
	force_inline X& operator*() const { return *ptr; }
	force_inline X* operator->() const { return ptr; }

	force_inline operator X *() const { return ptr; }
private:
	X* ptr;
};
*/

//typedef NulledPointer <struct STexture> TextureHolder;


class DemoApp : public CheckBoxListener, public SliderListener, public DropDownListener, public PushButtonListener
{
public:
	DemoApp();
	virtual ~DemoApp();

	bool Create();
	void Destroy();

	void Start();
	void Update();
	void Close();

	virtual void ResetCamera();
	virtual void UpdateCamera();
	void Controls();

	void MakeFrame();

	virtual bool OnKey(const uint key, const bool pressed);
	virtual bool OnMouseButton(const int x, const int y, const MouseButton button, const bool pressed);
	virtual bool OnMouseWheel(const int x, const int y, const int scroll);
	virtual bool OnMouseMove(const int x, const int y);

	virtual bool OnSize(const uint width, const uint height);

	void UpdateKey(const uint key, const bool pressed) { m_Keys[key] = pressed; }
	void UpdateMouse(const int dx, const int dy);
	void CaptureMouse(const bool capture);
	void SetInputFocus(const bool focus) { m_HasInputFocus = focus; }

	virtual void OnCheckBoxClicked(CheckBox* checkbox);
	virtual void OnSliderChanged(Slider* slider);
	virtual void OnDropDownChanged(DropDownList* list);
	virtual void OnButtonClicked(PushButton* button);

	void ApplyDisplayMode(bool recreate_pipelines);
	virtual void CreateRenderSetups(){}
	virtual void DestroyRenderSetups(){}
	virtual bool CreatePipelines(){ return true; }
	virtual void DestroyPipelines(){}

	virtual bool Init(){ return true; }
	virtual void Exit(){}

	virtual void DrawFrame(Context context, uint buffer_index) = 0;

	Device GetDevice() const { return m_Device; }

protected:
	void InitGUI();
	void DrawGUI(Context context, uint buffer_index, const SProfileData* profile_data, uint profile_data_count);

	Device m_Device;

	DeviceParams m_DeviceParams;

	Timer m_Timer;
	TexFont m_Font;
	Primitives m_Primitives;

	float3 m_CamPos;
	float3 m_CamDir;
	float  m_CamSpeed;
	float m_AngleX;
	float m_AngleY;

	bool m_Keys[256];
	bool m_HasInputFocus;
	bool m_CapturedMouse;
	bool m_InvertMouse;
	bool m_ShowFPS;
	bool m_ShowProfile;
	bool m_ShowPosition;
	bool m_TakeScreenshot;

	Queue <Widget *> m_Widgets;
	Dialog *m_ConfigDialog;
	CheckBox *m_InvertMouseBox, *m_FullscreenBox, *m_VSyncBox;
	Slider *m_MouseSensibilitySlider;
	DropDownList *m_ResolutionList, *m_AntiAlias;
	PushButton *m_ApplyResolution;

	float m_MouseSensibility;
// 	Dialog *configDialog, *keysDialog, *joystickDialog;
// 	PushButton *applyRes, *configureKeys, *configureJoystick;



	SamplerTable m_LinearClamp;
	BlendState m_BlendSrcAlpha;

	Config m_Config;

	uint m_UploadBufferSize;
	uint m_InitialUploadBufferSize;
};
