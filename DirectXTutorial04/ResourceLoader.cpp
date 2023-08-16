#include "ResourceLoader.h"

#include <wrl.h>
using Microsoft::WRL::ComPtr;

ID3D11ShaderResourceView* ResourceLoader::LoadTextureFromFile(ID3D11Device* device, std::wstring filename)
{
	filename = std::wstring(L"..//res//textures//") + filename;

	// ��ũ��ġ �̹��� ����.
	DirectX::ScratchImage image;
	HRESULT result = GetScratchImage(filename, &image);

	// ���� �˻�.
	if (FAILED(result))
	{
		MessageBox(nullptr, L"��ũ��ġ �̹��� �ε� ����", L"����", 0);
		//throw std::exception("��ũ��ġ �̹��� �ε� ����");
		return nullptr;
	}

	// �ؽ��� ����.
	ComPtr<ID3D11Texture2D> texture;
	result = DirectX::CreateTexture(
		device,
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		(ID3D11Resource**)texture.GetAddressOf()
	);

	// ���� �˻�.
	if (FAILED(result))
	{
		MessageBox(nullptr, L"�ؽ�ó ���� ����", L"����", 0);
		//throw std::exception("�ؽ�ó ���� ����");
		return nullptr;
	}

	// ���̴� ���ҽ� �� ����.
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	D3D11_TEXTURE2D_DESC textureDesc;
	texture->GetDesc(&textureDesc);

	srvDesc.Format = textureDesc.Format;

	ID3D11ShaderResourceView* shaderResourceView;
	result = device->CreateShaderResourceView(
		texture.Get(),
		&srvDesc,
		&shaderResourceView
	);

	// ���� �˻�.
	if (FAILED(result))
	{
		MessageBox(nullptr, L"���̴� ���ҽ� �� ���� ����", L"����", 0);
		//throw std::exception("���̴� ���ҽ� �� ���� ����");
		return nullptr;
	}

	return shaderResourceView;
}

void ResourceLoader::LoadModel(std::string filename, std::vector<Vertex>* vertices)
{
	// ���ҽ� ��� �߰�.
	filename = std::string("..//res//models//") + filename;

	// �ɼ� ����.
	const aiScene* scene = aiImportFile(
		filename.c_str(),
		aiProcess_JoinIdenticalVertices |
		aiProcess_ValidateDataStructure |
		aiProcess_ImproveCacheLocality |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_GenUVCoords |
		aiProcess_TransformUVCoords |
		aiProcess_FindInstances |
		aiProcess_LimitBoneWeights |
		aiProcess_OptimizeMeshes |
		aiProcess_GenSmoothNormals |
		aiProcess_SplitLargeMeshes |
		aiProcess_Triangulate | // �������� �簢���̸�, �ﰢ���� �ɰ�. ���������� �� �׷�.
		aiProcess_ConvertToLeftHanded |
		aiProcess_SortByPType
	);

	// ����Ʈ.
	if (scene != nullptr)
	{
		const aiMesh* mesh = scene->mMeshes[0]; // ������ ���� �� ���� ���� �־, ���� ���ƾ� ��.
		vertices->reserve(mesh->mNumVertices);

		// ���� �迭 ä���.
		for (int ix = 0; ix < mesh->mNumVertices; ix++)
		{
			// ��ġ �о����
			Vector3f position = Vector3f(
				mesh->mVertices[ix].x,
				mesh->mVertices[ix].y,
				mesh->mVertices[ix].z
			);

			Vertex vertex = Vertex(position.x, position.y, position.z);
			vertices->push_back(vertex);
		}
	}
	else
	{
		MessageBox(nullptr, L"�� �ε� ����", L"����", 0);
		throw std::exception("�� �ε� ����");
	}
}

void ResourceLoader::LoadModel(std::string filename, std::vector<VertexUV>* vertices4)
{
	// ���ҽ� ��� �߰�.
	filename = std::string("..//res//models//") + filename;

	// �ɼ� ����.
	const aiScene* scene = aiImportFile(
		filename.c_str(),
		aiProcess_JoinIdenticalVertices |
		aiProcess_ValidateDataStructure |
		aiProcess_ImproveCacheLocality |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_GenUVCoords |
		aiProcess_TransformUVCoords |
		aiProcess_FindInstances |
		aiProcess_LimitBoneWeights |
		aiProcess_OptimizeMeshes |
		aiProcess_GenSmoothNormals |
		aiProcess_SplitLargeMeshes |
		aiProcess_Triangulate | // �������� �簢���̸�, �ﰢ���� �ɰ�. ���������� �� �׷�.
		aiProcess_ConvertToLeftHanded |
		aiProcess_SortByPType
	);

	// ����Ʈ.
	if (scene != nullptr)
	{
		const aiMesh* mesh = scene->mMeshes[0]; // ������ ���� �� ���� ���� �־, ���� ���ƾ� ��.
		vertices4->reserve(mesh->mNumVertices);

		// ���� �迭 ä���.
		for (int ix = 0; ix < mesh->mNumVertices; ix++)
		{
			// ��ġ �о����.
			Vector3f position = Vector3f(
				mesh->mVertices[ix].x,
				mesh->mVertices[ix].y,
				mesh->mVertices[ix].z
			);

			// UV �о����.
			Vector2f uv;
			if (mesh->HasTextureCoords(0))
			{
				uv = Vector2f(
					mesh->mTextureCoords[0][ix].x,
					mesh->mTextureCoords[0][ix].y
				);
			}
			else
			{
				uv = Vector2f(0.0f, 0.0f);
			}

			VertexUV vertex = VertexUV(position, uv);
			vertices4->push_back(vertex);
		}
	}
	else
	{
		MessageBox(nullptr, L"�� �ε� ����", L"����", 0);
		throw std::exception("�� �ε� ����");
	}
}

std::wstring ResourceLoader::GetExtension(std::wstring str)
{
	size_t size = str.rfind('.', str.length());
	if (size != std::wstring::npos)
	{
		return (str.substr(size + 1, str.length() - size));
	}
	return std::wstring();
}

HRESULT ResourceLoader::GetScratchImage(std::wstring filename, DirectX::ScratchImage* image)
{
	// ������ Ȯ���� ���ϱ� (ex: jpg/png/tga).
	std::wstring extension = GetExtension(filename);

	if (extension._Equal(L"tga") || extension._Equal(L"TGA"))
	{
		return DirectX::LoadFromTGAFile(filename.c_str(), NULL, *image);
	}
	else if (extension._Equal(L"dds") || extension._Equal(L"DDS"))
	{
		return DirectX::LoadFromDDSFile(filename.c_str(), DirectX::DDS_FLAGS_NONE, NULL, *image);
	}
	else // �ٸ� Ȯ���� ���ϵ�.
	{
		return DirectX::LoadFromWICFile(filename.c_str(), DirectX::WIC_FLAGS_NONE, NULL, *image);
	}

	return E_NOTIMPL;
}
