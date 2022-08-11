#ifndef __TERAIN_H__
#define __TERAIN_H__

#include "cocos2d.h"
#include "renderer/backend/Device.h"
#include "renderer/backend/Buffer.h"

const int kMaxHillKeyPoints = 1000;
const int kHillSegmentWidth = 10;
const int kMaxHillVertices = 4000;
const int kMaxBorderVertices = 800;

class Terain : public axis::Node
{
private:
	axis::Mesh* mesh;
	std::vector<float> vertices;
	axis::DrawNode* drawCircleNode;
	axis::Size winSize;

	int _fromKeyPointI;
	int _toKeyPointI;
	int _offsetX;
	int _nHillVertices;

	// store hill points to use
	axis::Vec2 _hillKeyPoints[kMaxHillKeyPoints];

	// a buffer to store vertex data
	axis::Point _hillVertices[kMaxHillVertices];
	axis::Point _hillTexCoords[kMaxHillVertices];

	// for drawing box2d edge shape
	int _nBorderVertices;
	axis::Point _borderVertices[kMaxBorderVertices];

public:
	Terain();
	~Terain();

	static Terain* create();

	virtual bool init() override;
	virtual void draw(axis::Renderer* renderer, const axis::Mat4& transform, uint32_t flags) override;

	void generateHills();
	void resetHillVertices();

	void modern_way_to_generateTriangle();

	void setOffsetX(float newOffsetX);
};

#endif // !__TERAIN_H__
