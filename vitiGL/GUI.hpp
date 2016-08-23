#pragma once

#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/OpenGL/GL3Renderer.h>

#include <glm/glm.hpp>
#include <SDL2/SDL_events.h>

namespace vitiGL {

class GUI {
public:
	 GUI		(const std::string& resourceDirectory);
	~GUI		();

	void		update(unsigned int deltaTime);
	void		draw();

	void		onSDLEvent(SDL_Event& event);

	void		setMouseCursor(const std::string& imagePath);
	void		mouseOn();
	void		mouseOff();

	void		setScheme(const std::string& schemeFile);
	void		setFont(const std::string& fontFile);

	CEGUI::Window* createWidget(const glm::vec4& destRectPercent, const glm::vec4& destRectPixel,
								const std::string& scheme, const std::string& name = "");

	void setWidgetDestRect(CEGUI::Window* widget, const glm::vec4& destRectPercent, const glm::vec4& destRectPixel);

	static CEGUI::OpenGL3Renderer* renderer()	{ return _renderer; }
	CEGUI::GUIContext* context() const			{ return _context; }

private:
	static CEGUI::OpenGL3Renderer* _renderer;	//we only want one renderer
	CEGUI::GUIContext*	_context;
	CEGUI::Window*		_root;					//every widget is a child of root
};
}
