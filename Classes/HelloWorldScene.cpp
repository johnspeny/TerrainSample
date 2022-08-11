#include "HelloWorldScene.h"
#include "SampleScene.h"

USING_NS_CC;

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();
	auto safeArea = Director::getInstance()->getSafeAreaRect();
	auto safeOrigin = safeArea.origin;

	//_terain = Terain::createTerain(this);
	auto sampleScene = SampleScene::create();
	this->addChild(sampleScene);
	return true;
}