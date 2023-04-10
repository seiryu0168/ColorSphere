#include<d3dcompiler.h>
#include "ColorChecker.h"
#include"Engine/GameObject/Camera.h"
#include"Engine/DirectX_11/Input.h"

ColorChecker::ColorChecker()
{

}

ColorChecker::~ColorChecker()
{
}
void ColorChecker::Initialize(int screenWidth, int screenHeight, HWND hWnd)
{
	dispatchX_ = 16;
	dispatchY_ = 16;
	imageWidth_ = Direct3D::GetScreenWidth();
	imageHeight_ = Direct3D::GetScreenHeight();
	HRESULT hr;
	DXGI_SWAP_CHAIN_DESC scDesc;

	//�Ƃ肠�����S��0
	ZeroMemory(&scDesc, sizeof(scDesc));

	//�`���̃t�H�[�}�b�g
	scDesc.BufferDesc.Width = screenWidth;		//��ʕ�
	scDesc.BufferDesc.Height = screenHeight;	//��ʍ���
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// ���F�g���邩

	//FPS�i1/60�b��1��j
	scDesc.BufferDesc.RefreshRate.Numerator = 60;
	scDesc.BufferDesc.RefreshRate.Denominator = 1;

	//���̑�
	scDesc.Windowed = TRUE;			//�E�B���h�E���[�h���t���X�N���[����
	scDesc.OutputWindow = hWnd;		//�E�B���h�E�n���h��
	scDesc.BufferCount = 1;			//�o�b�N�o�b�t�@�̖���
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//�o�b�N�o�b�t�@�̎g��������ʂɕ`�悷�邽�߂�
	scDesc.SampleDesc.Count = 1;		//MSAA�i�A���`�G�C���A�X�j�̐ݒ�
	scDesc.SampleDesc.Quality = 0;		//�@�V
	
	//�X���b�v�`�F�C���쐬
	hr=Direct3D::GetFactory()->CreateSwapChain(Direct3D::GetDevice(), &scDesc, &pChain_);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"�X���b�v�`�F�C���쐬���s", L"�G���[", MB_OK);
	}
	//�o�b�N�o�b�t�@�擾
	ID3D11Texture2D* pBackBuffer;
	hr=pChain_->GetBuffer(0,__uuidof(ID3D11Texture2D),(LPVOID*)&pBackBuffer);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"�o�b�N�o�b�t�@�擾���s", L"�G���[", MB_OK);
	}
	//�����_�[�^�[�Q�b�g�r���[�쐬
	hr=Direct3D::GetDevice()->CreateRenderTargetView(pBackBuffer, NULL, &pRTV_);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"�����_�[�^�[�Q�b�g�r���[�쐬���s", L"�G���[", MB_OK);
	}
	SAFE_RELEASE(pBackBuffer);
	//ID3DBlob* csBlob;
	hr=InitShader();
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"�R���s���[�g�V�F�[�_�[�̃R���p�C���Ɏ��s", L"�G���[", MB_OK);
	}

	D3D11_BUFFER_DESC cBufferDesc;
	cBufferDesc.ByteWidth = sizeof(CONSTANT_BUFFER);
	cBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cBufferDesc.MiscFlags = 0;
	cBufferDesc.StructureByteStride = 0;
	hr = Direct3D::GetDevice()->CreateBuffer(&cBufferDesc, nullptr, &pConstantBuffer_);	

	//hr = Direct3D::GetDevice()->CreateComputeShader(csBlob->GetBufferPointer(),
	//												csBlob->GetBufferSize(),
	//												nullptr,
	//												&pComputeShader);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"�V�F�[�_�[�쐬���s", L"�G���[", MB_OK);
	}

	if (FAILED(hr))
	{
		MessageBox(nullptr, L"�V�F�[�_�[�̃R���p�C�����s", L"�G���[", MB_OK);
	}
	//Direct3D::GetContext()->OMSetRenderTargets(1, &pRTV_, Direct3D::GetDepthStencilView());// �`����ݒ�
}

void ColorChecker::SetSampleBuffer(ID3D11Texture2D* buff)
{

}

void ColorChecker::ShaderDispatch(int imageWidth, int imageHeight)
{
	Direct3D::GetContext()->Dispatch((float)imageWidth / (float)dispatchX_, (float)imageHeight / (float)dispatchY_,1);
}

void ColorChecker::GetShaderResult(ID3D11Buffer* resultBuffer)
{

}

