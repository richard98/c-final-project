#include "GamePage.h"
#include "cocos2d.h"
#include "stdlib.h"
#include "SimpleAudioEngine.h"
USING_NS_CC;

TMXTiledMap* map;
TMXLayer* destroy;
TMXLayer* can_not_destroy;
Bomb* bomb_pointer[15][15];
bool explosion_area[15][15];
Sprite* item[15][15];
int bomb_in_page, ai_bomb_in_page;
int mapname = 0;

Scene* MapChoose::createScene()
{
	auto scene = Scene::create();
	auto layer = MapChoose::create();
	scene->addChild(layer);
	return scene;
}

bool MapChoose::init()
{
	if (!Layer::init())
	{
		return false;
	}
	//scene
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto label = Label::create("Please choose a map!", "Arial", 36, Size::ZERO, TextHAlignment::LEFT, TextVAlignment::TOP);
	label->setPosition(visibleSize.width / 2, visibleSize.height / 4 * 3);
	this->addChild(label);
	Size size = Director::getInstance()->getVisibleSize();
	auto* background = Sprite::create("background.png");
	background->setPosition(size.width / 2, size.height / 2);
	background->setScale(0.7f);
	addChild(background, 0);
	auto* pLawnMap = MenuItemImage::create("lawnmap1.png", "lawnmap2.png", this, menu_selector(MapChoose::ChooseTheFirstMap));
	auto* lawnmap = Menu::create(pLawnMap, NULL);
	lawnmap->setAnchorPoint(Vec2(0, 0));
	lawnmap->setPosition(visibleSize.width / 4, visibleSize.height / 4);
	lawnmap->setScale(0.6f);
	addChild(lawnmap);
	auto* piceMap = MenuItemImage::create("icemap1.png", "icemap2.png", this, menu_selector(MapChoose::ChooseTheSecondMap));
	auto* icemap = Menu::create(piceMap, NULL);
	icemap->setAnchorPoint(Vec2(0, 0));
	icemap->setPosition(visibleSize.width / 4 * 3, visibleSize.height / 4);
	icemap->setScale(0.6f);
	addChild(icemap);
	return true;
}

void MapChoose::ChooseTheFirstMap(Ref* pSender)
{
	mapname = 1;
	auto *scene = GamePage::createScene();
	Director::getInstance()->pushScene(scene);
}

void MapChoose::ChooseTheSecondMap(Ref* pSender)
{
	mapname = 2;
	auto *scene = GamePage::createScene();
	Director::getInstance()->pushScene(scene);
}

Scene* GamePage::createScene()
{
	auto scene = Scene::create();
	auto layer = GamePage::create();
	scene->addChild(layer);
	return scene;
}

