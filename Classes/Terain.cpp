#include "Terain.h"

USING_NS_AX;

Terain::Terain() :
	_offsetX(0),
	_fromKeyPointI(0),
	_toKeyPointI(0)
{
}

Terain::~Terain()
{
}

Terain* Terain::create()
{
	auto* obj = new Terain();
	if (obj->init())
	{
		obj->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(obj);
	}
	return obj;
}

bool Terain::init()
{
	if (!Node::init())
	{
		return false;
	}

	// get window/screen size (1280, 720)
	winSize = Director::getInstance()->getWinSize();

	// circle
	drawCircleNode = DrawNode::create();
	this->addChild(drawCircleNode);

	// generate hill points
	this->generateHills();

	// only draw triangles on hill points that are visible on window/screen
	// generate vertices to draw triangles
	this->resetHillVertices();

	// modern way to draw triangle from the vertices stored
	this->modern_way_to_generateTriangle();

	log("Hello");

	return true;
}

void Terain::draw(axis::Renderer* renderer, const axis::Mat4& transform, uint32_t flags)
{
	// draw hill points and lines connecting the hill points
	glPushMatrix();
	glLoadMatrixd((GLdouble*)&transform);
	drawCircleNode->clear();
	for (int i = MAX(_fromKeyPointI, 1); i <= _toKeyPointI; ++i)
	{
		drawCircleNode->drawLine(_hillKeyPoints[i - 1], _hillKeyPoints[i], Color4F::RED);

		// new
		Point p0 = _hillKeyPoints[i - 1];
		Point p1 = _hillKeyPoints[i];
		int hSegments = floorf((p1.x - p0.x) / kHillSegmentWidth);
		float dx = (p1.x - p0.x) / hSegments;
		float da = M_PI / hSegments;
		float ymid = (p0.y + p1.y) / 2;
		float ampl = (p0.y - p1.y) / 2;

		Point pt0, pt1;
		pt0 = p0;
		for (int j = 0; j < hSegments + 1; ++j)
		{
			pt1.x = p0.x + j * dx;
			pt1.y = ymid + ampl * cosf(da * j);

			drawCircleNode->drawLine(pt0, pt1, Color4F::GREEN);
			pt0 = pt1;
		}
	}
	glPopMatrix();
}

void Terain::generateHills()
{
	// algo to generate hill points and store in _hillKeyPoints array

	float minDX = 160;
	int rangeDX = 80; // from 160 to 240
	float minDY = 60;
	int rangeDY = 40; // from 60 to 100

	float x = -minDX;
	float y = winSize.height / 2;

	float dy, ny;

	float sign = 1; // +1: going up, -1: going down
	float paddingTop = 20;
	float paddingBottom = 20;

	for (int i = 0; i < kMaxHillKeyPoints; ++i)
	{
		_hillKeyPoints[i] = Point(x, y);

		if (i == 0)
		{
			x = 0;
			y = winSize.height / 2;
		}
		else
		{
			x += rand() % rangeDX + minDX;
			while (true)
			{
				dy = rand() % rangeDY + minDY;
				ny = y + dy * sign;
				if ((sign<0 && (ny + minDY)>(winSize.height - paddingTop))
					|| (sign > 0 && (ny - minDY) < paddingBottom))
					continue;
				if (ny<(winSize.height - paddingTop) && ny >paddingBottom)
					break;
			}
			y = ny;
		}
		sign *= -1;
	}
}

