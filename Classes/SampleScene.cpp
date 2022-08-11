#include "SampleScene.h"

SampleScene* SampleScene::create()
{
	auto* obj = new SampleScene();
	if (obj->initWithNode())
	{
		obj->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(obj);
	}
	return obj;
}

bool SampleScene::initWithNode()
{
	if (!Node::init())
	{
		return false;
	}

	//generateTriangle();

	new_way_to_generateTriangle();

	return true;
}

void SampleScene::generateTriangle()
{
	const char* vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec2 a_position;\n"
		"layout (location = 1) in vec2 a_texCoord;\n"
		"layout (location = 2) in vec4 a_color;\n"
		"out vec4 ourColor;\n"
		"out vec2 TexCoord;\n"
		"uniform mat4 u_MVPMatrix;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = u_MVPMatrix * vec4(a_position, 0.0, 1.0);\n"
		"   ourColor = a_color;\n"
		"   TexCoord = a_texCoord;\n"
		"}\0";

	const char* fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec4 ourColor;\n"
		"in vec2 TexCoord;\n"
		"uniform sampler2D u_tex0;\n"
		"void main()\n"
		"{\n"
		"	FragColor = texture(u_tex0, TexCoord);\n"
		"}\0";

	auto program = cocos2d::backend::Device::getInstance()->newProgram(vertexShaderSource, fragmentShaderSource);

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

	// stride = 8 * sizeof(float)
	vertexLayout->setLayout(8 * sizeof(float));

	/*
	@brief get uniforms from program state
	*/
	_mvpMatrixLocation = pipelineDescriptor.programState->getUniformLocation("u_MVPMatrix");
	_textureLocation = pipelineDescriptor.programState->getUniformLocation("u_tex0");

	/*
	@brief vertex data
	*/
	float pos_Vertices[] = {
		// positions
		90.0f, 30.0f,
		120.0f, 60.0f,
		150.0f, 30.0f
	};

	float color_Vertices[] = {
		// color
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f
	};

	float texture_Vertices[] = {
		// texture
		90.0f, 30.0f,
		120.0f, 60.0f,
		150.0f, 30.0f
	};

	size_t pCount = (sizeof(pos_Vertices) / (sizeof(float) * 2));
	size_t colorCount = (sizeof(color_Vertices) / (sizeof(float) * 4));
	size_t textureCount = (sizeof(texture_Vertices) / (sizeof(float) * 2));

	// total vertex count from the vertex buffer ( 3 vertices for my case)
	m_vertexDataCount = 3;

	// glVertexAttribPointer part
	_customCommand.createVertexBuffer(sizeof(pos_Vertices), pCount, CustomCommand::BufferUsage::DYNAMIC);
	_customCommand.createVertexBuffer(sizeof(color_Vertices), colorCount, CustomCommand::BufferUsage::DYNAMIC);
	_customCommand.createVertexBuffer(sizeof(texture_Vertices), textureCount, CustomCommand::BufferUsage::DYNAMIC);

	/*
	@brief glBufferData(data, size)
	*/
	_customCommand.updateVertexBuffer(pos_Vertices, sizeof(pos_Vertices));
	_customCommand.updateVertexBuffer(color_Vertices, sizeof(color_Vertices));
	_customCommand.updateVertexBuffer(texture_Vertices, sizeof(texture_Vertices));

	/*
	@brief update uniforms
	*/
	const auto& projectionMat = Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
	_customCommand.getPipelineDescriptor().programState->setUniform(_mvpMatrixLocation, projectionMat.m, sizeof(projectionMat.m));

	/*
	@brief texture rendering on shape
	*/
	auto _texture = Director::getInstance()->getTextureCache()->addImage("lowres-desert-ground.png");
	_customCommand.getPipelineDescriptor().programState->setUniform(_textureLocation, &_texture, sizeof(_texture));
	_customCommand.getPipelineDescriptor().programState->setTexture(_textureLocation, 0, _texture->getBackendTexture());

	/*
	@brief: draw type
	*/
	_customCommand.setDrawType(CustomCommand::DrawType::ARRAY);
	_customCommand.setPrimitiveType(CustomCommand::PrimitiveType::TRIANGLE);
}

void SampleScene::new_way_to_generateTriangle()
{
	//auto renderer = RenderTexture;
}

void SampleScene::draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
	/*_customCommand.init(0);
	_customCommand.setVertexDrawInfo(0, m_vertexDataCount);
	renderer->addCommand(&_customCommand);*/
}