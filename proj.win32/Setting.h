#include "cocos2d.h"

class Setting : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	bool is_played;
	void menuMusicCallback(Ref* pSender);
	void Exit(Ref* pSender);
	CREATE_FUNC(Setting);
};

