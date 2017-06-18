#include "MainMenu.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "GamePage.h"
#include "Setting.h"

USING_NS_CC;

Scene* MainMenu::createScene()
{
	auto scene = Scene::create();
	auto layer = MainMenu::create();
	scene->addChild(layer);
	return scene;
}

bool MainMenu::init()
{
	if (!Layer::init())
	{
		return false;
	}
	//scene
	Size size = Director::getInstance()->getVisibleSize();
	auto background = Sprite::create("background.png");
	background->setPosition(size.width / 2, size.height / 2);
	background->setScale(0.5f);
	addChild(background, 0);
	//Ìí¼Ó±³¾°
	auto *pstartbutton = MenuItemImage::create("start-up.png", "start-down.png", this, menu_selector(MainMenu::menuItemStartCallback));
	auto *psettingbutton = MenuItemImage::create("setting-up.png", "setting-down.png", this, menu_selector(MainMenu::menuItemSettingCallback));
	auto *pclosebutton = MenuItemImage::create("help-up.png", "help-down.png", this, menu_selector(MainMenu::menuCloseCallback));
	pstartbutton->setPosition(size.width / 5 * 4, size.height / 4 * 3);
	pstartbutton->setScale(0.6f);
	psettingbutton->setPosition(size.width / 5 * 4, size.height / 4 * 2);
	psettingbutton->setScale(0.6f);
	pclosebutton->setPosition(size.width / 5 * 4, size.height / 4);
	pclosebutton->setScale(0.6f);
	auto button = Menu::create(pstartbutton, psettingbutton, pclosebutton, NULL);
	button->setPosition(Vec2::ZERO);
	addChild(button);
	//Ìí¼Ó°´Å¥
	return true;
}

void MainMenu::menuItemStartCallback(Ref* pSender)
{
	auto *scene = MapChoose::createScene();
	Director::getInstance()->pushScene(scene);
}

void MainMenu::menuItemSettingCallback(Ref* pSender)
{
	auto *scene = Setting::createScene();
	Director::getInstance()->pushScene(scene);
}

void MainMenu::menuCloseCallback(Ref* pSender)
{
	//Close the cocos2d-x game scene and quit the application
	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}