void ColorChecker::CalcPixel(ID3D11Texture2D* buff)	
{
	HRESULT hr;
	ID3D11SamplerState* smplSampler;
	D3D11_SAMPLER_DESC  SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = Direct3D::pDevice->CreateSamplerState(&SamDesc, &smplSampler);

	//�����_�����O���ʂ̃V�F�[�_�[���\�[�X�r���[���쐬
	ID3D11ShaderResourceView* smplSRV;
	//�V�F�[�_�[���\�[�X�r���[�̐ݒ�
	D3D11_SHADER_RESOURCE_VIEW_DESC smplSRVDesc;
	ZeroMemory(&smplSRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	smplSRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	smplSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	smplSRVDesc.Texture2D.MostDetailedMip = 0;
	smplSRVDesc.Texture2D.MipLevels = 1;
	//�󂯎�����o�b�t�@���e�N�X�`����
	ID3D11Texture2D* pTexture;
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Width = imageWidth_;
	texDesc.Height = imageHeight_;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texDesc.MiscFlags = 0;
	hr = Direct3D::GetDevice()->CreateTexture2D(&texDesc, nullptr, &pTexture);
	Direct3D::GetContext()->CopyResource(pTexture, buff);
	hr=Direct3D::GetDevice()->CreateShaderResourceView(pTexture, &smplSRVDesc, &smplSRV);

	//UAV�p�̕ϐ��o�b�t�@�̐ݒ�
	D3D11_BUFFER_DESC UAVBufferDesc;
	ZeroMemory(&UAVBufferDesc, sizeof(D3D11_BUFFER_DESC));
	UAVBufferDesc.ByteWidth = sizeof(IMAGE_DATA) * imageWidth_ * imageHeight_;
	UAVBufferDesc.Usage = D3D11_USAGE_DEFAULT;//�o�b�t�@�̓ǂݎ����@���w��
											  //����̏ꍇGPU�ɂ�鏑�����݂Ɠǂݍ��݂�����
	UAVBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS|//UAV�ƃV�F�[�_�[���\�[�X��
							  D3D11_BIND_SHADER_RESOURCE; //�o�C���h�t���O�𗧂ĂȂ��ƃo�b�t�@�쐬�����s����
	UAVBufferDesc.CPUAccessFlags = 0;//GPU���炵���A�N�Z�X���Ȃ��̂ł���
	UAVBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;//�\�������ꂽ�o�b�t�@�����ꍇ����
	UAVBufferDesc.StructureByteStride = sizeof(IMAGE_DATA);//�\�����o�b�t�@�̗v�f�̃T�C�Y
	
	//�V�F�[�_�[����̏o�͂��󂯎��z��p�ӂ��邽�߂̃o�b�t�@
	ID3D11Buffer* pOutputBuffer = nullptr;
	hr=Direct3D::GetDevice()->CreateBuffer(&UAVBufferDesc, NULL, &pOutputBuffer);
	
	//�V�F�[�_�[����̏o�͂��󂯎��z(UAV:UnorderedAccessView)
	ID3D11UnorderedAccessView* UAV;
	//UAV�̐ݒ�
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;		//UAV�̃��\�[�X�f�[�^�`���BDXGI_FORMAT���t�@�����X�Q��
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;//�o�͂��󂯎��o�b�t�@�Ƃ��Ďg���̂ł���
	UAVDesc.Buffer.FirstElement = 0;
	UAVDesc.Buffer.NumElements = imageWidth_ * imageHeight_;//�f�[�^�̗v�f��
	UAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;//�t�H�[�}�b�g�ɂ���ĕς��B
														 //�ڂ�����D3D_BUFFER_UAV_FLAG���t�@�����X�Q��
	//UAV�쐬
	hr=Direct3D::GetDevice()->CreateUnorderedAccessView(pOutputBuffer, &UAVDesc, &UAV);

	//////////////////��������̓V�F�[�_�[�ɐݒ肷��Ƃ���///////////////////////

	//�V�F�[�_�[���\�[�X�r���[�ݒ�
	Direct3D::GetContext()->CSSetShaderResources(0, 1, &smplSRV);
	Direct3D::GetContext()->CSSetSamplers(0, 1, &smplSampler);
	//�쐬�����R���s���[�g�V�F�[�_��ݒ�
	Direct3D::GetContext()->CSSetShader(pComputeShader, nullptr, 0);
	//UAV��ݒ�
	Direct3D::GetContext()->CSSetUnorderedAccessViews(0, 1, &UAV,0);
	//���s
	ShaderDispatch(imageWidth_, imageHeight_);

	///////////�V�F�[�_�[����̃f�[�^���󂯎��Ƃ���
	
	//�󂯎��o�b�t�@���
	ID3D11Buffer* outBuff = nullptr;
	//�o�b�t�@�̐ݒ�
	D3D11_BUFFER_DESC bufDesc;
	pOutputBuffer->GetDesc(&bufDesc);
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;//CPU����ǂ߂�悤�Ƀt���O�𗧂Ă�
	bufDesc.Usage = D3D11_USAGE_STAGING;		   //GPU����CPU�ւ̃f�[�^�]�����邽�߂̃t���O
	bufDesc.BindFlags = 0;						   
	bufDesc.MiscFlags = 0;

	hr=Direct3D::GetDevice()->CreateBuffer(&bufDesc, nullptr, &outBuff);
	Direct3D::GetContext()->CopyResource(outBuff, pOutputBuffer);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	Direct3D::GetContext()->Map(outBuff, 0, D3D11_MAP_READ, 0, &mappedResource);
	IMAGE_DATA* p = reinterpret_cast<IMAGE_DATA*>(mappedResource.pData);
	size_t dataSize = (size_t)(imageWidth_ * imageHeight_);
	pPixelData_ = new IMAGE_DATA[dataSize];
	memcpy_s(pPixelData_, sizeof(IMAGE_DATA) *dataSize, p, sizeof(IMAGE_DATA) * dataSize);
	Direct3D::GetContext()->Unmap(outBuff, 0);
	
	D3D11_BUFFER_DESC vDesc;
	vDesc.ByteWidth = sizeof(IMAGE_DATA) * dataSize;
	vDesc.Usage = D3D11_USAGE_DEFAULT;
	vDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vDesc.CPUAccessFlags = 0;
	vDesc.MiscFlags = 0;
	vDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vData;
	vData.pSysMem = pPixelData_;

	Direct3D::GetDevice()->CreateBuffer(&vDesc, &vData, &pVertexBuffer_);

	//for (size_t i = 0; i < imageWidth_ * imageHeight_; i++)
	//{
	//	XMFLOAT3 Col = pPixelData_[i].color;
	//	int o = 0;
	//}
	//SAFE_DELETE(pPixelData_);
	SAFE_RELEASE(smplSampler);
	SAFE_RELEASE(pTexture);
	SAFE_RELEASE(smplSRV);
	SAFE_RELEASE(pOutputBuffer);
	SAFE_RELEASE(UAV);
	SAFE_RELEASE(outBuff);	
	SAFE_DELETE(smplSampler);
	SAFE_DELETE(pTexture);
	SAFE_DELETE(smplSRV);
	SAFE_DELETE(pOutputBuffer);
	SAFE_DELETE(UAV);
	SAFE_DELETE(outBuff);
}
	

HRESULT ColorChecker::InitComputeShader(LPCWSTR fileName, LPCSTR entryPoint, ID3D11Device* device, ID3DBlob** blob)
{
	if (!fileName || !entryPoint || !device || !blob)
	{
		return E_INVALIDARG;
	}
	UINT flag = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef DEBUG
	flag |= D3DCOMPILE_DEBUG;
#endif // DEBUG

	*blob = nullptr;
	ID3DBlob* shaderBlob;
	ID3DBlob* errorBlob;
	LPCSTR profile;
	if (Direct3D::GetDevice()->GetFeatureLevel())
	{
		profile = "cs_5_0";
	}
	else
	{
		profile = "cs_4_0";
	}

	const D3D_SHADER_MACRO defines[] = { "EXAMPLE_DEFINE","1",NULL,NULL};
	//�V�F�[�_�[�R���p�C��
	HRESULT hr=D3DCompileFromFile(fileName, 
								  defines, 
								  D3D_COMPILE_STANDARD_FILE_INCLUDE, 
								  entryPoint,
								  profile,
								  flag, 
								  0,
								  &shaderBlob,
								  &errorBlob);

	
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		if (shaderBlob)
		{
			shaderBlob->Release();
		}

		return hr;
	}
	*blob = shaderBlob;
	
	return hr;
}

