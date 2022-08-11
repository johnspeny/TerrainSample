#ifndef __TERAIN_H__
#define __TERAIN_H__

#define kMaxHillKeyPoints 1000
#define kHillSegmentWidth  10

#define kMaxHillVertices  4000
#define kMaxBorderVertices  800

#include "cocos2d.h"
#include "renderer/backend/Device.h"

USING_NS_CC;

class HelloWorld;

class Terain : public cocos2d::Node
{
private:
	DrawNode* drawCircleNode;
	Size winSize;

	int _nHillVertices;
	cocos2d::Point _hillVertices[kMaxHillVertices];
	cocos2d::Point _hillTexCoords[kMaxHillVertices];
	int _nBorderVertices;
	cocos2d::Point _borderVertices[kMaxBorderVertices];

	CustomCommand _customCommand;
	float m_vertexDataCount;
	backend::UniformLocation _mvpMatrixLocation;
	backend::UniformLocation _colorLocation;
	backend::UniformLocation _textureLocation;

public:
	Terain();
	~Terain();

	static Terain* createTerain(HelloWorld* _newscene);

	virtual bool init(HelloWorld* _scene);

	Sprite* _stripes;

	void generateHills();

	int _fromKeyPointI;
	int _toKeyPointI;
	int _offsetX;
	Vec2 _hillKeyPoints[kMaxHillKeyPoints];

	void resetHillVertices();

	void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
};

#endif // !__TERAIN_H__
