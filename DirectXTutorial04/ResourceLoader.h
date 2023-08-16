#pragma once

#include <DirectXTex.h>
#include <string>

#include <d3d11.h>
#include <Windows.h>

#include "Vector2f.h"
#include "Vector3f.h"

#include "Vertex.h"
#include "VertexUV.h"

// assimp ���̺귯��.
#include <Importer.hpp>
#include <cimport.h>
#include <postprocess.h>
#include <scene.h>

#include <vector>

class ResourceLoader
{
public:
	static ID3D11ShaderResourceView* LoadTextureFromFile(
		ID3D11Device* device,
		std::wstring filename
	);
	// �� �ε�.
	static void LoadModel(std::string filename, std::vector<Vertex>* vertices);
	static void LoadModel(std::string filename, std::vector<VertexUV>* vertice4);

	static std::wstring GetExtension(std::wstring str);

private:
	static HRESULT GetScratchImage(
		std::wstring filename,
		DirectX::ScratchImage* image
	);
};