HRESULT ColorChecker::InitVertexShader(LPCWSTR fileName, LPCSTR entryPoint, ID3D11Device* device, ID3DBlob** blob)
{
	if (!fileName || !entryPoint || !device || !blob)
	{
		return E_INVALIDARG;
	}
	UINT flag = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef DEBUG
	flag |= D3DCOMPILE_DEBUG;
#endif // DEBUG

	* blob = nullptr;
	ID3DBlob* shaderBlob;
	ID3DBlob* errorBlob;
	LPCSTR profile;
	if (Direct3D::GetDevice()->GetFeatureLevel())
	{
		profile = "vs_5_0";
	}
	else
	{
		profile = "vs_4_0";
	}

	const D3D_SHADER_MACRO defines[] = { "EXAMPLE_DEFINE","1",NULL,NULL };
	//�V�F�[�_�[�R���p�C��
	HRESULT hr = D3DCompileFromFile(fileName,
		defines,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint,
		profile,
		flag,
		0,
		&shaderBlob,
		&errorBlob);


	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		if (shaderBlob)
		{
			shaderBlob->Release();
		}

		return hr;
	}
	*blob = shaderBlob;

	return hr;
}

HRESULT ColorChecker::InitGeometryShader(LPCWSTR fileName, LPCSTR entryPoint, ID3D11Device* device, ID3DBlob** blob)
{
	if (!fileName || !entryPoint || !device || !blob)
	{
		return E_INVALIDARG;
	}
	UINT flag = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef DEBUG
	flag |= D3DCOMPILE_DEBUG;
#endif // DEBUG

	* blob = nullptr;
	ID3DBlob* shaderBlob;
	ID3DBlob* errorBlob;
	LPCSTR profile;
	if (Direct3D::GetDevice()->GetFeatureLevel())
	{
		profile = "gs_5_0";
	}
	else
	{
		profile = "gs_4_0";
	}

	const D3D_SHADER_MACRO defines[] = { "EXAMPLE_DEFINE","1",NULL,NULL };
	//�V�F�[�_�[�R���p�C��
	HRESULT hr = D3DCompileFromFile(fileName,
		defines,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint,
		profile,
		flag,
		0,
		&shaderBlob,
		&errorBlob);


	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		if (shaderBlob)
		{
			shaderBlob->Release();
		}

		return hr;
	}
	*blob = shaderBlob;

	return hr;
}

