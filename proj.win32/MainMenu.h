
#include "cocos2d.h"
class MainMenu :public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	void menuItemStartCallback(Ref* pSender);
	void menuItemSettingCallback(Ref* pSender);
	void MainMenu::menuCloseCallback(Ref* pSender);
	CREATE_FUNC(MainMenu);
};

