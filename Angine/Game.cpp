#include "Game.h"


Game::Game(HINSTANCE hInstance) : isRunning(true)
{
	this->kernel = new Kernel();
	this->window = new Window(hInstance);
    this->renderer = new Renderer(1920, 1080, true, this->window->getHWND(), true, 1.0f, .0f);
}


Game::~Game(void)
{
    delete this->renderer;
    delete this->window;
    delete this->kernel;
}


void Game::init()
{
	this->kernel->add(this->window);
	this->kernel->add(this->renderer);
}


bool Game::running()
{
	return this->isRunning;
}


void Game::run()
{
    this->processKeyboard();
	this->kernel->execute();

}


void Game::processKeyboard()
{
    std::set<int> keys = this->window->keys();

    for (std::set<int>::iterator key = keys.begin(); key != keys.end(); ++key) {
        switch (*key) {
        case 27:
            this->quit();
        default:
            std::wstringstream s;
            s << "Key " << (*key) << " pressed. Buffer size: " << keys.size() << std::endl;

            OutputDebugString(s.str().c_str());
        }

    }
}


void Game::quit()
{
    this->isRunning = false;

    OutputDebugString(L"Quit.\n");
}