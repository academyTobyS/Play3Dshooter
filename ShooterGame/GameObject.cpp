#include "GameObject.h"

using namespace Play3d;

GameObject::GameObject( GameObjectType objType, Vector3f position)
{
	m_type = objType;
	m_pos = position;
}

void GameObject::SetSprite( std::string spriteName, float animationSpeed )
{
	//m_spriteID = Play::GetSpriteId( spriteName.c_str() );
	m_animSpeed = animationSpeed;
};

// Code adapted from Play::IsColliding
bool GameObject::IsColliding( GameObject* obj )
{
	//Don't collide with NULL objects
	if( m_type == -1 || obj->m_type == -1 )
		return false;

	int xDiff = (int)m_pos.x- (int)obj->m_pos.x;
	int yDiff = (int)m_pos.y - (int)obj->m_pos.y;
	int radii = (int)m_radius + (int)obj->m_radius;

	// Game programmers don't do square root!
	return((xDiff * xDiff) + (yDiff * yDiff) < radii * radii);
}

void GameObject::Draw() const
{
	Matrix4x4f transform = 
		  MatrixTranslate<f32>(m_pos.x, m_pos.y, m_pos.z)
		* MatrixRotationX<f32>(m_rotation.x)
		* MatrixRotationY<f32>(m_rotation.y)
		* MatrixRotationZ<f32>(m_rotation.z);

	Graphics::SetMaterial(m_materialId);
	Graphics::DrawMesh(m_meshId, transform);
}

void GameObject::StandardMovementUpdate()
{
	m_rotation += m_rotSpeed;
	m_oldPos = m_pos;
	m_velocity += m_acceleration;
	m_pos += m_velocity;
}

void GameObject::Destroy( )
{
	m_destroy = true;
}

void GameObject::UpdateAnimation()
{
	m_frameTimer += m_animSpeed;

	if( m_frameTimer >= 1 )
	{
		m_frame++;
		m_frameTimer = 0;
	}
}

Graphics::MeshId GameObject::AssignMesh(Graphics::MeshId& rId, const char* filepath)
{
	if (rId.IsInvalid())
	{
		rId = Graphics::CreateMeshFromObjFile(filepath, Colour::White, 0.25f);
	}
	return rId;
}

Graphics::MaterialId GameObject::AssignMaterial(Graphics::MaterialId& rId, const char* filepath)
{
	if (rId.IsInvalid())
	{
		Graphics::SimpleMaterialDesc desc;
		desc.m_state.m_cullMode = Graphics::CullMode::BACK;
		desc.m_state.m_fillMode = Graphics::FillMode::SOLID;
		Colour::White.as_float_rgba_srgb(&desc.m_constants.diffuseColour.x);
		desc.m_bEnableLighting = true;
		desc.m_lightCount = 3;

		// We can optionally use textures.
		if(filepath != "")
		{
			desc.m_texture[0] = Graphics::CreateTextureFromFile(filepath);
			desc.m_sampler[0] = Graphics::CreateLinearSampler();
		}

		rId = Resources::CreateAsset<Graphics::Material>(desc);
	}
	return rId;
}

Graphics::MaterialId GameObject::AssignMaterial(Play3d::Graphics::MaterialId& rId, const char* vertShaderFilepath, const char* fragShaderFilepath, const char* textureFilepath)
{
	if (rId.IsInvalid())
	{
		using namespace Graphics;
		ShaderId customPixelShader;
		{
			static const char* hlslCode = R"(
			struct PSInput
			{
				float4 position : SV_POSITION;
				float4 colour : COLOUR;
			};
			float4 PS_Main(PSInput input) : SV_TARGET
			{
				return float4(input.position.x * 0.001, input.position.y*0.001, 0.5, 1.0);
			})";

			ShaderCompilerDesc compilerOptions = {};
			compilerOptions.m_name = "Example PS Shader";
			compilerOptions.m_type = Graphics::ShaderType::PIXEL_SHADER;
			compilerOptions.m_hlslCode = hlslCode;
			compilerOptions.m_entryPoint = "PS_Main";
			compilerOptions.m_defines.push_back({ "LIGHTS", "4" });
			customPixelShader = Shader::Compile(compilerOptions);
		}

		ComplexMaterialDesc desc = {};
		desc.m_state.m_cullMode = Graphics::CullMode::BACK;
		desc.m_state.m_fillMode = Graphics::FillMode::SOLID;
		desc.m_PixelShader = customPixelShader;

		return Resources::CreateAsset<Graphics::Material>(desc);
	}
}