void Terain::resetHillVertices()
{
	static  int prevFromKeyPointI = -1;
	static  int prevToKeyPointI = -1;

	// key points interval for drawing
	while (_hillKeyPoints[_fromKeyPointI + 1].x < _offsetX - winSize.width / 8 / this->getScale())
	{
		_fromKeyPointI++;
	}
	while (_hillKeyPoints[_toKeyPointI].x < _offsetX + winSize.width * 9 / 8 / this->getScale())
	{
		_toKeyPointI++;
	}

	// new
	if (prevFromKeyPointI != _fromKeyPointI || prevToKeyPointI != _toKeyPointI)
	{
		// vertices for visible area
		_nHillVertices = 0;
		_nBorderVertices = 0;
		Point p0, p1, pt0, pt1;
		p0 = _hillKeyPoints[_fromKeyPointI];
		for (int i = _fromKeyPointI + 1; i < _toKeyPointI + 1; ++i)
		{
			p1 = _hillKeyPoints[i];

			// triangle strip between p0 and p1
			int hSegments = floorf((p1.x - p0.x) / kHillSegmentWidth);
			float dx = (p1.x - p0.x) / hSegments;
			float da = M_PI / hSegments;
			float ymid = (p0.y + p1.y) / 2;
			float ampl = (p0.y - p1.y) / 2;
			pt0 = p0;
			_borderVertices[_nBorderVertices++] = pt0;
			for (int j = 1; j < hSegments + 1; ++j)
			{
				pt1.x = p0.x + j * dx;
				pt1.y = ymid + ampl * cosf(da * j);
				_borderVertices[_nBorderVertices++] = pt1;

				_hillVertices[_nHillVertices] = Vec2(pt0.x, 0);
				_hillTexCoords[_nHillVertices++] = Vec2(pt0.x / 512, 1.0f);
				_hillVertices[_nHillVertices] = Vec2(pt1.x, 0);
				_hillTexCoords[_nHillVertices++] = Vec2(pt1.x / 512, 1.0f);

				_hillVertices[_nHillVertices] = Vec2(pt0.x, pt0.y);
				_hillTexCoords[_nHillVertices++] = Vec2(pt0.x / 512, 0);
				_hillVertices[_nHillVertices] = Vec2(pt1.x, pt1.y);
				_hillTexCoords[_nHillVertices++] = Vec2(pt1.x / 512, 0);

				pt0 = pt1;
			}

			p0 = p1;
		}

		prevFromKeyPointI = _fromKeyPointI;
		prevToKeyPointI = _toKeyPointI;
	}
}

void Terain::modern_way_to_generateTriangle()
{
	// Triangle
	auto renderer = MeshRenderer::create();

	int perVertexSizeInFloat = 7;  // 3+4+2

	IndexArray indices;
	indices.clear(CustomCommand::IndexFormat::U_INT);

	auto InsertVertex = [perVertexSizeInFloat, &indices, this](float x, float y, float z, Color4F col)
	{
		unsigned int startindex = vertices.size() / perVertexSizeInFloat;
		vertices.insert(vertices.end(), {
			x,y,z, col.r,col.g,col.b,col.a,
			});

		indices.insert<uint32_t>(indices.size(),
			ilist_u32_t{ startindex });
	};

	size_t n = sizeof(_hillVertices) / sizeof(_hillVertices[0]);

	for (int i = 0; i < n; i++)
	{
		InsertVertex(_hillVertices[i].x, _hillVertices[i].y, 0, { 1,1,1,1 });
	}

	std::vector<MeshVertexAttrib> attribs;
	MeshVertexAttrib att;

	att.type = backend::VertexFormat::FLOAT3;
	att.vertexAttrib = shaderinfos::VertexKey::VERTEX_ATTRIB_POSITION;
	attribs.push_back(att);

	att.type = backend::VertexFormat::FLOAT4;
	att.vertexAttrib = shaderinfos::VertexKey::VERTEX_ATTRIB_COLOR;
	attribs.push_back(att);

	/*att.type = backend::VertexFormat::FLOAT2;
	att.vertexAttrib = shaderinfos::VertexKey::VERTEX_ATTRIB_TEX_COORD;
	attribs.push_back(att);*/

	mesh = Mesh::create(vertices, perVertexSizeInFloat, indices, attribs);

	auto mat = MeshMaterial::createBuiltInMaterial(MeshMaterial::MaterialType::QUAD_COLOR, false);
	mat->setPrimitiveType(backend::PrimitiveType::TRIANGLE_STRIP);
	mesh->setMaterial(mat);
	//mesh->setTexture("lowres-desert-ground.png");

	renderer->addMesh(mesh);

	addChild(renderer);
}

void Terain::setOffsetX(float newOffsetX)
{
	_offsetX = newOffsetX;
	this->setPosition(Point(-_offsetX * this->getScale(), 0));
	this->resetHillVertices();

	// Updating the vertex data, but does not work
	mesh->getMeshIndexData()->getVertexBuffer()->updateData(vertices.data(), vertices.size());
}