HRESULT ColorChecker::InitPixelShader(LPCWSTR fileName, LPCSTR entryPoint, ID3D11Device* device, ID3DBlob** blob)
{
	if (!fileName || !entryPoint || !device || !blob)
	{
		return E_INVALIDARG;
	}
	UINT flag = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef DEBUG
	flag |= D3DCOMPILE_DEBUG;
#endif // DEBUG

	* blob = nullptr;
	ID3DBlob* shaderBlob;
	ID3DBlob* errorBlob;
	LPCSTR profile;
	if (Direct3D::GetDevice()->GetFeatureLevel())
	{
		profile = "ps_5_0";
	}
	else
	{
		profile = "ps_4_0";
	}

	const D3D_SHADER_MACRO defines[] = { "EXAMPLE_DEFINE","1",NULL,NULL };
	//�V�F�[�_�[�R���p�C��
	HRESULT hr = D3DCompileFromFile(fileName,
		defines,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint,
		profile,
		flag,
		0,
		&shaderBlob,
		&errorBlob);


	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		if (shaderBlob)
		{
			shaderBlob->Release();
		}

		return hr;
	}
	*blob = shaderBlob;

	return hr;
}

HRESULT ColorChecker::InitShader()
{
	HRESULT hr;
	ID3DBlob* csBlob;
	ID3DBlob* vsBlob;
	//ID3DBlob* gsBlob;
	ID3DBlob* psBlob;
	hr = InitComputeShader(L"ComputeShader.hlsl", "CS", Direct3D::GetDevice(), &csBlob);
	hr = InitVertexShader(L"ColorSphere.hlsl", "VS", Direct3D::GetDevice(), &vsBlob);
	//hr = InitGeometryShader(L"ComputeShader.hlsl", "GS", Direct3D::GetDevice(), &gsBlob);
	hr = InitPixelShader(L"ColorSphere.hlsl", "PS", Direct3D::GetDevice(), &psBlob);

	if (FAILED(hr))
		return hr;

	hr = Direct3D::GetDevice()->CreateComputeShader(csBlob->GetBufferPointer(),
													csBlob->GetBufferSize(),
													nullptr,
													&pComputeShader);
	hr = Direct3D::GetDevice()->CreateVertexShader(vsBlob->GetBufferPointer(),
												   vsBlob->GetBufferSize(),
												   nullptr,
												   &pVertexShader_);
	//hr = Direct3D::GetDevice()->CreateGeometryShader(gsBlob->GetBufferPointer(),
	//												 gsBlob->GetBufferSize(),
	//												 nullptr,
	//												 &pGeometryShader_);
	hr = Direct3D::GetDevice()->CreatePixelShader(psBlob->GetBufferPointer(),
												  psBlob->GetBufferSize(),
												  nullptr,
												  &pPixelShader_);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"�V�F�[�_�[�̍쐬�Ɏ��s", L"�G���[", MB_OK);
		return hr;
	}

	D3D11_INPUT_ELEMENT_DESC layout[] = { 
			{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 } };

	hr = Direct3D::GetDevice()->CreateInputLayout(layout,
												  sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC),
												  vsBlob->GetBufferPointer(),
												  vsBlob->GetBufferSize(),
												  &pLayout_);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"�C���v�b�g���C�A�E�g�̍쐬�Ɏ��s", L"�G���[", MB_OK);
		return hr;
	}

	D3D11_RASTERIZER_DESC rDesc;
	rDesc.CullMode = D3D11_CULL_NONE;
	rDesc.FillMode = D3D11_FILL_SOLID;
	rDesc.FrontCounterClockwise = TRUE;
	hr = Direct3D::GetDevice()->CreateRasterizerState(&rDesc, &pRasterizer_);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"���X�^���C�U�̍쐬�Ɏ��s", L"�G���[", MB_OK);
		return hr;
	}

	SAFE_RELEASE(psBlob);
	SAFE_RELEASE(vsBlob);
	//SAFE_RELEASE(gsBlob);
	SAFE_RELEASE(csBlob);
	return hr;
}


