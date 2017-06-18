#include "cocos2d.h"
USING_NS_CC;

void map_init();
Point coordinate_change1(Point position);
Point coordinate_change2(Point position);
Point coordinate_change3(Point position);

class Character : public cocos2d::Sprite
{
public:
	Sprite *ch;
	Sprite *com;
	Point ch_position;
	int bomb_number, ai_bomb_number;
	int bomb_power, ai_bomb_power;
	float speed, ai_speed;
	bool is_run;
	bool is_trapped;
	bool ai_is_trapped;
	static Character *createCharacter(cocos2d::Point position);
	virtual bool init();
	void characterInit(cocos2d::Point position);
	void moveup(float dt);
	void movedown(float dt);
	void moveleft(float dt);
	void moveright(float dt);
	int barrierx;
	int barriery;
	bool is_can_reach(float x, float y);
	void set_bomb(Point position);
	void ai_set_bomb(Point position);
	void update(float dt);
	Animation* createAnimate(const char* direction);
	int aiGetPosSign(Vec2 pos);//ai获得某个点的标记信息，标记这个点的危险程度以及这个点是否可以移动
	bool aiCanEscape(Vec2 pos);//ai是否能够逃跑，能够逃跑才能放泡泡
	void aiMove(float dt);//ai移动
	std::vector<Vec2 > escapePos;
};

class Bomb : public cocos2d::Sprite
{
public:
	Sprite *bomb;
	int bomb_number;
	int bomb_power;
	int mapx;
	int mapy;
	bool set_explosion;
	void explosion(float dt);
	void explosion_reset(float dt);
	static Bomb *createBomb(cocos2d::Point position);
	virtual bool init();
	void bombInit(cocos2d::Point position);
	void drop_item(Point position);
	void exp(float dt);
	void ai_exp(float dt);
};

class MapChoose :public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	void ChooseTheFirstMap(Ref* pSender);
	void ChooseTheSecondMap(Ref* pSender);
	CREATE_FUNC(MapChoose);
};

class GamePage : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(GamePage);
};
