#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Terain.h"

class HelloWorld : public cocos2d::Scene
{
private:
	Terain* _terain;

public:
	virtual bool init() override;
};

#endif  // __HELLOWORLD_SCENE_H__
