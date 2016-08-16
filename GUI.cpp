#include "GUI.hpp"

namespace vitiGL {

CEGUI::OpenGL3Renderer* GUI::_renderer = nullptr;

GUI::GUI(const std::string& resourceDirectory) 
	:	_context	{ nullptr },
		_root		{ nullptr }
{
	/* only initialize CEGUI once: */
	if (_renderer == nullptr) {
		_renderer = &CEGUI::OpenGL3Renderer::bootstrapSystem();

		CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>
			(CEGUI::System::getSingleton().getResourceProvider());

		rp->setResourceGroupDirectory("imagesets", resourceDirectory + "/imagesets/");
		rp->setResourceGroupDirectory("schemes", resourceDirectory + "/schemes/");
		rp->setResourceGroupDirectory("fonts", resourceDirectory + "/fonts/");
		rp->setResourceGroupDirectory("layouts", resourceDirectory + "/layouts/");
		rp->setResourceGroupDirectory("looknfeels", resourceDirectory + "/looknfeel/");
		rp->setResourceGroupDirectory("lua_scripts", resourceDirectory + "/lua_scripts/");

		CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
		CEGUI::Scheme::setDefaultResourceGroup("schemes");
		CEGUI::Font::setDefaultResourceGroup("fonts");
		CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
		CEGUI::WindowManager::setDefaultResourceGroup("layouts");
		CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
	}

	_context = &CEGUI::System::getSingleton().createGUIContext(_renderer->getDefaultRenderTarget());
	_root = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "root");
	_context->setRootWindow(_root);
}

GUI::~GUI(){
	CEGUI::System::getSingleton().destroyGUIContext(*_context);
}

void GUI::draw() {
	glDisable(GL_DEPTH_TEST); //no text otherwise
	_renderer->beginRendering();
	_context->draw();
	_renderer->endRendering();
	glEnable(GL_DEPTH_TEST);
}

void GUI::setScheme(const std::string & schemeFile) {
	CEGUI::SchemeManager::getSingleton().createFromFile(schemeFile);
}

void GUI::setFont(const std::string & fontFile) {
	CEGUI::FontManager::getSingleton().createFromFile(fontFile + ".font");
	_context->setDefaultFont(fontFile);
}

CEGUI::Window * GUI::createWidget(const glm::vec4& destRectPercent, const glm::vec4& destRectPixel, 
	const std::string & scheme, const std::string & name) 
{
	/* create widget and add it to root:*/
	CEGUI::Window* window = CEGUI::WindowManager::getSingleton().createWindow(scheme, name);
	_root->addChild(window);

	/* set position and dimensions: */
	setWidgetDestRect(window, destRectPercent, destRectPixel);

	return window;
}

void GUI::setWidgetDestRect(CEGUI::Window* widget, const glm::vec4 & destRectPercent, const glm::vec4 & destRectPixel) {
	widget->setPosition(CEGUI::UVector2{ CEGUI::UDim{ destRectPercent.x, destRectPixel.x }, CEGUI::UDim{ destRectPercent.y, destRectPixel.y } });
	widget->setSize(CEGUI::USize{ CEGUI::UDim{ destRectPercent.z, destRectPixel.z }, CEGUI::UDim{ destRectPercent.w, destRectPixel.w } });
}

}