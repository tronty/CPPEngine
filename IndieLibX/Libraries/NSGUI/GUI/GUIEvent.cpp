
#include <Framework3/IRenderer.h>
#include <GUI/GUIUtils.h>
void STXGUI::mouseScrolled     (){};
void STXGUI::mouseExit         (){};
GUIFrame* STXGUI::GetGuiFrame(){return guiFrame;}
STXGUI::STXGUI()
{
	mouseLocked = false;
}
STXGUI::~STXGUI(){}
void STXGUI::update(unsigned int cookie)
	{
		if(cookie)
			return;
		if(!guiFrame)
			return;
		guiFrame->setPosition(0,0);
    		guiFrame->setDimensions(float(IRenderer::GetRendererInstance()->GetViewportWidth()), float(IRenderer::GetRendererInstance()->GetViewportHeight()));		
  		guiFrame->setVisible(true);
    		guiFrame->forceUpdate(true);
		int fi=0;
  		guiFrame->render(fi);

		if(STX_Service::GetInputInstance()->IsMouseButtonPressed())
  			guiFrame->checkMouseEvents(CLICKED ); 
        	else if(STX_Service::GetInputInstance()->IsMouseDoubleClick())
  			guiFrame->checkMouseEvents(DOUBLE_CLICKED );
		else if(STX_Service::GetInputInstance()->IsMouseButtonReleased())
			guiFrame->checkMouseEvents(RELEASED);
        	else if(STX_Service::GetInputInstance()->IsMouseMotion())
		{
			if(STX_Service::GetInputInstance()->IsMouseButtonPressed())
				guiFrame->checkMouseEvents(DRAGGED );
			else 
				guiFrame->checkMouseEvents(MOVED   );
		}
		//if(STX_Service::GetInputInstance()->IsMouseButtonPressed(MBUTTON_WHEELDOWN))
           	//	mouseScrolled();

		GUIEvent* evt = (GUIEvent*)this->guiFrame->getWidgetByCallbackString("User Controls"); 
		if(evt) actionPerformed(*evt);
	}

	void STXGUI::init(std::string afile, std::string aUserControls)
	{
		if(!guiFrame)
		{
		guiFrame=new GUIFrame();
		LOG_FNLN_NONE;
  std::string pName=stx_convertpath(afile);
	LOG_FNLN_NONE;
	LOG_PRINT_NONE("pName=%s\n", pName.c_str());
  		if(guiFrame->GUIPanel::loadXMLSettings(pName.c_str()))
    			userControls = (GUIPanel*)this->guiFrame->getWidgetByCallbackString("User Controls");    
  		guiFrame->setGUIEventListener((GUIEventListener*)this);
  		guiFrame->setVisible(true);
		LOG_FNLN_NONE;
		LOG_PRINT_NONE("guiFrame->elements.size()=%d\n", guiFrame->elements.size());
		}
		else update();
	}

GUIFrame* g_guiFrame=0;
GUIPanel* g_userControls=0;

GUIFrame* STXGUI::guiFrame=0;
GUIPanel* STXGUI::userControls=0;

bool isClicked(GUIEvent &evt)
{
  GUIRectangle *sourceRectangle = evt.getEventSource();
  return sourceRectangle->isClicked();
}

GUIRectangle* GetGUIRectangle(GUIEvent &evt)
{
  const NSString &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int           widgetType      = sourceRectangle->getWidgetType();
  if(widgetType == RADIO_BUTTON)
    	return (GUIRadioButton*)sourceRectangle;
  else if(widgetType == CHECK_BOX)
    	return (GUICheckBox*)sourceRectangle;
  return 0;
}

void InitNSGUI(std::string aGUILayout, std::string aUserControls)
{
	if(!g_guiFrame)
	{
	g_guiFrame=new GUIFrame();
		LOG_FNLN_NONE;
  std::string pName=stx_convertpath(aGUILayout);
	LOG_FNLN_NONE;
	LOG_PRINT_NONE("pName=%s\n", pName.c_str());
  	if(g_guiFrame->GUIPanel::loadXMLSettings(pName.c_str()))
    		g_userControls = (GUIPanel*)g_guiFrame->getWidgetByCallbackString(aUserControls.c_str());
  	g_guiFrame->setGUIEventListener(0);//???(GUIEventListener*)this);
  	g_guiFrame->setVisible(true);
	}
	else UpdateNSGUI();
}
void UpdateNSGUI(std::string aUserControls)
{
	if(!g_guiFrame) return;
	g_guiFrame->setPosition(0,0);
	g_guiFrame->setDimensions(float(IRenderer::GetRendererInstance()->GetViewportWidth()), float(IRenderer::GetRendererInstance()->GetViewportHeight()));
  	g_guiFrame->setVisible(true);
	g_guiFrame->forceUpdate(true);
	int fi=0;
	g_guiFrame->render(fi);
	if(STX_Service::GetInputInstance()->IsMouseButtonPressed())
  		g_guiFrame->checkMouseEvents(CLICKED ); 
        else if(STX_Service::GetInputInstance()->IsMouseDoubleClick())
  		g_guiFrame->checkMouseEvents(DOUBLE_CLICKED );
	else if(STX_Service::GetInputInstance()->IsMouseButtonReleased())
		g_guiFrame->checkMouseEvents(RELEASED);
        else if(STX_Service::GetInputInstance()->IsMouseMotion())
	{
		if(STX_Service::GetInputInstance()->IsMouseButtonPressed())
			g_guiFrame->checkMouseEvents(DRAGGED );
		else 
			g_guiFrame->checkMouseEvents(MOVED   ); 
	}
		//if(STX_Service::GetInputInstance()->IsMouseButtonPressed(MBUTTON_WHEELDOWN))
           	//	mouseScrolled();

}

std::list <GUIEvent*> GUIevents;
void DisposeGUIEvent(GUIEvent* evt)
{
	delete evt;
}

bool isGUIEventsEmpty()
{
	return GUIevents.empty();
}

GUIEvent* PollGUIEvent()
{
  if(!GUIevents.empty())
  {
  	GUIEvent* evt=GUIevents.front();	
	GUIevents.pop_front();
	return evt;
  }
	return 0;
}
void PushGUIEvent(GUIEvent* evt)
{
	GUIevents.push_back(evt);
}

GUIEvent::GUIEvent()
{
  callbackString = "Unknown";
  eventSource    = 0;
}

GUIEvent::GUIEvent(GUIRectangle *element)
{
  if(element)
  {
  	callbackString = element? element->getCallbackString() : "Unknown";
  	eventSource    = element;
  }
  else
  {
  	callbackString = "Unknown";
  	eventSource    = 0;
  }
}

GUIEvent::GUIEvent(const GUIEvent& copy)
{
  this->operator =(copy);
}

GUIEvent &GUIEvent::operator= (const GUIEvent& copy)
{
  if(this != &copy)
  {
    callbackString = copy.callbackString;
    eventSource    = copy.eventSource;
  }
  return *this;
}

GUIRectangle *GUIEvent::getEventSource()   { return eventSource ;   }
const NSString &GUIEvent::getCallbackString(){ return callbackString; }
NSString GUIEvent::getCallbackCString(){ return callbackString.c_str(); }

int GUIEvent::getWidgetType()
{
  GUIRectangle *sourceRectangle = getEventSource();
  int           widgetType      = sourceRectangle->getWidgetType();
  return widgetType;
}