bool GamePage::init()
{
	if (!Layer::init())
	{
		return false;
	}
	//scene
	Size size = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	//获取窗口大小及原点
	auto *character = Character::createCharacter(Vec2(20, 20));
	map_init();
	addChild(map,1);
	addChild(character,10);
	//添加人物
	character->schedule(schedule_selector(Character::aiMove), 1.0f);
	character->schedule(schedule_selector(Character::update));
	auto KeyListener = EventListenerKeyboard::create();
	KeyListener->onKeyPressed = [character,this](EventKeyboard::KeyCode keycode, cocos2d::Event *event)
	{
		float x = character->ch->getPositionX();
		float y = character->ch->getPositionY();
		if (!character->is_trapped)
			switch ((int)keycode)
			{
			case 26:
				if (!character->is_run)
				{
					character->schedule(schedule_selector(Character::moveleft));
				}
				break;
			case 27:
				if (!character->is_run)
				{
					character->schedule(schedule_selector(Character::moveright));
					auto* animation = character->createAnimate("right");
					auto* action = Animate::create(animation);
					character->ch->runAction(action);
				}
				break;
			case 28:
				if (!character->is_run)
				{
					character->schedule(schedule_selector(Character::moveup));
					auto* animation = character->createAnimate("up");
					auto* action = Animate::create(animation);
					character->ch->runAction(action);
				}
				break;
			case 29:
				if (!character->is_run)
				{
					character->schedule(schedule_selector(Character::movedown));
					auto* animation = character->createAnimate("down");
					auto* action = Animate::create(animation);
					character->ch->runAction(action);
				}
				break;
			case 59:
				if (bomb_in_page < character->bomb_number)
				{
					character->set_bomb(Vec2(x, y));
				}
				break;
			default:
				break;
			}
	};
	KeyListener->onKeyReleased = [character](EventKeyboard::KeyCode keycode, cocos2d::Event *event)
	{
		if (!character->is_trapped)
		switch ((int)keycode)
		{
		case 26:
			character->unschedule(schedule_selector(Character::moveleft));
			character->ch->stopAllActions();
			character->is_run = false;
			break;
		case 27:
			character->unschedule(schedule_selector(Character::moveright));
			character->ch->stopAllActions();
			character->is_run = false;
			break;
		case 28:
			character->unschedule(schedule_selector(Character::moveup));
			character->ch->stopAllActions();
			character->is_run = false;
			break;
		case 29:
			character->unschedule(schedule_selector(Character::movedown));
			character->ch->stopAllActions();
			character->is_run = false;
			break;
		default:
			break;
		}
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(KeyListener, this);
	//添加键盘监视
	return true;
}

Character *Character::createCharacter(Point position)
{
	Character *character = new Character();
	if (character && character->init())
	{
		character->autorelease();
		character->characterInit(position);
		character->is_trapped = false;
		character->ai_is_trapped = false;
		character->is_run = false;
		character->speed = 1.0f;
		character->ai_speed = 1.0f;
		character->bomb_number = 2;
		character->bomb_number = 2;
		character->bomb_power = 2;
		character->bomb_power = 2;
		return character;
	}
	CC_SAFE_DELETE(character);
	return NULL;
}

bool Character::init()
{
	if (!Sprite::init())
	{
		return false;
	}
	return true;
}

void Character::characterInit(Point position)
{
	ch = Sprite::create("friend_01.png");
	ch->setAnchorPoint(Vec2(0.5f, 0.4f));
	ch->setScale(1.0f);
	ch->setPosition(position);
	com = Sprite::create("friend_01.png");
	com->setAnchorPoint(Vec2(0.5f, 0.4f));
	com->setPosition(Vec2(580, 580));
	addChild(com, 10);
	addChild(ch, 10);
}

Animation* Character::createAnimate(const char* direction)
{
	auto* m_frameCache = SpriteFrameCache::getInstance();
	m_frameCache->addSpriteFramesWithFile("run_down.plist", "run_down.png");
	Vector<SpriteFrame*> frameArray;
	for (int i = 1; i < 5; i++)
	{
		auto name = String::createWithFormat("run%s%d.png", direction, i)->getCString();
		auto* frame = m_frameCache->getSpriteFrameByName(name);
		frameArray.pushBack(frame);
	}
	Animation* animation = Animation::createWithSpriteFrames(frameArray);
	animation->setLoops(-1);
	animation->setDelayPerUnit(0.1f);
	return animation;
}

void Character::moveup(float dt)
{
	if (is_trapped)
		return;
	Size mapSize = map->getMapSize();
	Size tileSize = map->getTileSize();
	float y = ch->getPositionY();
	if (y + tileSize.width / 2<mapSize.height*tileSize.height&&is_can_reach(ch->getPositionX() - tileSize.width / 2 + 5, y + tileSize.height / 2) && is_can_reach(ch->getPositionX() + tileSize.width / 2 - 5, y + tileSize.height / 2))
	{
		ch->runAction(MoveBy::create(0.01f, Vec2(0, speed)));
		is_run = true;
	}
	barrierx = 0;
	barriery = -3;
	return;
}

void Character::movedown(float dt)
{
	if (is_trapped)
		return;
	Size mapSize = map->getMapSize();
	Size tileSize = map->getTileSize();
	float y = ch->getPositionY();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	if (y>tileSize.width / 2 && is_can_reach(ch->getPositionX() - tileSize.width / 2 + 5, y - tileSize.height / 2) && is_can_reach(ch->getPositionX() + tileSize.width / 2 - 5, y - tileSize.height / 2))
	{
		auto* action = MoveBy::create(0.01f, Vec2(0, -speed));
		ch->runAction(action);
		is_run = true;
	}
	barrierx = 0;
	barriery = 3;
	return;
}

void Character::moveleft(float dt)
{
	if (is_trapped)
		return;
	Size mapSize = map->getMapSize();
	Size tileSize = map->getTileSize();
	float x = ch->getPositionX();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	if (x>tileSize.width / 2 && is_can_reach(x - tileSize.width / 2, ch->getPositionY() + tileSize.height / 2 - 5) && is_can_reach(x - tileSize.width / 2, ch->getPositionY() - tileSize.height / 2 + 5))
	{
		ch->runAction(MoveBy::create(0.01f, Vec2(-speed, 0)));
		is_run = true;
	}
	barrierx = 3;
	barriery = 0;
	return;
}

void Character::moveright(float dt)
{
	if (is_trapped)
		return;
	Size mapSize = map->getMapSize();
	Size tileSize = map->getTileSize();
	float x = ch->getPositionX();
	if (x + tileSize.width / 2<mapSize.height*tileSize.height&&is_can_reach(x + tileSize.width / 2, ch->getPositionY() + tileSize.height / 2 - 5) && is_can_reach(x + tileSize.width / 2, ch->getPositionY() - tileSize.height / 2 + 5))
	{
		ch->runAction(MoveBy::create(0.01f, Vec2(speed, 0)));
		is_run = true;
	}
	barrierx = -3;
	barriery = 0;
	return;
}

bool Character::is_can_reach(float x, float y)
{
	bool result = false;
	Size tileSize = map->getTileSize();
	Point pos = coordinate_change3(Vec2(x, y));
	int mapx= pos.x;
	int mapy = pos.y;
	if (mapx > 15 || mapy > 15 || mapx < 0 || mapy < 0)
	{
		ch->runAction(MoveBy::create(0.01f, Vec2(barrierx, barriery)));
		return false;
	}
	int tileGidD = destroy->tileGIDAt(Vec2(mapx, mapy));
	int tileGidN = can_not_destroy->tileGIDAt(Vec2(mapx, mapy));
	if (!tileGidD && !tileGidN && bomb_pointer[mapx][mapy] == NULL)
		result = true;
	else if (bomb_pointer[mapx][mapy] != NULL)
	{
		Point poi = coordinate_change2(Vec2(mapx, mapy));
		if (x - poi.x < tileSize.width / 2 || poi.x - x < tileSize.width / 2 || y - poi.y < tileSize.height / 2 || poi.y - y < tileSize.height)
			return true;
	}
	else
		ch->runAction(MoveBy::create(0.01f, Vec2(barrierx, barriery)));
	return result;
}


void map_init()
{
	if (mapname = 1)
		map = TMXTiledMap::create("lawnmap.tmx");
	else map = TMXTiledMap::create("icemap.tmx");
	map->setPosition(Vec2(0,0));
	map->setScale(1.0f);
	destroy = map->getLayer("destroym");
	can_not_destroy = map->getLayer("can_not_destroym");
	Size mapsize = map->getMapSize();
	for (int x = 0; x < mapsize.width; x++)
		for (int y = 0; y < mapsize.height; y++)
		{
			bomb_pointer[x][y] = NULL;
			explosion_area[x][y] = false;
		}
}

void Character::set_bomb(Point position)
{
	Point pos = coordinate_change3(position);
	int x = pos.x;
	int y = pos.y;
	if (bomb_in_page < bomb_number && bomb_pointer[x][y] == NULL)
	{
		bomb_in_page++;
		Point bombpos = coordinate_change2(pos);
		Bomb *bomb = Bomb::createBomb(Vec2(bombpos.x, bombpos.y));
		bomb->mapx = x;
		bomb->mapy = y;
		bomb->bomb_power = bomb_power;
		addChild(bomb, 9);
		bomb->scheduleOnce(schedule_selector(Bomb::explosion), 3.0f);
		bomb->scheduleOnce(schedule_selector(Bomb::exp), 3.0f);
		bomb_pointer[bomb->mapx][bomb->mapy] = bomb;
	}
}

void Bomb::exp(float dt)
{
	bomb_in_page--;
}

void Character::ai_set_bomb(Point position)
{
	Point pos = coordinate_change3(position);
	int x = pos.x;
	int y = pos.y;
	if (ai_bomb_in_page < ai_bomb_number && bomb_pointer[x][y] == NULL)
	{
		ai_bomb_in_page++;
		Point bombpos = coordinate_change2(pos);
		Bomb *bomb = Bomb::createBomb(Vec2(bombpos.x, bombpos.y));
		bomb->mapx = x;
		bomb->mapy = y;
		bomb->bomb_power = ai_bomb_power;
		addChild(bomb, 9);
		bomb->scheduleOnce(schedule_selector(Bomb::explosion), 3.0f);
		bomb->scheduleOnce(schedule_selector(Bomb::ai_exp), 3.0f);
		bomb_pointer[bomb->mapx][bomb->mapy] = bomb;
	}
}

void Bomb::ai_exp(float dt)
{
	ai_bomb_in_page--;
}

void Character::update(float dt)
{
	Point position = this->ch->getPosition();
	Point ai_position = this->com->getPosition();
	Point mappos = coordinate_change3(position);
	Point ai_mappos = coordinate_change3(ai_position);
	ch_position.x = position.x;
	ch_position.y = position.y;
	if (explosion_area[(int)mappos.x][(int)mappos.y])
		is_trapped = true;
	if (explosion_area[(int)ai_mappos.x][(int)ai_mappos.y])
		ai_is_trapped = true;
	if (item[(int)mappos.x][(int)mappos.y] != NULL)
	{
		switch (item[(int)mappos.x][(int)mappos.y]->getTag())
		{
		case 1:
			if (speed < 3)
				speed++;
			break;
		case 2:
			bomb_number++;
			break;
		case 3:
			bomb_power++;
			break;
		}
		item[(int)mappos.x][(int)mappos.y]->removeFromParentAndCleanup(true);
		item[(int)mappos.x][(int)mappos.y] = NULL;
	}
	if(item[(int)ai_mappos.x][(int)ai_mappos.y] != NULL)
	{
		switch (item[(int)ai_mappos.x][(int)ai_mappos.y]->getTag())
		{
		case 1:
			if (ai_speed < 3)
				ai_speed++;
			break;
		case 2:
			ai_bomb_number++;
			break;
		case 3:
			ai_bomb_power++;
			break;
		}
		item[(int)ai_mappos.x][(int)ai_mappos.y]->removeFromParentAndCleanup(true);
		item[(int)ai_mappos.x][(int)ai_mappos.y] = NULL;
	}
}

void Bomb::explosion(float dt)
{
	Size mapSize = map->getMapSize();
	bomb_pointer[mapx][mapy] = NULL;
	removeChild(bomb, true); 
	for (int block = 1; block <= bomb_power; block++)
	{
		if (mapy + block < mapSize.height)
		{
			explosion_area[mapx][mapy + block] = true;
			if (destroy->getTileGIDAt(Vec2(mapx, mapy + block)))
			{
				destroy->setTileGID(0, Vec2(mapx, mapy + block));
				drop_item(Vec2(mapx, mapy + block));
				break;
			}
			else if (can_not_destroy->getTileGIDAt(Vec2(mapx, mapy + block)))
				break;
			else if (bomb_pointer[mapx][mapy + block] != NULL)
			{
				bomb_pointer[mapx][mapy + block]->unschedule(schedule_selector(Bomb::explosion));
				bomb_pointer[mapx][mapy + block]->explosion(0.1f);
				break;
			}
		}
	}
	for (int block = 1; block <= bomb_power; block++)
	{
		if (mapy - block >= 0)
		{
			explosion_area[mapx][mapy - block] = true;
			if (destroy->getTileGIDAt(Vec2(mapx, mapy - block)))
			{
				destroy->setTileGID(0, Vec2(mapx, mapy - block));
				drop_item(Vec2(mapx, mapy - block));
				break;
			}
			else if (can_not_destroy->getTileGIDAt(Vec2(mapx, mapy - block)))
				break;
			else if (bomb_pointer[mapx][mapy - block] != NULL)
			{
				bomb_pointer[mapx][mapy - block]->unschedule(schedule_selector(Bomb::explosion));
				bomb_pointer[mapx][mapy - block]->explosion(0.1f);
				break;
			}
		}
	}
	for (int block = 1; block <= bomb_power; block++)
	{
		if (mapx + block < mapSize.width)
		{
			explosion_area[mapx + block][mapy] = true;
			if (destroy->getTileGIDAt(Vec2(mapx + block, mapy)))
			{
				destroy->setTileGID(0, Vec2(mapx + block, mapy));
				drop_item(Vec2(mapx + block, mapy));
				break;
			}
			else if (can_not_destroy->getTileGIDAt(Vec2(mapx + block, mapy)))
				break;
			else if (bomb_pointer[mapx + block][mapy] != NULL)
			{
				bomb_pointer[mapx + block][mapy]->unschedule(schedule_selector(Bomb::explosion));
				bomb_pointer[mapx + block][mapy]->explosion(0.1f);
				break;
			}
		}
	}
	for (int block = 1; block <= bomb_power; block++)
	{
		if (mapx - block >= 0)
		{
			explosion_area[mapx - block][mapy] = true;
			if (destroy->getTileGIDAt(Vec2(mapx - block, mapy)))
			{
				destroy->setTileGID(0, Vec2(mapx - block, mapy));
				drop_item(Vec2(mapx - block, mapy));
				break;
			}
			else if (can_not_destroy->getTileGIDAt(Vec2(mapx - block, mapy)))
				break;
			else if (bomb_pointer[mapx - block][mapy] != NULL)
			{
				bomb_pointer[mapx - block][mapy]->unschedule(schedule_selector(Bomb::explosion));
				bomb_pointer[mapx - block][mapy]->explosion(0.1f);
				break;
			}
		}
	}
	scheduleOnce(schedule_selector(Bomb::explosion_reset),0.1f);
}

Bomb *Bomb::createBomb(Point position)
{
	Bomb *bomb = new Bomb();
	if (bomb && bomb->init())
	{
		bomb->autorelease();
		bomb->bombInit(position);
		return bomb;
	}
	CC_SAFE_DELETE(bomb);
	return NULL;
}

bool Bomb::init()
{
	if (!Sprite::init())
	{
		return false;
	}
	return true;
}

void Bomb::bombInit(Point position)
{
	bomb = Sprite::create("item_96.png");
	bomb->setAnchorPoint(Vec2(0.5f, 0.5f));
	bomb->setScale(1.0f);
	bomb->setPosition(position);
	addChild(bomb,9);
}

void Bomb::drop_item(Point position)
{
	int itemID = rand() % 6;
	Point pos = coordinate_change2(position);
	int x = position.x;
	int y = position.y;
	switch (itemID)
	{
	case 0:
	{
		auto* item1 = Sprite::create("item_64.png");
		item1->setPosition(pos);
		item1->setScale(1.0f);
		item1->setTag(1);
		item[x][y] = item1;
		addChild(item1,8);
		break;
	}
	case 1:
	{
		auto* item2 = Sprite::create("item_61.png");
		item2->setPosition(pos);
		item2->setScale(1.0f);
		item2->setTag(2);
		item[x][y] = item2;
		addChild(item2,8);
		break;
	}
	case 2:
	{
		auto* item3 = Sprite::create("item_62.png");
		item3->setPosition(pos);
		item3->setScale(1.0f);
		item3->setTag(3);
		item[x][y] = item3;
		addChild(item3,8);
		break;
	}
	default:
		break;
	}
}

Point coordinate_change1(Point position) //找到最近的瓦片的中心
{
	Size tileSize = map->getTileSize();
	int mapx = position.x / tileSize.width;
	int mapy = position.y / tileSize.height;
	return Vec2(mapx*tileSize.width + tileSize.width / 2, mapy*tileSize.height + tileSize.height / 2);
}

Point coordinate_change2(Point position) //瓦片地图坐标转换为屏幕坐标
{
	Size mapSize = map->getMapSize();
	Size tileSize = map->getTileSize();
	return Vec2(position.x*tileSize.width + tileSize.width / 2, (mapSize.height - position.y - 1)*tileSize.height + tileSize.height / 2);
}

Point coordinate_change3(Point position) //屏幕坐标转换为瓦片地图坐标
{
	Size mapSize = map->getMapSize();
	Size tileSize = map->getTileSize();
	int mapx = position.x / tileSize.width;
	int mapy = mapSize.height - position.y / tileSize.height;
	if (position.y > mapSize.height*tileSize.height)
		mapy = -1;
	return Vec2(mapx, mapy);
}

void Bomb::explosion_reset(float dt)
{
	Size mapsize = map->getMapSize();
	for (int x = 0; x < mapsize.width; x++)
		for (int y = 0; y < mapsize.height; y++)
			explosion_area[x][y] = false;
}

int Character::aiGetPosSign(cocos2d::Vec2 pos)
{
	Size mapSize = map->getMapSize();
	pos = coordinate_change3(pos);
	if (pos.x < 0 || pos.x >= mapSize.width || pos.y < 0 || pos.y >= mapSize.height)
	{
		return -1;
	}
	else if (destroy->getTileGIDAt(pos) != 0 || can_not_destroy->getTileGIDAt(pos) != 0 || bomb_pointer[(int)pos.x][(int)pos.y] != NULL)
	{//被箱子或者障碍物挡住
		return -1;
	}
	for (int ins = 1; ins <= 14; ins++)
	{
		if (pos.x + ins <= 14 && bomb_pointer[(int)pos.x + ins][(int)pos.y] != NULL && ins <= bomb_power)
			return ins;
		if (pos.x - ins >= 0 && bomb_pointer[(int)pos.x - ins][(int)pos.y] != NULL && ins <= bomb_power)
			return ins;
		if (pos.y + ins <= 14 && bomb_pointer[(int)pos.x][(int)pos.y + ins] != NULL && ins <= bomb_power)
			return ins;
		if (pos.y - ins >= 0 && bomb_pointer[(int)pos.x][(int)pos.y - ins] != NULL && ins <= bomb_power)
			return ins;
	}
	return 10;
}

bool Character::aiCanEscape(cocos2d::Vec2 pos)
{
	escapePos.clear();
	if (aiGetPosSign(pos + Vec2(0, 40)) == 10 && aiGetPosSign(pos + Vec2(40, 40)) == 10)
	{//当前点的上边和右上可以逃跑
		escapePos.push_back(pos + Vec2(0, 40));
		escapePos.push_back(pos + Vec2(40, 40));
		return true;
	}
	if (aiGetPosSign(pos + Vec2(0, 40)) == 10 && aiGetPosSign(pos + Vec2(-40, 40)) == 10)
	{//上，左上
		escapePos.push_back(pos + Vec2(0, 40));
		escapePos.push_back(pos + Vec2(-40, 40));
		return true;
	}
	if (aiGetPosSign(pos + Vec2(0, -40)) == 10 && aiGetPosSign(pos + Vec2(40, -40)) == 10)
	{//下，右下
		escapePos.push_back(pos + Vec2(0, -40));
		escapePos.push_back(pos + Vec2(40, -40));
		return true;
	}
	if (aiGetPosSign(pos + Vec2(0, -40)) == 10 && aiGetPosSign(pos + Vec2(-40, -40)) == 10)
	{//下，左下
		escapePos.push_back(pos + Vec2(0, -40));
		escapePos.push_back(pos + Vec2(-40, -40));
		return true;
	}
	if (aiGetPosSign(pos + Vec2(40, 0)) == 10 && aiGetPosSign(pos + Vec2(40, 40)) == 10)
	{//右,右上
		escapePos.push_back(pos + Vec2(40, 0));
		escapePos.push_back(pos + Vec2(40, 40));
		return true;
	}
	if (aiGetPosSign(pos + Vec2(40, 0)) == 10 && aiGetPosSign(pos + Vec2(40, -40)) == 10)
	{//右，右下
		escapePos.push_back(pos + Vec2(40, 0));
		escapePos.push_back(pos + Vec2(40, -40));
		return true;
	}
	if (aiGetPosSign(pos + Vec2(-40, 0)) == 10 && aiGetPosSign(pos + Vec2(-40, 40)) == 10)
	{//左，左上
		escapePos.push_back(pos + Vec2(-40, 0));
		escapePos.push_back(pos + Vec2(-40, 40));
		return true;
	}
	if (aiGetPosSign(pos + Vec2(-40, 0)) == 10 && aiGetPosSign(pos + Vec2(-40, -40)) == 10)
	{//左，左下
		escapePos.push_back(pos + Vec2(-40, 0));
		escapePos.push_back(pos + Vec2(-40, -40));
		return true;
	}
	return false;
}

void Character::aiMove(float dt)
{
	if (ai_is_trapped) return;
	std::vector<Vec2 > poses1;//没有危险的点
	std::vector<Vec2 > poses2;//距离泡泡2的点
	std::vector<Vec2 > poses3;//距离泡泡3的点

	Vec2 currentPos = com->getPosition();//当前玩家位置
	int currentSign = aiGetPosSign(com->getPosition());//当前位置，标记
	std::vector<Vec2> nearPoses;//四个相邻的位置
	nearPoses.push_back(currentPos + Vec2(0, 40));
	nearPoses.push_back(currentPos + Vec2(0, -40));
	nearPoses.push_back(currentPos + Vec2(40, 0));
	nearPoses.push_back(currentPos + Vec2(-40, 0));
	for (int i = 0; i < 4; i++) {//获得四个点的位置标记信息
		switch (aiGetPosSign(nearPoses.at(i)))
		{
		case 10:
			poses1.push_back(nearPoses.at(i));
			break;
		case 2:
			poses2.push_back(nearPoses.at(i));
			break;
		case 3:
			poses3.push_back(nearPoses.at(i));
			break;
		default:
			break;
		}
	}
	if (escapePos.size() == 0)
	{//如果没有逃跑路线，说明ai还没有放泡泡
		if (rand() % 5 == 0 && aiCanEscape(currentPos))
		{//如果可以逃跑，1/5概率添加泡泡
			ai_set_bomb(com->getPosition());
		}
		else
		{
			escapePos.clear();
		}
	}
	Vec2 movePos = Vec2::ZERO;
	if (escapePos.size() > 0)
	{//如果有逃跑路线，需要按照逃跑路线移动
		movePos = escapePos.at(0);
		escapePos.erase(escapePos.begin());
	}
	else
	{
		float dis = 10000;
		if (poses1.size() > 0)
		{//如果有没有危险的点
			for (int i = 0; i < poses1.size(); i++)
			{//选择一个距离红色玩家近的点移动
				if (poses1.at(i).getDistance(com->getPosition()) < dis)
				{
					dis = poses1.at(i).getDistance(ch->getPosition());
					movePos = poses1.at(i);
				}
			}
		}
		else if (currentSign == 1 && poses2.size() > 0)
		{//如果当前位置距离泡泡为1，有距离为2的点，移动到距离为2的点
			for (int i = 0; i < poses2.size(); i++)
			{
				if (poses2.at(i).getDistance(com->getPosition()) < dis)
				{
					dis = poses2.at(i).getDistance(ch->getPosition());
					movePos = poses2.at(i);
				}
			}
		}
		else if (currentSign == 2 && poses3.size() > 0)
		{//如果当前位置距离泡泡为2，有距离为3的点，移动到距离为3的点
			for (int i = 0; i < poses3.size(); i++)
			{
				if (poses3.at(i).getDistance(com->getPosition()) < dis)
				{
					dis = poses3.at(i).getDistance(ch->getPosition());
					movePos = poses3.at(i);
				}
			}
		}
		if (movePos == Vec2::ZERO)
		{//如果没有点可以移动就停住不动，等待泡泡爆炸再移动
			return;
		}
	}

	Vec2 deltaPos = movePos - currentPos;//移动数值
										 //计算需要旋转的角度
	float radians = atan2(deltaPos.y, deltaPos.x);
	com->stopAllActions();
	if (UserDefault::getInstance()->getBoolForKey("effect", true))
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("move.mp3");
	//蓝色玩家移动
	//if (movePos.x > 0 && movePos.x < 600 && movePos.y>0 && movePos.y < 600)
		com->runAction(MoveBy::create(1.0f, deltaPos));

}