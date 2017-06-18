#include "Setting.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "MainMenu.h"

USING_NS_CC;

using namespace CocosDenshion;

Scene* Setting::createScene()
{
	auto scene = Scene::create();
	auto layer = Setting::create();
	scene->addChild(layer);
	return scene;
}

bool Setting::init()
{
	if (!Layer::init())
	{
		return false;
	}
	//scene
	Size size = Director::getInstance()->getVisibleSize();
	auto background = Sprite::create("setting-back.jpg");
	background->setPosition(size.width / 2, size.height / 2);
	background->setScale(2.5f);
	addChild(background, 0);
	auto label = Label::create("music", "Arial", 36, Size::ZERO, TextHAlignment::LEFT, TextVAlignment::TOP);
	label->setPosition(size.width / 10, size.height / 4 * 3);
	this->addChild(label);
	auto pturnOn = MenuItemImage::create("music_on.png", "music_off.png");
	auto pturnOff = MenuItemImage::create("music_off.png", "music_on.png");
	MenuItemToggle *music_switch = MenuItemToggle::createWithCallback(CC_CALLBACK_1(Setting::menuMusicCallback, this), pturnOn, pturnOff, NULL);
	music_switch->setScale(1.0f);
	auto* menu= Menu::create(music_switch, NULL);
	menu->setPosition(Point(size.width / 6, size.height / 4 * 3));
	addChild(menu);
	auto *labelbutton = LabelTTF::create("E X I T !", "fonts/Marker Felt.ttf", 36);
	auto *plabelbutton = MenuItemLabel::create(labelbutton, this,menu_selector(Setting::Exit));
	auto *ExitButton = Menu::create(plabelbutton, NULL);
	ExitButton->setPosition(size.width / 2, size.height / 4);
	this->addChild(ExitButton);
	return true;
}

void Setting::menuMusicCallback(Ref* pSender)
{
	is_played = SimpleAudioEngine::sharedEngine()->isBackgroundMusicPlaying();
	if (is_played == false)
	{
		SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
		is_played = true;
	}
	else
	{
		SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
		is_played = false;
	}
}

void Setting::Exit(Ref* pSender)
{
	auto *scene = MainMenu::createScene();
	Director::getInstance()->pushScene(scene);

}