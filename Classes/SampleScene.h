#ifndef __SAMPLESCENE_H__
#define __SAMPLESCENE_H__

#include "cocos2d.h"
#include "renderer/backend/Device.h"

USING_NS_CC;

class SampleScene : public cocos2d::Node
{
private:
	Size winSize;

	CustomCommand _customCommand;
	float m_vertexDataCount;
	backend::UniformLocation _mvpMatrixLocation;
	backend::UniformLocation _colorLocation;
	backend::UniformLocation _textureLocation;

public:
	/*SampleScene();
	~SampleScene();*/

	static SampleScene* create();

	bool initWithNode();

	void generateTriangle();
	void new_way_to_generateTriangle();

	void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
};

#endif // !__SAMPLESCENE_H__