void ColorChecker::Update()
{
	if (Input::IsMouseButton(0))
	{
		XMFLOAT3 rotate = StoreFloat3(Input::GetMousePosition());
		transform.rotate_.y = rotate.x;
	}

	for (size_t i = 0; i < imageWidth_ * imageHeight_; i++)
	{
		XMFLOAT3 Col=pPixelData_[i].color;
		int o = 0;
	}
	SAFE_DELETE(pPixelData_);
}

void ColorChecker::DrawStart()
{
	Direct3D::GetContext()->OMSetRenderTargets(1, &pRTV_, Direct3D::GetDepthStencilView());
	float clearColor[4] = { 0,0,0,1 };
	Direct3D::GetContext()->ClearRenderTargetView(pRTV_, clearColor);
	Direct3D::GetContext()->ClearDepthStencilView(Direct3D::GetDepthStencilView(),
												  D3D11_CLEAR_DEPTH,
												  1.0f,
												  0);
}

void ColorChecker::Draw()
{
	//XMMATRIX matW = transform.GetLocalScaleMatrix() * Camera::GetBillBoardMatrix() * transform.GetWorldTranslateMatrix();
	//CONSTANT_BUFFER cBuffer;
	//cBuffer.matWVP = XMMatrixTranspose(matW *
	//								   Camera::GetViewMatrix() *
	//								   Camera::GetProjectionMatrix());
	//
	//D3D11_MAPPED_SUBRESOURCE subResource;
	//Direct3D::GetContext()->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	//memcpy_s(subResource.pData, subResource.RowPitch, (void*)(&cBuffer), sizeof(cBuffer));
	//Direct3D::GetContext()->Unmap(pConstantBuffer_, 0);
	//UINT stride = sizeof(IMAGE_DATA);
	//UINT offset = 0;
	//
	//
	//Direct3D::GetContext()->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);
	//Direct3D::GetContext()->VSSetConstantBuffers(0, 1, &pConstantBuffer_);
	//Direct3D::GetContext()->GSSetConstantBuffers(0, 1, &pConstantBuffer_);
	//Direct3D::GetContext()->PSSetConstantBuffers(0, 1, &pConstantBuffer_);
	//
	//Direct3D::GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	//Direct3D::GetContext()->Draw(imageWidth_ * imageHeight_, 0);
	//Direct3D::GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ColorChecker::DrawEnd()
{
	pChain_->Present(0,0);
}

void ColorChecker::Release()
{
}