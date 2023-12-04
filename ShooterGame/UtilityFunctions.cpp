#include "UtilityFunctions.h"
#include <string>
#include <fstream>

using namespace Play3d;
using namespace Graphics;

float RandValueInRange(const float min, const float max)
{
	PLAY_ASSERT(min <= max);
	return min + ((max - min) * (static_cast<float>(rand()) / RAND_MAX));
}

Graphics::MeshId AssignMesh(Graphics::MeshId& rId, const char* filepath)
{
	if (rId.IsInvalid())
	{
		rId = Graphics::CreateMeshFromObjFile(filepath, Colour::White, 0.25f);
	}
	return rId;
}

// Create basic texture material
Graphics::MaterialId AssignMaterial(Graphics::MaterialId& rId, const char* filepath)
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
		if (filepath != "")
		{
			desc.m_texture[0] = Graphics::CreateTextureFromFile(filepath);
			desc.m_sampler[0] = Graphics::CreateLinearSampler();
		}

		rId = Resources::CreateAsset<Graphics::Material>(desc);
	}
	return rId;
}

static const float s_viewBoundsHalf{ 15.f / 2 };
float GetGameHalfWidth()
{
	Graphics::SurfaceSize surfaceSize = Graphics::GetDisplaySurfaceSize();
	f32 aspect((f32)surfaceSize.m_width / (f32)surfaceSize.m_height);
	return s_viewBoundsHalf * aspect;
}

float GetGameHalfHeight()
{
	return s_viewBoundsHalf;
}


Graphics::MaterialId AssignMaterialHLSL(Play3d::Graphics::MaterialId& rId, const char* hlslPath, const char* texturePath)
{
	if(rId.IsInvalid())
	{
		size_t fileSizeBytes;
		const char* hlslCode = (const char*)System::LoadFileData(hlslPath, fileSizeBytes);
		PLAY_ASSERT_MSG(hlslCode, "Could not load the shader file");

		char shaderName[64];

		Graphics::ShaderId customVertexShader;
		{
			sprintf_s(shaderName, 64, "%s_VS", hlslPath);
			Graphics::ShaderCompilerDesc compilerOptions = {};
			compilerOptions.m_name = shaderName;
			compilerOptions.m_type = Graphics::ShaderType::VERTEX_SHADER;
			compilerOptions.m_flags = (u32)Graphics::ShaderCompilationFlags::DEBUG;
			compilerOptions.m_hlslCode = hlslCode;
			compilerOptions.m_entryPoint = "VS_Main";
			compilerOptions.m_defines.push_back({ "MAX_LIGHTS", "4" });
			customVertexShader = Graphics::Shader::Compile(compilerOptions);
			PLAY_ASSERT_MSG(customVertexShader.IsValid(), "Vertex Shader Compilation Failed!");
		}

		Graphics::ShaderId customPixelShader;
		{
			sprintf_s(shaderName, 64, "%s_PS", hlslPath);
			Graphics::ShaderCompilerDesc compilerOptions = {};
			compilerOptions.m_name = shaderName;
			compilerOptions.m_type = Graphics::ShaderType::PIXEL_SHADER;
			compilerOptions.m_flags = (u32)Graphics::ShaderCompilationFlags::DEBUG;
			compilerOptions.m_hlslCode = hlslCode;
			compilerOptions.m_entryPoint = "PS_Main";
			compilerOptions.m_defines.push_back({ "MAX_LIGHTS", "4" });
			customPixelShader = Graphics::Shader::Compile(compilerOptions);
			PLAY_ASSERT_MSG(customVertexShader.IsValid(), "Pixel Shader Compilation Failed!");
		}

		System::ReleaseFileData(const_cast<char*>(hlslCode));

		Graphics::ComplexMaterialDesc desc;
		desc.m_state.m_cullMode = Graphics::CullMode::BACK;
		desc.m_state.m_fillMode = Graphics::FillMode::SOLID;
		desc.m_VertexShader = customVertexShader;
		desc.m_PixelShader = customPixelShader;

		if(texturePath != "")
		{
			desc.m_texture[0] = Graphics::CreateTextureFromFile(texturePath);
			desc.m_sampler[0] = Graphics::CreateLinearSampler();
		}

		rId = Resources::CreateAsset<Graphics::Material>(desc);
	}
	return rId;
}