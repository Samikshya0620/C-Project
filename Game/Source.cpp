#include "SFML-2.5.1\include\SFML\Graphics.hpp"
#include "SFML-2.5.1\include\SFML\Audio.hpp"


#include<vector>

#include "Hero.h"
#include "Enemy.h"
#include "Rocket.h"



//SFML has a Vector2f data type which is named viewSize to define 
	//the size of the window
sf::Vector2f viewSize(1024, 768);
//viewMode set the width and height of the window 
//it also obtains different resolution that the monitor supports
//viewMode is made Videomode
sf::VideoMode vm(viewSize.x, viewSize.y);
//creating a window using Renderwindow class of SFML
//Default style combines the ability to resize the window close it and add title bar
sf::RenderWindow window(vm, "First game ", sf::Style::Default);


void spawnEnemy();
void shoot();


bool checkCollision(sf::Sprite sprite1, sf::Sprite sprite2);

void reset();

//Variables
sf::Texture skyTexture, bgTexture;

sf::Sprite skySprite, bgSprite;

Hero hero;

std::vector <Enemy*> enemies;
std::vector <Rocket*> rockets;

float currentTime;
float prevTime = 0.0f;

int score = 0;
bool gameover = true;

//Text
sf::Font headingFont;
sf::Font scoreFont;

sf::Text headingText;
sf::Text scoreText;
sf::Text tutorialText;

//Audio

sf::Music bgMusic;

sf::SoundBuffer fireBuffer;
sf::SoundBuffer hitBuffer;

sf::Sound fireSound(fireBuffer);
sf::Sound hitSound(hitBuffer);

/**
sf::Vector2f playerPosition;
bool playerMoving = false;
*/

void init() {
	//load sky Texture
	skyTexture.loadFromFile("Assets/graphics/sky.png");
	//set and attach a Texture to Sprite
	skySprite.setTexture(skyTexture);

	bgTexture.loadFromFile("Assets/graphics/bg.png");
	bgSprite.setTexture(bgTexture);

	//Load Font

	headingFont.loadFromFile("Assets/fonts/SnackerComic.ttf");
	scoreFont.loadFromFile("Assets/fonts/arial.ttf");

	//Set Heading Text
	headingText.setFont(headingFont);
	headingText.setString("MINI BAZOOKA");
	headingText.setCharacterSize(84);
	headingText.setFillColor(sf::Color::Red);

	sf::FloatRect headingbounds = headingText.getLocalBounds();
	headingText.setOrigin(headingbounds.width / 2, headingbounds.height / 2);
	headingText.setPosition(sf::Vector2f(viewSize.x * 0.5f, viewSize.y * 0.10f));
	
	//Set Score Text

	scoreText.setFont(scoreFont);
	scoreText.setString("Score: 0");
	scoreText.setCharacterSize(45);
	scoreText.setFillColor(sf::Color::Red);
	
	sf::FloatRect scorebounds = scoreText.getLocalBounds();
	scoreText.setOrigin(scorebounds.width / 2, scorebounds.height /
		2);
	scoreText.setPosition(sf::Vector2f(viewSize.x * 0.5f, viewSize.y * 0.10f));
	
	//Tutorial Text

	tutorialText.setFont(scoreFont);
	tutorialText.setString("Press Space to Fire and Start Game ,Up Arrow to Jump");
	tutorialText.setCharacterSize(35);
	tutorialText.setFillColor(sf::Color::Red);

	sf::FloatRect tutorialbounds = tutorialText.getLocalBounds();
	tutorialText.setOrigin(tutorialbounds.width / 2, tutorialbounds.height / 2);
	tutorialText.setPosition(sf::Vector2f(viewSize.x * 0.5f, viewSize.y * 0.20f));
	
	//Audio

	bgMusic.openFromFile("Assets/audio/bgMusic.ogg");
	bgMusic.play();

	hitBuffer.loadFromFile("Assets/audio/hit.ogg");
	fireBuffer.loadFromFile("Assets/audio/fire.ogg");

    hero.init("Assets/graphics/heroAnim.png",4,1.0f, sf::Vector2f(viewSize.x * 0.25f, viewSize.y * 0.50f), 200);
	
	srand((int)time(0));

}

