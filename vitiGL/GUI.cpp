#include "GUI.hpp"
#include "Keytable.hpp"

namespace vitiGL {

CEGUI::OpenGL3Renderer* GUI::_renderer = nullptr;

GUI::GUI(const std::string& resourceDirectory, const std::string& schemePath)
	:	_context	{ nullptr },
		_root		{ nullptr },
		_resourceDir{ resourceDirectory }
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

	setScheme(schemePath);
}

GUI::~GUI(){
	CEGUI::System::getSingleton().destroyGUIContext(*_context);
}

void GUI::update(unsigned int deltaTime) {
	_context->injectTimePulse((float)deltaTime / 1000.0f);
}

void GUI::draw() {
	glDisable(GL_DEPTH_TEST); //no text otherwise
	_renderer->beginRendering();
	_context->draw();
	_renderer->endRendering();
	glEnable(GL_DEPTH_TEST);
}

CEGUI::Window* GUI::createConsole(const std::string & layoutFile) {
	CEGUI::Window* window = CEGUI::WindowManager::getSingleton().loadLayoutFromFile(_resourceDir + "/" + layoutFile, "console");
	_root->addChild(window);
	return window;
}

void GUI::onSDLEvent(SDL_Event & event) {
	CEGUI::utf32 codePoint;

	switch (event.type) {
	case SDL_MOUSEMOTION:
		_context->injectMousePosition(event.motion.x, event.motion.y);
		break;
	case SDL_KEYDOWN:
		_context->injectKeyDown(SDLToCEGUI(event.key.keysym.sym));
		break;
	case SDL_KEYUP:
		_context->injectKeyUp(SDLToCEGUI(event.key.keysym.sym));
		break;
	case SDL_TEXTINPUT:
		/*	NOT CORRECT
			We need to decode utf8 (SDL) and convert to utf-32
			see http://utfcpp.sourceforge.net/ for a pre-built converter
		*/
		codePoint = 0;
		for (int i = 0; i < event.text.text[i] != '\0'; i++) {
			codePoint |= (CEGUI::utf32)event.text.text[i] << (i * 8); //bitshift
		}
		_context->injectChar(codePoint);
		break;
	case SDL_MOUSEBUTTONDOWN:
		_context->injectMouseButtonDown(SDLToCEGUI_Button(event.button.button));
		break;
	case SDL_MOUSEBUTTONUP:
		_context->injectMouseButtonUp(SDLToCEGUI_Button(event.button.button));
		break;
	}
}

void GUI::setMouseCursor(const std::string& imagePath) {
	_context->getMouseCursor().setDefaultImage(imagePath); //can crash!
}

void GUI::mouseOn() {
	_context->getMouseCursor().show();
}

void GUI::mouseOff() {
	_context->getMouseCursor().hide();
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