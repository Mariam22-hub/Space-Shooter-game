#include<iostream>
#include"SFML\Graphics.hpp"
#include"SFML\Window.hpp"
#include"SFML\System.hpp"
#include<cstdlib>
#include <map>
#include<vector>
#include <string>
#include <sstream>

using namespace sf;


class Enemy{

private:

	unsigned pointCount;
	sf::CircleShape shape;
	int type;
	float speed;
	int hp;
	int hpMax;
	int damage;
	int points;

	Sprite sprite;
	Texture texture;

    void initVariables()
    {
        this->pointCount = rand() % 8 + 3; //min = 3 max = 10
        this->type		= 0;
        this->speed		= static_cast<float>(this->pointCount/3);
        this->hpMax		= static_cast<int>(this->pointCount);
        this->hp		= this->hpMax;
        this->damage	= this->pointCount;
        this->points	= this->pointCount;
    }

    void initShape()
    {
        this->shape.setRadius(this->pointCount * 5);
        this->shape.setPointCount(this->pointCount);
        this->shape.setFillColor(sf::Color(rand() % 255 + 1, rand() % 255 + 1, rand() % 255 + 1, 255));
    }

    void initTexture()
    {
	//Load a texture from file
        if (!this->texture.loadFromFile("enemy.png"))
        {
            std::cout << "ERROR::PLAYER::INITTEXTURE::Could not load texture file." << "\n";
        }
    }

public:

    Enemy(Texture *texture, float pos_x, float pos_y)
    {
        sprite.setTexture(*texture);

        this->initVariables();
        //this->initShape();

        this->sprite.setPosition(pos_x, pos_y);
        this->sprite.setScale(0.2f,0.2f);
    }

    ~Enemy()
    {

    }

//Accessors
    const sf::FloatRect getBounds() const
    {
        return this->sprite.getGlobalBounds();
    }

    const int & getPoints() const
    {
        return this->points;
    }

    const int & getDamage() const
    {
        return this->damage;
    }

//Functions
    void update()
    {
        this->sprite.move(0.f, this->speed);
    }

    void render(sf::RenderTarget * target)
    {
        target->draw(this->sprite);
    }
};

//----------------------------------------------------------------------------------------------

class Bullet{
private:

	Sprite shape;

	Vector2f direction;
	float movementSpeed;

public:

    	//animation dimensions


        Bullet()
    {

    }

    Bullet(Texture *texture, float pos_x, float pos_y, float dir_x, float dir_y, float movement_speed)
    {


        shape.setTexture(*texture);

        this->shape.setPosition(pos_x, pos_y);
        this->direction.x = dir_x;
        this->direction.y = dir_y;
        this->movementSpeed = movement_speed;
    }

    Sprite get_sprite(){
        return this->shape;
    }

    ~Bullet()
    {

    }

    const sf::FloatRect getBounds() const
    {
        return this->shape.getGlobalBounds();
    }

    void update()
    {
	//Movement
        this->shape.move(this->movementSpeed * this->direction);
    }

    void render(sf::RenderTarget * target)
    {
        target->draw(this->shape);
    }

};

//------------------------------------------------------------------------------------------------------------

class Player{

private:
	sf::Sprite sprite;
	sf::Texture texture;

	float movementSpeed;

	float attackCooldown;
	float attackCooldownMax;

	int hp;
	int hpMax;

	void initVariables()
    {
        this->movementSpeed = 4.f;

        this->attackCooldownMax = 10.f;
        this->attackCooldown = this->attackCooldownMax;

        this->hpMax = 100;
        this->hp = this->hpMax;
    }

    void initTexture()
    {
	//Load a texture from file
        if (!this->texture.loadFromFile("ship.png"))
        {
            std::cout << "ERROR::PLAYER::INITTEXTURE::Could not load texture file." << "\n";
        }
    }

    void initSprite()
    {
	//Set the texture to the sprite
        this->sprite.setTexture(this->texture);

	//Resize the sprite
        this->sprite.scale(0.1f, 0.1f);
    }

public:

    Player()
    {
        this->initVariables();
        this->initTexture();
        this->initSprite();

        sprite.setPosition(300.f,400.f);
    }

    ~Player()
    {

    }


    const sf::Vector2f & getPos() const
    {
        return this->sprite.getPosition();
    }


    const FloatRect getBounds() const
    {
        return this->sprite.getGlobalBounds();
    }


    const int & getHp() const
    {
        return this->hp;
    }


    const int & getHpMax() const
    {
        return this->hpMax;
    }


    void setPosition(const Vector2f pos)
    {
        this->sprite.setPosition(pos);
    }