void updateInput() {
	sf::Event event;

	//While there are pending events...
	while (window.pollEvent(event)) {

		if (event.type == sf::Event::KeyPressed) {

			if (event.key.code == sf::Keyboard::Up) {

				hero.jump(900.0f);

			}

			if (event.key.code == sf::Keyboard::Space) {

				if (gameover) {

					gameover = false;
					reset();
				}
				else
				shoot();

			}
		}
		if (event.key.code == sf::Keyboard::Escape || event.type == sf::Event::Closed)
			window.close();
	}
}
void update(float dt){

	hero.update(dt);

	currentTime += dt;

	//Spawn Enemies

	if (currentTime >= prevTime + 1.125f) {

		spawnEnemy();

		prevTime = currentTime;
	}

	//update Enemies

	for (int i = 0; i < enemies.size(); i++) {

		Enemy* enemy = enemies[i];

		enemy->update(dt);

		if (enemy->getSprite().getPosition().x < 0) {

			enemies.erase(enemies.begin() + i);
			delete(enemy);

			gameover = true;
		}
	}
	//update Rockets

	for (int i = 0; i < rockets.size(); i++)
	{

		Rocket* rocket = rockets[i];
		
		rocket->update(dt);
		if (rocket->getSprite().getPosition().x > viewSize.x) {
			
			rockets.erase(rockets.begin() + i);
			delete rocket;
		}
	}
	
	//Check Collision between Rocket and Enemies

	for (int i = 0; i < rockets.size(); i++) {
		for (int j = 0; j < enemies.size(); j++) {

			Rocket* rocket = rockets[i];
			Enemy* enemy = enemies[j];

			if (checkCollision(rocket->getSprite(), enemy->getSprite())) {

				hitSound.play();

				score++;

				std::string finalScore = "Score: " + std::to_string(score);

				scoreText.setString(finalScore);

				sf::FloatRect scorebounds = scoreText.getLocalBounds();
				scoreText.setOrigin(scorebounds.width / 2, scorebounds.height /
					2);
				scoreText.setPosition(sf::Vector2f(viewSize.x * 0.5f, viewSize.y * 0.10f));
				
				rockets.erase(rockets.begin() + i);
				enemies.erase(enemies.begin() + j);
				
				delete(rocket);
				delete(enemy);
				
				std::cout << "Rocket intersects enemy" << std::endl;
			}
		}
	}
}
void draw() {

	window.draw(skySprite);
	window.draw(bgSprite);

	window.draw(hero.getSprite());
	for (Enemy* enemy : enemies) {

		window.draw(enemy->getSprite());
	}

	for (Rocket* rocket : rockets) {

		window.draw(rocket->getSprite());

	}
	if (gameover) {

		window.draw(headingText);
		window.draw(tutorialText);
	}
	else {
		window.draw(scoreText);
	}
}

int main() {

	sf::Clock clock;

	//window.setFramerateLimit(60);

	init();

	while (window.isOpen()) {
		//update input/events(Keyboard)
		updateInput();
		//update game HERE
		sf::Time dt = clock.restart();
		if (!gameover) {
			update(dt.asSeconds());
		}

		// Draw Game HERE
		window.clear(sf::Color::Red);
		draw();
		
		//Show everything we just drew
		window.display();

	}
	
	return 0;
}
void spawnEnemy() {

	
	
	sf::Vector2f enemyPos;
	
	float speed;
	if (score < 20) {
		int randLoc = rand() % 3;//generates 0,1 or 2
		switch (randLoc)
		{
		case 0:
			enemyPos = sf::Vector2f(viewSize.x, viewSize.y * 0.75f);
			speed = -400;
			break;
		case 1:
			enemyPos = sf::Vector2f(viewSize.x, viewSize.y * 0.60f);
			speed = -550;
			break;
		case 2:
			enemyPos = sf::Vector2f(viewSize.x, viewSize.y * 0.40f);
			speed = -650;
			break;
		default:
			std::cout << "incorrect y value \n";
			return;
		}
	}
	else {
		int randLoc = rand() % 4;
		switch (randLoc)
		{
		case 0:
			enemyPos = sf::Vector2f(viewSize.x, viewSize.y * 0.75f);speed = -1000; break;	
		case 1:
			enemyPos = sf::Vector2f(viewSize.x, viewSize.y * 0.60f); speed = -800; break;	
		case 2:
			enemyPos = sf::Vector2f(viewSize.x, viewSize.y * 0.40f); speed = -600; break;
		case 3:
			enemyPos = sf::Vector2f(viewSize.x, viewSize.y * 0.20f); speed = -400; break;
		default:
			std::cout << "incorrect y value \n";
			return;
		}
	}
	Enemy* enemy = new Enemy();
	enemy->init("Assets/graphics/enemy.png", enemyPos, speed);
	enemies.push_back(enemy);
}
void shoot() {

	Rocket* rocket = new Rocket();

	rocket->init("Assets/graphics/rocket.png", hero.getSprite().getPosition(), 400.f);
	
	rockets.push_back(rocket);
	
	fireSound.play();
}
bool checkCollision(sf::Sprite sprite1, sf::Sprite sprite2) {

	sf::FloatRect shape1 = sprite1.getGlobalBounds();
	sf::FloatRect shape2 = sprite2.getGlobalBounds();
	
	if (shape1.intersects(shape2)) {

		return true;

	}
	else {

		return false;

	}
}
void reset() {
	score = 0;
	currentTime = 0.0f;
	prevTime = 0.0;

	scoreText.setString("Score: 0");

	for (Enemy* enemy : enemies) {

		delete(enemy);

	}
	for (Rocket* rocket : rockets) {

		delete(rocket);

	}

	enemies.clear();
	rockets.clear();
}
