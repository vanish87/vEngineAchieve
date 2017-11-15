#include "Common/Header/Configure.h"
#include "Common/Header/CommonPreDec.h"
#include "TinyXML2\tinyxml2.h"

using namespace tinyxml2;

namespace vEngine
{	
	Configure::Configure()
	{
		this->LoadConfig("Configure/DefaultConfig.xml");
	}

	Configure::~Configure()
	{

	}

	void Configure::LoadConfig(std::string const & cfg_file)
	{
		XMLDocument doc;

		if (doc.LoadFile(cfg_file.c_str()) != XMLError::XML_SUCCESS)
		{
			PRINT_AND_BREAK("FAIL to load " + cfg_file);
			return;
		}

		XMLElement* configure = doc.FirstChildElement("configure");
		XMLElement* context = configure->FirstChildElement("context");

		std::string render_factory_name = context->FirstChildElement("render_factory")->Attribute("name");

		XMLElement* render = configure->FirstChildElement("render");
		XMLElement* frame = render->FirstChildElement("frame");

		int width = frame->IntAttribute("width");
		int height = frame->IntAttribute("height");
		bool full_screen = frame->BoolAttribute("fullscreen");

		bool msaa4x = render->FirstChildElement("msaa4x")->BoolAttribute("value");
		bool gamma  = render->FirstChildElement("gamma")->BoolAttribute("value");
		bool deferred_rendering = render->FirstChildElement("deferred_rendering")->BoolAttribute("value");

		if (deferred_rendering)
		{
			XMLElement* gbuffer_size_ptr = render->FirstChildElement("deferred_rendering")->FirstChildElement("gbuffer_size");
			int gbuffer_size = gbuffer_size_ptr->IntAttribute("value");
			context_setting_.render_setting.gbuffer_size = gbuffer_size;
		}

		context_setting_.render_factory_name = render_factory_name;
		context_setting_.render_setting.width = width;
		context_setting_.render_setting.height = height;
		context_setting_.render_setting.left = 0;
		context_setting_.render_setting.top = 0;
		context_setting_.render_setting.full_screen = full_screen;
		context_setting_.render_setting.msaa4x = msaa4x;
		context_setting_.render_setting.gamma = gamma;
		context_setting_.render_setting.deferred_rendering = deferred_rendering;
	}

	Configure::ContextSetting& Configure::GetContextSetting()
	{
		return this->context_setting_;
	}

}