#include "Game.h"


void Game::initVaribles()
{
	this->endGame = false;
	this->spawnTimerMax = 10.f;
	this->spawnTimer = this->spawnTimerMax;
	this->maxSwagBalls = 10;
	this->RedSwagBalls = 0;
	this->maxRedSwagBalls = 9;
}

void Game::initWindow()
{
	this->videoMode = sf::VideoMode(800, 600);
	this->window = new sf::RenderWindow(this->videoMode, "EAMAYC", sf::Style::Close | sf::Style::Titlebar);
	this->window->setFramerateLimit(60);
}

void Game::initFonts()
{
	if (!this->font.loadFromFile("Fonts/Dosis-Light.otf")) {
		std::cout << "ERROR::GAME::INITFONT::COULD NOT LOAD Dosis-Light.ttf" << "\n";
	}
}

void Game::initText()
{
	//Gui text init
	this->guiText.setFont(this->font);
	this->guiText.setFillColor(sf::Color::White);
	this->guiText.setCharacterSize(32);

	//End game text
	this->endGameText.setFont(this->font);
	this->endGameText.setFillColor(sf::Color::Red);
	this->endGameText.setOutlineColor(sf::Color::Yellow);
	this->endGameText.setOutlineThickness(8);
	this->endGameText.setCharacterSize(48);
	this->endGameText.setPosition(sf::Vector2f(250, 200));
	this->endGameText.setString("YOU ARE DEAD!");
}

//Constructors and Destructors
Game::Game()
{
	this->initVaribles();
	this->initWindow();
	this->initFonts();
	this->initText();
}

Game::~Game()
{
	delete this->window;
}

const bool& Game::getEndGame() const
{
	return this->endGame;
}

//Functions
const bool Game::running() const
{
	return this->window->isOpen();
}

void Game::pollEvents()
{
	while (this->window->pollEvent(this->sfmlEvent)) {
		switch (this->sfmlEvent.type) {
		case sf::Event::Closed:
			this->window->close();
			break;
		case sf::Event::KeyPressed:
			if (this->sfmlEvent.key.code == sf::Keyboard::Escape)
				this->window->close();
			break;
		}
	}
}

void Game::spawnSwagBalls()
{
	//Timer
	if (this->spawnTimer < this->spawnTimerMax)
		this->spawnTimer += 1.f;
	else {
		if (this->swagBalls.size() < this->maxSwagBalls) {
			this->swagBalls.push_back(SwagBall(*this->window, this->randBallType()));

			this->spawnTimer = 0.f;
		}
	}
}



const int Game::randBallType()
{
	int type = SwagBallTypes::DEFAULT;

	int randValue = rand() % 100 + 1;
	if (randValue > 60 && randValue <= 80) {
		if (this->RedSwagBalls < this->maxRedSwagBalls) {
			this->RedSwagBalls++;
			type = SwagBallTypes::DAMAGING;
		}
		else type = SwagBallTypes::DEFAULT;
	}
	else if (randValue > 90 && randValue <= 100)
		type = SwagBallTypes::HEALING;

	return type;
}

void Game::updatePlayer()
{
	this->player.update(this->window);

	if (this->player.getHp() <= 0)
		this->endGame = true;
}

void Game::updateCollision()
{
	//Check the collision
	for (size_t i = 0; i < this->swagBalls.size(); i++)
	{
		if (this->player.getShape().getGlobalBounds().intersects(this->swagBalls[i].getShape().getGlobalBounds())) {
			switch (this->swagBalls[i].getType()) {
			case SwagBallTypes::DEFAULT:
				this->points++;
				break;
			case SwagBallTypes::DAMAGING:
				this->player.takeDamage(1);
				RedSwagBalls--;
				break;
			case SwagBallTypes::HEALING:
				this->player.gainHealth(1);
				break;
			}

			//Remove the balls
			this->swagBalls.erase(this->swagBalls.begin() + i);
		}
	}
	
}

void Game::updateGui()
{
	std::stringstream ss;

	ss << "-Points: " << this->points << "\n" << "-Health: " << this->player.getHp() << "/" << this->player.getHpMax() << "\n";

	this->guiText.setString(ss.str());
}

void Game::update()
{
	this->pollEvents();

	if (this->endGame == false) {
		this->spawnSwagBalls();
		this->updatePlayer();
		this->updateCollision();
		this->updateGui();
	}
}

void Game::renderGui(sf::RenderTarget* target)
{
	target->draw(this->guiText);
}

void Game::render()
{
	this->window->clear();

	//Render stuff
	this->player.render(this->window);

	for (auto i : this->swagBalls){
		i.render(*this->window);
	}

	//Render gui
	this->renderGui(this->window);

	//Render end text
	if (this->endGame)
		this->window->draw(this->endGameText);

	this->window->display();
}