    void setPosition(const float x, const float y)
    {
        this->sprite.setPosition(x, y);
    }

    void setHp(const int hp)
    {
        this->hp = hp;
    }

    void loseHp(const int value)
    {
        this->hp -= value;

        if (this->hp < 0)
            this->hp = 0;
    }


    void move(const float dirX, const float dirY)
    {
        this->sprite.move(this->movementSpeed * dirX, this->movementSpeed * dirY);
    }


    const bool canAttack()
    {
        if (this->attackCooldown >= this->attackCooldownMax)
        {
            this->attackCooldown = 0.f;
            return true;
        }

        return false;
    }


    void updateAttack()
    {
        if(this->attackCooldown < this->attackCooldownMax)
            this->attackCooldown += 0.5f;
    }

//Functions

    void update()
    {
        this->updateAttack();
    }


    void render(sf::RenderTarget& target)
    {
        target.draw(this->sprite);
    }

};


//////---------------------------------------------------------------------------------

class Game{
private:

    sf::RenderWindow* window;

	//Resources
	std::map<std::string, sf::Texture*> textures;

	std::vector<Bullet*> bullets;
	std::vector<Enemy*> enemies;

	//GUI
	Font font;
	Text pointText;

	Text gameOverText;

	//World
	Sprite worldBackground;
	Texture worldBackgroundTex;

	//Systems
	unsigned points;

	//Player
	Player* player;

	//PlayerGUI
	RectangleShape playerHpBar;
	RectangleShape playerHpBarBack;


 	//Enemies
	float spawnTimer;
    float spawnTimerMax;


	void initWindow()
    {
        this->window = new sf::RenderWindow(sf::VideoMode(800, 600), "Swaglords of Space - Game 3", sf::Style::Close | Style::Titlebar);
        this->window->setFramerateLimit(144);
        this->window->setVerticalSyncEnabled(false);
    }



    void initTextures()
    {
//        this->textures["BULLET"] = new Texture();
//        this->textures["BULLET"]->loadFromFile("bullet.png");

        this->textures["BULLET"] = new Texture();
        this->textures["BULLET"]->loadFromFile("bullet.png");


        this->textures["ENEMY"] = new Texture();
        this->textures["ENEMY"]->loadFromFile("enemy.png");
    }


    void initGUI()
    {
	//Load font
        if (!this->font.loadFromFile("PixellettersFull.ttf"))
            std::cout << "ERROR::GAME::Failed to load font" << "\n";

	//Init point text
        this->pointText.setPosition(700.f, 25.f);
        this->pointText.setFont(this->font);
        this->pointText.setCharacterSize(20);
        this->pointText.setFillColor(sf::Color::White);
        this->pointText.setString("test");

        this->gameOverText.setFont(this->font);
        this->gameOverText.setCharacterSize(60);
        this->gameOverText.setFillColor(sf::Color::Red);
        this->gameOverText.setString("Game Over!");
        this->gameOverText.setPosition(
            this->window->getSize().x / 2.f - this->gameOverText.getGlobalBounds().width / 2.f,
            this->window->getSize().y / 2.f - this->gameOverText.getGlobalBounds().height / 2.f);

	//Init player GUI
        this->playerHpBar.setSize(sf::Vector2f(300.f, 25.f));
        this->playerHpBar.setFillColor(sf::Color::Red);
        this->playerHpBar.setPosition(sf::Vector2f(20.f, 20.f));

        this->playerHpBarBack = this->playerHpBar;
        this->playerHpBarBack.setFillColor(sf::Color(25, 25, 25, 200));
    }


    void initWorld()
    {
        if (!this->worldBackgroundTex.loadFromFile("background.png"))
        {
            std::cout << "ERROR::GAME::COULD NOT LOAD BACKGROUND TEXTURE" << "\n";
        }

        this->worldBackground.setTexture(this->worldBackgroundTex);
    }


    void initSystems()
    {
        this->points = 0;
    }


    void initPlayer()
    {
        this->player = new Player();
    }


    void initEnemies()
    {
        this->spawnTimerMax = 50.f;
        this->spawnTimer = this->spawnTimerMax;
    }


public:

        Game()
    {
        this->initWindow();
        this->initTextures();
        this->initGUI();
        this->initWorld();
        this->initSystems();

        this->initPlayer();
        this->initEnemies();
    }

    ~Game()
    {
        delete this->window;
        delete this->player;

	//Delete textures
        for (auto &i : this->textures)
        {
            delete i.second;
        }

	//Delete bullets
        for (auto *i : this->bullets)
        {
            delete i;
        }

	//Delete enemies
        for (auto *i : this->enemies)
        {
            delete i;
        }
    }

//Functions
    void run()
    {
        while (this->window->isOpen())
        {
            this->updatePollEvents();

            if(this->player->getHp() > 0)
                this->update();

            this->render();
        }
    }


