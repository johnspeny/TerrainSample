#include "Terain.h"
#include "HelloWorldScene.h"

Terain::Terain()
{
	_stripes = NULL;
	_offsetX = 0;
	_fromKeyPointI = 0;
	_toKeyPointI = 0;
}

Terain::~Terain()
{
	CC_SAFE_RELEASE_NULL(_stripes);
}

bool Terain::init(HelloWorld* _scene)
{
	if (!Node::init())
	{
		return false;
	}

	_scene->addChild(this);

	winSize = Director::getInstance()->getWinSize();

	drawCircleNode = DrawNode::create();
	this->addChild(drawCircleNode);

	this->generateHills();
	this->resetHillVertices();

	/// <summary>
	///
	/// </summary>
	/// <param name="_scene"></param>
	/// <returns></returns>

	const char* vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec2 aPos;\n"
		"layout (location = 1) in vec2 a_texcoord;\n"
		"out vec2 TexCoord;\n"
		"uniform mat4 u_MVPMatrix;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = u_MVPMatrix * vec4(aPos, 0.0, 1.0);\n"
		"   TexCoord = a_texcoord;\n"
		"}\0";

	const char* fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec2 TexCoord;\n"
		"uniform sampler2D ourTexture;\n"
		"void main()\n"
		"{\n"
		"	FragColor = texture(ourTexture, TexCoord);\n"
		"}\0";

	//auto program = cocos2d::backend::Device::getInstance()->newProgram(vertexShaderSource, fragmentShaderSource);
	auto program = backend::Program::getBuiltinProgram(backend::ProgramType::POSITION_TEXTURE_COLOR);

	auto& pipelineDescriptor = _customCommand.getPipelineDescriptor();
	_programState = new (std::nothrow) backend::ProgramState(program);
	pipelineDescriptor.programState = _programState;

	auto& vertexLayout = pipelineDescriptor.programState->getVertexLayout();
	const auto& attributeInfo = _programState->getProgram()->getActiveAttributes();
	auto iter = attributeInfo.find("a_position");
	if (iter != attributeInfo.end())
	{
		vertexLayout->setAttribute("a_position",
			iter->second.location,
			backend::VertexFormat::FLOAT2,
			0,
			false);
	}

	iter = attributeInfo.find("a_texCoord");
	if (iter != attributeInfo.end())
	{
		vertexLayout->setAttribute("a_texCoord",
			iter->second.location,
			backend::VertexFormat::FLOAT2,
			2 * sizeof(float),
			false);
	}

	iter = attributeInfo.find("a_color");
	if (iter != attributeInfo.end())
	{
		vertexLayout->setAttribute("a_color",
			iter->second.location,
			backend::VertexFormat::UBYTE4,
			4 * sizeof(float),
			true);
	}

	vertexLayout->setLayout(8 * sizeof(float));

	/*
	@brief get uniforms from program state
	*/
	_mvpMatrixLocation = pipelineDescriptor.programState->getUniformLocation("u_MVPMatrix");
	_textureLocation = pipelineDescriptor.programState->getUniformLocation("u_tex0");

	/*	90, 30,
	size.width - 90, 30,
	size.width / 2, size.width - 180
*/

	float vertices[] = {
		// positions
		90.0f, 30.0f,
		90.0f, 60.0f,
		180.0f, 180.0f
	};

	float texV[] = {
		90.0f, 30.0f,
		90.0f, 60.0f,
		180.0f, 180.0f
	};

	//CCLOG("log it %d", sizeof(vertices));

	/*
	@brief generate a buffer glGenBuffer(1'how many we need', &buffer'where to store the buffer')
			glBindBuffer(GL_ARRAY_BUFFER, buffer) which is binding
			sizeOfVertex : total vertices count in bytes
			capacity: which is vertext count
	*/
	size_t hCount = (sizeof(vertices) / (sizeof(float) * 2));

	CCLOG("hCount %d", hCount);

	//_customCommand.createVertexBuffer(sizeof(vertices), hCount, CustomCommand::BufferUsage::DYNAMIC);
	_customCommand.createVertexBuffer(sizeof(V2F_C4B_T2F), hCount, CustomCommand::BufferUsage::DYNAMIC);

	//int tCount = sizeof(_hillTexCoords) / 2;
	////m_vertexDataCount = sizeof(_hillTexCoords) / 2;
	//_customCommand.createVertexBuffer(sizeof(_hillTexCoords), tCount, CustomCommand::BufferUsage::DYNAMIC);

	m_vertexDataCount = hCount;
	/*
	@brief glBufferData(data, size)
	*/
	_customCommand.updateVertexBuffer(vertices, sizeof(vertices));
	//_customCommand.updateVertexBuffer(_hillTexCoords, sizeof(_hillTexCoords));
	//_customCommand.updateVertexBuffer(vertices, 0, sizeof(vertices));
	//_customCommand.updateVertexBuffer(vertices, 3 * sizeof(float), sizeof(vertices));

	/*
	@brief update uniforms
	*/

	const auto& projectionMat = Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
	_customCommand.getPipelineDescriptor().programState->setUniform(_mvpMatrixLocation, projectionMat.m, sizeof(projectionMat.m));

	/*
	@brief update colors
	*/
	//_customCommand.getPipelineDescriptor().programState->setUniform(_colorLocation, "vec3(1.0, 0.0, 0.0)", 3 * sizeof(float));

	/*
	@brief texture rendering on shape
	*/

	auto _texture = Director::getInstance()->getTextureCache()->addImage("lowres-desert-ground.png");
	_customCommand.getPipelineDescriptor().programState->setUniform(_textureLocation, &_texture, sizeof(_texture));
	_customCommand.getPipelineDescriptor().programState->setTexture(_textureLocation, 0, _texture->getBackendTexture());

	/*
	@brief: draw type
		_customCommand.setPrimitiveType(CustomCommand::PrimitiveType::TRIANGLE_STRIP);
	*/
	_customCommand.setDrawType(CustomCommand::DrawType::ARRAY);
	_customCommand.setPrimitiveType(CustomCommand::PrimitiveType::TRIANGLE);

	/// <summary>
	///
	/// </summary>
	/// <param name="_scene"></param>
	/// <returns></returns>

	return true;
}

Terain* Terain::createTerain(HelloWorld* _scene)
{
	auto* terain = new Terain();
	if (terain && terain->init(_scene))
	{
		terain->autorelease();
	}
	else {
		CC_SAFE_DELETE(terain);
	}
	return terain;
}

void Terain::generateHills()
{
	/*
	Size winSize = Director::getInstance()->getWinSize();
	float x = 0;
	float y = winSize.height / 2;
	for (int i = 0; i < kMaxHillKeyPoints; ++i)
	{
		_hillKeyPoints[i] = Vec2(x, y);
		x += winSize.width / 2;
		y = rand() % (int)winSize.height;
	}*/

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

void Terain::draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
	_customCommand.init(0);
	_customCommand.setVertexDrawInfo(0, m_vertexDataCount);
	//log("drawn");
	renderer->addCommand(&_customCommand);

	//Node::draw();
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

	// new
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