    void updatePollEvents()
    {
        sf::Event e;
        while (this->window->pollEvent(e))
        {
            if (e.Event::type == sf::Event::Closed)
                this->window->close();
            if (e.Event::KeyPressed && e.Event::key.code == sf::Keyboard::Escape)
                this->window->close();
        }
    }


    void updateInput()
    {
	//Move player
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            this->player->move(-1.f, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            this->player->move(1.f, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            this->player->move(0.f, -1.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            this->player->move(0.f, 1.f);

        if (Keyboard::isKeyPressed(Keyboard::Space) && this->player->canAttack())
        {
            this->bullets.push_back(new Bullet(this->textures["BULLET"], this->player->getPos().x + this->player->getBounds().width/2.f, this->player->getPos().y, 0.f, -1.f, 5.f));

        }
    }


    void updateGUI()
    {
        std::stringstream ss;

        ss << "Points: " << this->points;

        this->pointText.setString(ss.str());

	//Update player GUI
        float hpPercent = static_cast<float>(this->player->getHp()) / this->player->getHpMax();
        this->playerHpBar.setSize(Vector2f(300.f * hpPercent, this->playerHpBar.getSize().y));
    }


    void updateWorld()
    {

    }


    void updateCollision()
    {
	//Left world collision
        if (this->player->getBounds().left < 0.f)
        {
            this->player->setPosition(0.f, this->player->getBounds().top);
        }
	//Right world collison
        else if (this->player->getBounds().left + this->player->getBounds().width >= this->window->getSize().x)
        {
            this->player->setPosition(this->window->getSize().x - this->player->getBounds().width, this->player->getBounds().top);
        }

	//Top world collision
        if (this->player->getBounds().top < 0.f)
        {
            this->player->setPosition(this->player->getBounds().left, 0.f);
        }
	//Bottom world collision
        else if (this->player->getBounds().top + this->player->getBounds().height >= this->window->getSize().y)
        {
            this->player->setPosition(this->player->getBounds().left, this->window->getSize().y - this->player->getBounds().height);
        }
    }


    void updateBullets()
    {
        unsigned counter = 0;
        for (auto *bullet : this->bullets)
        {
            bullet->update();

		//Bullet culling (top of screen)
            if (bullet->getBounds().top + bullet->getBounds().height < 0.f)
            {
			//Delete bullet
                delete this->bullets.at(counter);
                this->bullets.erase(this->bullets.begin() + counter);
            }

            ++counter;
        }
    }


    void updateEnemies()
    {
	//Spawning
        this->spawnTimer += 0.5f;
        if (this->spawnTimer >= this->spawnTimerMax)
        {
            this->enemies.push_back(new Enemy(this->textures["ENEMY"], rand() % this->window->getSize().x-70.f, -100.f));
            this->spawnTimer = 0.f;
        }

	//Update
        unsigned counter = 0;
        for (auto *enemy : this->enemies)
        {
            enemy->update();

		//Bullet culling (top of screen)
            if (enemy->getBounds().top > this->window->getSize().y)
            {
			//Delete enemy
                delete this->enemies.at(counter);
                this->enemies.erase(this->enemies.begin() + counter);
            }

		//Enemy player collision
            else if(enemy->getBounds().intersects(this->player->getBounds()))
            {
                this->player->loseHp(this->enemies.at(counter)->getDamage());
                delete this->enemies.at(counter);
                this->enemies.erase(this->enemies.begin() + counter);
            }

            ++counter;
        }
    }


    void updateCombat()
    {
        for (int i = 0; i < this->enemies.size(); ++i)
        {
            bool enemy_deleted = false;
            for (size_t k = 0; k < this->bullets.size() && enemy_deleted == false; k++)
            {
                if (this->enemies[i]->getBounds().intersects(this->bullets[k]->getBounds()))
                {
                    this->points += this->enemies[i]->getPoints();

                    delete this->enemies[i];
                    this->enemies.erase(this->enemies.begin() + i);

                    delete this->bullets[k];
                    this->bullets.erase(this->bullets.begin() + k);

                    enemy_deleted = true;
                }
            }
        }
    }


    void update()
    {
        this->updateInput();

        this->player->update();

        this->updateCollision();

        this->updateBullets();

        this->updateEnemies();

        this->updateCombat();

        this->updateGUI();

        this->updateWorld();
    }


    void renderGUI()
    {
        this->window->draw(this->pointText);
        this->window->draw(this->playerHpBarBack);
        this->window->draw(this->playerHpBar);
    }


    void renderWorld()
    {
        this->window->draw(this->worldBackground);
    }


    void render()
    {
        this->window->clear();

	//Draw world
        this->renderWorld();

	//Draw all the stuffs
        this->player->render(*this->window);

        for (auto *bullet : this->bullets)
        {
            bullet->render(this->window);
        }

        for (auto *enemy : this->enemies)
        {
            enemy->render(this->window);
        }

        this->renderGUI();

	//Game over screen
        if (this->player->getHp() <= 0)
            this->window->draw(this->gameOverText);

        this->window->display();
    }

};



int main()
{
	srand(static_cast<unsigned>(0));

	Game game;

    game.run();
//	while (game.getwindow().isOpen())
//	{
//		game.update();
//		game.render();
//	}

	//End of application
	return 0;
}


//class Enemy{
//
//    CircleShape shape;
//
//    int type;
//    float speed;
//    int hp;
//    int hpMax;
//    int damage;
//    int points;
//    unsigned pointCount;
//
//    Sprite sprite;
//
//    void initVariables(){
//
//        this->pointCount = rand() % 8 + 3; //min = 3 max = 10
//        this->type		= 0;
//        this->speed		= static_cast<float>(this->pointCount/3);
//        this->hpMax		= static_cast<int>(this->pointCount);
//        this->hp		= this->hpMax;
//        this->damage	= this->pointCount;
//        this->points	= this->pointCount;
//        this->speed = 5.f;
//        this->points = 5;
//    }
//
//
//    void initShape()
//    {
//        this->shape.setRadius(this->pointCount * 5);
//        this->shape.setPointCount(this->pointCount);
//        this->shape.setFillColor(Color(rand() % 255 + 1, rand() % 255 + 1, rand() % 255 + 1, 255));
//    }
//
//
//public:
//
//    Enemy(){}
//
//    //enemy constructor
////    Enemy(Texture *texture, float pos_x, float pos_y){
////        this->shape.setPosition(pos_x, pos_y);
////
////        this->initShape();
////        this->initVariables();
////
////        sprite.setTexture(*texture);
////        sprite.setScale(5.f,5.f);
////        sprite.setPosition(pos_x, pos_y);
////    }
//
//
//    Enemy(float pos_x, float pos_y){
//        this->shape.setPosition(pos_x, pos_y);
//
//        this->initShape();
//        this->initVariables();
//    }
//
//    const FloatRect getBounds() const{
//        return this->shape.getGlobalBounds();
//    }
//
//
//    void update(){
//        shape.move(0.f, speed);
//    }
//
//    void render(RenderTarget &target){
//        target.draw(this->shape);
//    }
//
//    ~Enemy(){}
//
//};
//
//
//
//
////-----------------------------------------------------------------------------------------------------------------
//
//class Bullet
//{
//private:
//
//	Sprite shape;
//    //Texture *texture;
//
//	Vector2f direction;
//	float movementSpeed;
//
//public:
//
//	Bullet(){};
//
//
//	//constructor bullets
//	Bullet(Texture *texture, float pos_x, float pos_y, float dir_x, float dir_y, float movement_speed){
//
//        shape.setTexture(*texture);
//        shape.setPosition(pos_x,pos_y);
//
//        direction.x = dir_x;
//        direction.y = dir_y;
//
//        movementSpeed = movement_speed;
//	}
//
//
//	//bullet bounds getter
//	const FloatRect getBounds() const{
//        return this->shape.getGlobalBounds();
//    }
//
//
//	//update movement bullets
//	void update(){
//        shape.move(this->movementSpeed * this->direction);
//	}
//
//	//render bullets
//	void render(RenderTarget &target){
//        target.draw(shape);
//	}
//
//    //destructor bullets
//    ~Bullet(){};
//};
//
//
////--------------------------------------------------------------------------------------------------------
//
//class Player{
//
//    Sprite sprite;
//    Texture texture;
//
//    float movement_speed;
//    float attackCooldown;
//    float attackCooldownMax;
//
//    //loading texture and images
//    void inittexture(){
//
//        if (!texture.loadFromFile("ship.png")){
//            std::cout << "error in player class - inittexture function; couldn't load file \n";
//        }
//    }
//
//    //initiate sprite and texture
//    void initsprite(){
//
//        //set texture player
//        sprite.setTexture(texture);
//
//        //set size player
//        sprite.scale(0.1f, 0.1f);
//    }
//
//
//    void initVariables(){
//        this->movement_speed = 5.f;
//
//        this->attackCooldownMax = 10.f;
//        this->attackCooldown = this->attackCooldownMax;
//
////        this->hpMax = 100;
////        this->hp = this->hpMax;
//    }
//
//
//
//public:
//
//    //constructor player
//    Player(){
//
//        initVariables();
//        inittexture();
//        initsprite();
//    }
//
//    //player position getters
//
//    void setPosition(const Vector2f pos){
//        this->sprite.setPosition(pos);
//    }
//
//    void setPosition(const float x, const float y){
//        this->sprite.setPosition(x, y);
//    }
//
//    const Vector2f &getPos() const {
//        return this->sprite.getPosition();
//    }
//
//    const FloatRect getBounds() const {
//        return this->sprite.getGlobalBounds();
//    }
//
//
//    //update moving player
//    void movement(const float dirx, const float diry){
//        sprite.move(movement_speed * dirx, movement_speed * diry);
//    }
//
//
//    //update cool down
//    void updateAttack(){
//        if(attackCooldown < attackCooldownMax)
//            this->attackCooldown += 0.5f;
//    }
//
//
//    const bool canAttack(){
//
//        if (this->attackCooldown >= this->attackCooldownMax)
//        {
//            this->attackCooldown = 0.f;
//            return true;
//        }
//
//        return false;
//    }
//
//    //update player
//    void update(){
//        this->updateAttack();
//    }
//
//    //render textures and player, and draw
//    void render(RenderTarget &target){
//        target.draw(this->sprite);
//    }
//
//    //destructor
//    ~Player(){};
//};
//
//
////------------------------------------------------------------------------------------------------------------------------------
//
//
//
//class Game{
//
//private:
//
//    //window
//    RenderWindow *window;
//
//    //player
//    Player *player;
//
//    //enemy
//    std::vector<Enemy*> enemies;
//    float spawnTimer;
//    float spawnTimerMax;
//
//    //gui
//    Font font;
//    Text pointText;
//
//    //point system
//    unsigned points;
//
//    //background
//    Texture b;
//    Sprite background;
//
//    //resources
//    std::map<std::string, Texture*> textures;
//
//    //bullets
//    std::vector <Bullet*> bullets;
//
//
//    //initiate and create window
//    void initwindow(){
//
//        this->window = new RenderWindow(VideoMode(800,600), "Invaders", Style :: Close | Style :: Titlebar);
//        this->window->setFramerateLimit(60);
//        window->setVerticalSyncEnabled(false);
//    }
//
//    //initiate and create player
//    void initiateplayer(){
//        this->player = new Player();
//    }
//
//
//    void initEnemies(){
//        spawnTimerMax = 50.f;
//        spawnTimer = spawnTimerMax;
//    }
//
//    //initiate textures
//    void initTextures(){
//        this->textures["BULLET"] = new Texture();
//        this->textures["BULLET"]->loadFromFile("bullet.png");
//
//        this->textures["ENEMY"] = new Texture();
//        this->textures["ENEMY"]->loadFromFile("enemy.png");
//
//        this->textures["BACKGROUND"] = new Texture();
//        this->textures["BACKGROUND"]->loadFromFile("background.png");
//    }
//
//    void initGUI(){
//        //load fonts
//        if (!font.loadFromFile("PixellettersFull.ttf"))
//            std:: cout << "failed to load" << "\n";
//
//        //init point text
//        pointText.setFont(font);
//        pointText.setCharacterSize(12);
//        pointText.setFillColor(Color::White);
//        pointText.setString("text");
//    }
//
//    void initWorld(){
//
//        if (!this->b.loadFromFile("background.png"))
//        {
//            std::cout << "ERROR::GAME::COULD NOT LOAD BACKGROUND TEXTURE" << "\n";
//        }
//
//        this->background.setTexture(this->b);
//    }
//
//public:
//
//    //constructor
//    Game(){
//        this->initwindow();
//        this->initiateplayer();
//        this->initTextures();
//        this->initGUI();
//        this->initWorld();
//        this->initEnemies();
//
//        points = 0;
//    }
//
//
//    //run game
//    void run(){
//
//        while (this->window->isOpen()){
//            update();
//            render();
//
//        }
//
//    }
//
//    //update game
//    void updatepoll(){
//        Event e;
//
//        while(this->window->pollEvent(e)){
//
//            if (e.Event::type == Event::Closed){
//                window->close();
//            }
//            if (e.Event :: KeyPressed && e.Event::key.code == Keyboard::Escape){
//                window->close();
//            }
//        }
//
//
//    }
//
//    void updateInput(){
//
//        //move player
//
//        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
//            this->player->movement(-1.f,0.f);
//
//        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
//            this->player->movement(1.f, 0.f);
//
//        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
//            this->player->movement(0.f, -1.f);
//
//        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
//            this->player->movement(0.f, 1.f);
//
//        //creating bullets
//        if (Keyboard::isKeyPressed(Keyboard::Space) && this->player->canAttack()){
//            bullets.push_back(new Bullet(this->textures["BULLET"], this->player->getPos().x+32,this->player->getPos().y-35,0.f, -1.f, 5.f));
//        }
//    }
//
//
//    void updateBullets(){
//        int counter{0};
//
//        for (auto *i: this->bullets){
//            i->update();
//
//            if (i->getBounds().top + i->getBounds().height < 0.f){
//
//                //delete bullet outside of screen
//                delete this->bullets.at(counter);
//                this->bullets.erase(this->bullets.begin() + counter);
//                --counter;
//
//                std::cout << this->bullets.size() << "\n";
//            }
//
//            ++counter;
//        }
//    }
//
//
//    void updateEnemies(){
//        //spawning
//        spawnTimer += 0.5f;
//
//        if (spawnTimer >= spawnTimerMax){
//            this->enemies.push_back(new Enemy(rand() % this->window->getSize().x-20.f, -100.f));
//            spawnTimer = 0.f;
//        }
//
//        //updating
//        int counter{0};
//
//        for (auto *enemy: this->enemies){
//            enemy->update();
//
//            if (enemy->getBounds().top > this->window->getSize().y){
//
//                //delete bullet outside of screen
//                delete this->enemies.at(counter);
//                this->enemies.erase(this->enemies.begin() + counter);
//                --counter;
//
//                std::cout << this->enemies.size() << "\n";
//            }
//
//            ++counter;
//        }
//
//    }
//
//
//    void updateCombat(){
//        for (int i{0}; i<enemies.size(); i++){
//            bool enemy_deleted = false;
//
//            for (int k{0}; k<bullets.size() && enemy_deleted == false; k++){
//
//                if (this->enemies[i]->getBounds().intersects(this->bullets[k]->getBounds()))
//                {
//                    //this->points += this->enemies[i]->getPoints();
//
//                    delete this->enemies[i];
//                    this->enemies.erase(this->enemies.begin() + i);
//
//                    delete this->bullets[k];
//                    this->bullets.erase(this->bullets.begin() + k);
//
//                    enemy_deleted = true;
//                }
//            }
//        }
//    }
//
//    void updateCollision(){
//
//        if (this->player->getBounds().left < 0.f){
//            this->player->setPosition(0.f, this->player->getBounds().top);
//        }
//        else if (this->player->getBounds().left + this->player->getBounds().width >= this->window->getSize().x)
//        {
//            this->player->setPosition(this->window->getSize().x - this->player->getBounds().width, this->player->getBounds().top);
//        }
//
//	//Top world collision
//        if (this->player->getBounds().top < 0.f)
//        {
//            this->player->setPosition(this->player->getBounds().left, 0.f);
//        }
//	//Bottom world collision
//        else if (this->player->getBounds().top + this->player->getBounds().height >= this->window->getSize().y)
//        {
//            this->player->setPosition(this->player->getBounds().left, this->window->getSize().y - this->player->getBounds().height);
//        }
//    }
//
//    void updateGUI(){
//        std::stringstream ss;
//        ss <<"Score:  " << this->points;
//        pointText.setString(ss.str());
//    }
//
//
//
//    void update(){
//
//        this->updateInput();
//
//        this->updatepoll();
//
//        this->player->update();
//
//        this->updateCollision();
//
//        this->updateBullets();
//
//        this->updateEnemies();
//
//        this->updateCombat();
//
//        this->updateGUI();
//
//        //this->updateWorld();
//    }
//
//    //clearing, drawing & displaying
//    void render(){
//        this->window->clear();
//
//        //draw player
//
//        renderWorld();
//
//        player->render(*this->window);
//
//
//        //draw bullets
//        for (auto *bullet: bullets){
//            bullet->render(*this->window);
//        }
//
//        for (auto *enemy: enemies){
//            enemy->render(*this->window);
//        }
//
//        //display window
//        window->display();
//
//        renderGUI();
//
//    }
//
//    //render gui
//    void renderGUI(){
//        this->window->draw(pointText);
//    }
//
//    void renderWorld(){
//        this->window->draw(background);
//    }
//
//    //destructor
//    ~Game(){
//        delete this->window;
//        delete this->player;
//
//        //delete textures
//        for (auto &t: this->textures){
//            delete t.second;
//        }
//
//        //delete bullets
//        for (auto &b: this->bullets){
//            delete b;
//        }
//
//        //delete enemies
//        for (auto *enemy: enemies){
//            delete enemy;
//        }
//    }
//
//};
//
////---------------------------------------------------------------------------------------------------------------------------------------------------
//
//
//
//
//
//






//enemy shooter

////LENGTH OF VECTOR: |V| = sqrt(V.x^2 + V.y^2)
////NORMALIZE VECTOR: U = V / |V|
//
//using namespace sf;
//
//class Bullet
//{
//public:
//	CircleShape shape;
//	Vector2f currVelocity;
//	float maxSpeed;
//
//	Bullet(float radius = 5.f)
//		: currVelocity(0.f, 0.f), maxSpeed(15.f)
//	{
//		this->shape.setRadius(radius);
//		this->shape.setFillColor(Color::Red);
//	}
//};
//
//int main()
//{
//	srand(time(NULL));
//
//	RenderWindow window(VideoMode(800, 600), "360 Shooter!", Style::Default);
//	window.setFramerateLimit(60);
//
//	//Player
//	CircleShape player(25.f);
//	player.setFillColor(Color::White);
//	player.setPointCount(3);
//	player.setOrigin(25.f, 25.f);
//
//	//Bullets
//	Bullet b1;
//	std::vector<Bullet> bullets;
//
//	//Enemy
//	RectangleShape enemy;
//	enemy.setFillColor(Color::Magenta);
//	enemy.setSize(Vector2f(50.f, 50.f));
//	int spawnCounter = 20;
//
//	std::vector<RectangleShape> enemies;
//
//	//Vectors
//	Vector2f playerCenter;
//	Vector2f mousePosWindow;
//	Vector2f aimDir;
//	Vector2f aimDirNorm;
//
//	while (window.isOpen())
//	{
//		Event event;
//		while (window.pollEvent(event))
//		{
//			if (event.type == Event::Closed)
//				window.close();
//		}
//
//		//Update
//		//Vectors
//		playerCenter = Vector2f(player.getPosition());
//		mousePosWindow = Vector2f(Mouse::getPosition(window));
//		aimDir = mousePosWindow - playerCenter;
//		aimDirNorm = aimDir / sqrt(pow(aimDir.x, 2) + pow(aimDir.y, 2));
//
//		float PI = 3.14159265f;
//		float deg = atan2(aimDirNorm.y, aimDirNorm.x) * 180 / PI;
//		player.setRotation(deg + 90);
//
//		//Set Final Rotation of Ship
//		std::cout << deg << "\n";
//
//		//Player
//		if (Keyboard::isKeyPressed(Keyboard::A))
//			player.move(-10.f, 0.f);
//		if (Keyboard::isKeyPressed(Keyboard::D))
//			player.move(10.f, 0.f);
//		if (Keyboard::isKeyPressed(Keyboard::W))
//			player.move(0.f, -10.f);
//		if (Keyboard::isKeyPressed(Keyboard::S))
//			player.move(0.f, 10.f);
//
//		//Enemies
//		if (spawnCounter < 20)
//			spawnCounter++;
//
//		if(spawnCounter >= 20 && enemies.size() < 50)
//		{
//			enemy.setPosition(Vector2f(rand() % window.getSize().x, rand() % window.getSize().x));
//			enemies.push_back(RectangleShape(enemy));
//
//			spawnCounter = 0;
//		}
//
//		//Shooting
//		if (Mouse::isButtonPressed(Mouse::Left))
//		{
//			b1.shape.setPosition(playerCenter);
//			b1.currVelocity = aimDirNorm * b1.maxSpeed;
//
//			bullets.push_back(Bullet(b1));
//		}
//
//		for (size_t i = 0; i < bullets.size(); i++)
//		{
//			bullets[i].shape.move(bullets[i].currVelocity);
//
//			//Out of bounds
//			if (bullets[i].shape.getPosition().x < 0 || bullets[i].shape.getPosition().x > window.getSize().x
//				|| bullets[i].shape.getPosition().y < 0 || bullets[i].shape.getPosition().y > window.getSize().y)
//			{
//				bullets.erase(bullets.begin() + i);
//
//			}
//			else
//			{
//				//Enemy collision
//				for (size_t k = 0; k < enemies.size(); k++)
//				{
//					if (bullets[i].shape.getGlobalBounds().intersects(enemies[k].getGlobalBounds()))
//					{
//						bullets.erase(bullets.begin() + i);
//						enemies.erase(enemies.begin() + k);
//						break;
//					}
//				}
//			}
//		}
//
//		//Draw
//		window.clear();
//
//		for (size_t i = 0; i < enemies.size(); i++)
//		{
//			window.draw(enemies[i]);
//		}
//
//		window.draw(player);
//
//		for (size_t i = 0; i < bullets.size(); i++)
//		{
//			window.draw(bullets[i].shape);
//		}
//
//		window.display();
//	}
//
//	return 0;
//}





//BALL SHOOTER

//#include<iostream>
//#include"SFML\Graphics.hpp"
//#include"SFML\Window.hpp"
//#include"SFML\System.hpp"
//
//using namespace sf;
//
//int main()
//{
//	RenderWindow window(VideoMode(640, 480), "Simple game");
//	window.setFramerateLimit(60);
//
//	CircleShape hoop;
//	int dir = 0;
//	hoop.setRadius(50.f);
//	hoop.setFillColor(Color::Black);
//	hoop.setOutlineThickness(2.f);
//	hoop.setOutlineColor(Color::Blue);
//	hoop.setPosition(Vector2f(0, 10.f));
//
//	CircleShape ball;
//	bool isShot = false;
//	ball.setRadius(20.f);
//	ball.setFillColor(Color::Red);
//	ball.setPosition(Vector2f(0, window.getSize().y - ball.getRadius()*3));
//
//	while (window.isOpen())
//	{
//		sf::Event event;
//		while (window.pollEvent(event))
//		{
//			if (event.type == sf::Event::Closed)
//				window.close();
//			if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
//				window.close();
//		}
//
//		//Update hoop
//		if (hoop.getPosition().x <= 0)
//			dir = 1;
//		else if (hoop.getPosition().x + hoop.getRadius()*2 >= window.getSize().x)
//			dir = 0;
//
//		if (dir == 0)
//		{
//			hoop.move(-5.f, 0);
//		}
//		else
//		{
//			hoop.move(5.f, 0);
//		}
//
//		//Update ball
//		if (Mouse::isButtonPressed(Mouse::Left))
//			isShot = true;
//
//		if(!isShot)
//			ball.setPosition(Mouse::getPosition(window).x, ball.getPosition().y);
//		else
//			ball.move(0, -5.f);
//
//		//Collision ball
//		if (ball.getPosition().y <= 0 || ball.getGlobalBounds().intersects(hoop.getGlobalBounds()))
//		{
//			//Reset ball
//			isShot = false;
//			ball.setPosition(ball.getPosition().x, window.getSize().y - ball.getRadius() * 3);
//		}
//
//		//DRAW
//		window.clear(Color::White);
//
//		window.draw(hoop);
//		window.draw(ball);
//
//		window.display();
//	}
//
//	return 0;
//}
















// MOVING BOX IN ALL DIRECTIONS

//void Draw(RenderWindow &target, RectangleShape &square);
//void update(RectangleShape &square, int keytime, RenderWindow &target);
//
//int main(){
//
//    RenderWindow window(VideoMode(640,400), "SFML");
//    window.setFramerateLimit(60);
//
//    int keytime = 8;
//
//    RectangleShape square(Vector2f(100.f, 100.f));
//    square.setFillColor(Color::Blue);
//    square.setPosition(window.getSize().x/2, window.getSize().y/2);
//
//    while (window.isOpen()){
//        Event event;
//
//        while (window.pollEvent(event)){
//            if (event.type == Event::Closed){
//                window.close();
//            }
//
//            if (event.KeyPressed && event.key.code == Keyboard::Escape){
//                window.close();
//            }
//        }
//
//
//        //update
//        update(square, keytime, window);
//
//        Draw(window, square);
//
//        //display
////        window.display();
//    }
//
//return 0;
//}
//
//void update(RectangleShape &square, int keytime, RenderWindow &target){
//
//    if (keytime < 8){
//        keytime++;
//    }
//
//    if (Keyboard::isKeyPressed(Keyboard::Up) && square.getPosition().y > 0){
//
//        square.move(0.0f, -5.f);
//        //keytime = 0;
//    }
//
//    if (Keyboard::isKeyPressed(Keyboard::Down)  && square.getPosition().y + square.getSize().y < target.getSize().y){
//
//        square.move(0.0f, 5.f);
//        keytime = 0;
//    }
//
//    if (Keyboard::isKeyPressed(Keyboard::Right)&& square.getPosition().x + square.getSize().x < target.getSize().x){
//
//        square.move(5.f, 0.0f);
//        keytime = 0;
//    }
//
//    if (Keyboard::isKeyPressed(Keyboard::Left) && square.getPosition().x > 0){
//
//        square.move(-5.f, 0.0f);
//        keytime = 0;
//    }
//
//    if (Mouse::isButtonPressed(Mouse::Left)){
//
//        square.setFillColor(Color::Red);
//        keytime = 0;
//    }
//}
//
//
//void Draw(RenderWindow &target, RectangleShape &square){
//
//    target.clear(Color::White);
//
//    target.draw(square);
//
//    target.display();
//}
