#include<d3dcompiler.h>
#include "ColorChecker.h"
#include"Engine/ResourceManager/Model.h"
#include"Engine/GameObject/Camera.h"
#include"Engine/DirectX_11/Input.h"

ColorChecker::ColorChecker()
{
	R = 0;
}

ColorChecker::~ColorChecker()
{
	Release();
}
void ColorChecker::Initialize(int screenWidth, int screenHeight, HWND hWnd)
{
	hModel_ = ModelManager::Load("Assets\\Block.fbx");
	assert(hModel_ >= 0);
	imageWidth_ = Direct3D::GetScreenWidth();
	imageHeight_ = Direct3D::GetScreenHeight();
	HRESULT hr;
	DXGI_SWAP_CHAIN_DESC scDesc;

	//とりあえず全部0
	ZeroMemory(&scDesc, sizeof(scDesc));

	//描画先のフォーマット
	scDesc.BufferDesc.Width = screenWidth;		//画面幅
	scDesc.BufferDesc.Height = screenHeight;	//画面高さ
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 何色使えるか

	//FPS（1/60秒に1回）
	scDesc.BufferDesc.RefreshRate.Numerator = 60;
	scDesc.BufferDesc.RefreshRate.Denominator = 1;

	//その他
	scDesc.Windowed = TRUE;			//ウィンドウモードかフルスクリーンか
	scDesc.OutputWindow = hWnd;		//ウィンドウハンドル
	scDesc.BufferCount = 1;			//バックバッファの枚数
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//バックバッファの使い道＝画面に描画するために
	scDesc.SampleDesc.Count = 1;		//MSAA（アンチエイリアス）の設定
	scDesc.SampleDesc.Quality = 0;		//　〃

	//スワップチェイン作成
	hr = Direct3D::GetFactory()->CreateSwapChain(Direct3D::GetDevice(), &scDesc, &pChain_);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"スワップチェイン作成失敗", L"エラー", MB_OK);
	}
	//バックバッファ取得
	ID3D11Texture2D* pBackBuffer;
	hr = pChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"バックバッファ取得失敗", L"エラー", MB_OK);
	}
	//レンダーターゲットビュー作成
	hr = Direct3D::GetDevice()->CreateRenderTargetView(pBackBuffer, NULL, &pRTV_);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"レンダーターゲットビュー作成失敗", L"エラー", MB_OK);
	}
	SAFE_RELEASE(pBackBuffer);
	//ID3DBlob* csBlob;
	hr = InitShader();
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"シェーダーのコンパイル失敗", L"エラー", MB_OK);
	}
	/////////////////////コンスタントバッファ///////////////////
	{
		D3D11_BUFFER_DESC cBufferDesc;
		cBufferDesc.ByteWidth = sizeof(CONSTANT_BUFFER);
		cBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cBufferDesc.MiscFlags = 0;
		cBufferDesc.StructureByteStride = 0;
		hr = Direct3D::GetDevice()->CreateBuffer(&cBufferDesc, nullptr, &pConstantBuffer_);
	}
	////////////////////コンピュートシェーダ用のコンスタントバッファ//////////////
	{
		D3D11_BUFFER_DESC cBufferDesc;
		cBufferDesc.ByteWidth = sizeof(CONSTANT_BUFFER);
		cBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cBufferDesc.MiscFlags = 0;
		cBufferDesc.StructureByteStride = 0;
		hr = Direct3D::GetDevice()->CreateBuffer(&cBufferDesc, nullptr, &pConputeConstantBuffer_);
	}
	////////////////////////テクスチャデスク/////////////////////
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Width = imageWidth_;
	texDesc.Height = imageHeight_;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DYNAMIC;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texDesc.MiscFlags = 0;
	hr = Direct3D::GetDevice()->CreateTexture2D(&texDesc, nullptr, &pSampleImage_);

	////////////////////////シェーダーリソースビューデスク///////////////////
	ZeroMemory(&smplSRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	smplSRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	smplSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	smplSRVDesc.Texture2D.MostDetailedMip = 0;
	smplSRVDesc.Texture2D.MipLevels = 1;
	hr = Direct3D::GetDevice()->CreateShaderResourceView(pSampleImage_, &smplSRVDesc, &smplSRV_);

	///////////////////////UAVバッファデスク//////////////////////////

	ZeroMemory(&UAVBufferDesc, sizeof(D3D11_BUFFER_DESC));
	UAVBufferDesc.ByteWidth = sizeof(IMAGE_DATA) * imageWidth_ * imageHeight_;
	UAVBufferDesc.Usage = D3D11_USAGE_DEFAULT;  //バッファの読み取る方法を指定
	//これの場合GPUによる書き込みと読み込みを許可
	UAVBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS |//UAVとシェーダーリソースの
		D3D11_BIND_SHADER_RESOURCE;  //バインドフラグを立てないとバッファ作成が失敗する
	UAVBufferDesc.CPUAccessFlags = 0;//GPUからしかアクセスしないのでこれ
	UAVBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;//構造化されたバッファを作る場合これ
	UAVBufferDesc.StructureByteStride = sizeof(IMAGE_DATA);//構造化バッファの要素のサイズ
	hr = Direct3D::GetDevice()->CreateBuffer(&UAVBufferDesc, NULL, &pOutputBuffer_);

	//バッファの設定
	D3D11_BUFFER_DESC bufDesc;
	pOutputBuffer_->GetDesc(&bufDesc);
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;//CPUから読めるようにフラグを立てる
	bufDesc.Usage = D3D11_USAGE_STAGING;		   //GPUからCPUへのデータ転送するためのフラグ
	bufDesc.BindFlags = 0;
	bufDesc.MiscFlags = 0;

	hr = Direct3D::GetDevice()->CreateBuffer(&bufDesc, nullptr, &outBuff_);

	/////////////////////UAVデスク//////////////////////
	ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;		//UAVのリソースデータ形式。DXGI_FORMATリファレンス参照
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;//出力を受け取るバッファとして使うのでこれ
	UAVDesc.Buffer.FirstElement = 0;
	UAVDesc.Buffer.NumElements = imageWidth_ * imageHeight_;//データの要素数
	UAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;   //フォーマットによって変わる。
	//詳しくはD3D_BUFFER_UAV_FLAGリファレンス参照
	hr = Direct3D::GetDevice()->CreateUnorderedAccessView(pOutputBuffer_, &UAVDesc, &UAV_);

	/////////////////////ビューポート作成///////////////////

	vp.Width = screenWidth;
	vp.Height = screenHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	////コンピュートシェーダ用コンスタントバッファにデータ入れる////
	CONPUTE_C_BUFFER ccBuf;
	ccBuf.dispatch = dispatch;
	ccBuf.uv = { 0,0 };
	D3D11_MAPPED_SUBRESOURCE subResource;
	Direct3D::GetContext()->Map(pConputeConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	memcpy_s(subResource.pData, subResource.RowPitch, (void*)(&ccBuf), sizeof(ccBuf));
	Direct3D::GetContext()->Unmap(pConputeConstantBuffer_, 0);
}

void ColorChecker::SetSampleBuffer(ID3D11Texture2D* buff)
{

}

void ColorChecker::ShaderDispatch(int imageWidth, int imageHeight)
{
	Direct3D::GetContext()->Dispatch(imageWidth/(float)dispatch.x, imageHeight/(float)dispatch.y, 1);
}

void ColorChecker::GetShaderResult(ID3D11Buffer* resultBuffer)
{

}

void ColorChecker::CreateUAV(ID3D11UnorderedAccessView* uav, ID3D11Buffer* buff)
{

}

void ColorChecker::CreateSRV(ID3D11ShaderResourceView* srv, ID3D11Resource* rsource)
{
}

void ColorChecker::CalcPixel(ID3D11Texture2D* buff)
{
	SAFE_RELEASE(pVertexBuffer_);
	HRESULT hr;
	ID3D11SamplerState* smplSampler;
	D3D11_SAMPLER_DESC  SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = Direct3D::pDevice->CreateSamplerState(&SamDesc, &smplSampler);

	//レンダリング結果のシェーダーリソースビューを作成


	//受け取ったバッファをテクスチャ化
	Direct3D::GetContext()->CopyResource(pSampleImage_, buff);
	//D3D11_MAPPED_SUBRESOURCE mappedTex;
	//Direct3D::GetContext()->Map(pSampleImage_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedTex);
	//UINT* screen = (UINT*)mappedTex.pData;

	//for (int i = 0; i < imageWidth_; i++)
	//{
	//	UINT row = i * mappedTex.RowPitch;
	//	for (int j = 0; j < imageHeight_; j++)
	//	{
	//		UINT col = j * 4;
	//		screen[row + col + 0] = 255;
	//		screen[row + col + 1] = 255;
	//		screen[row + col + 2] = 255;
	//		screen[row + col + 3] = 255;
	//
	//	}
	//
	//}

	//Direct3D::GetContext()->CopyResource(buff, pSampleImage_);
	//Direct3D::GetContext()->Unmap(pSampleImage_, 0);


	//////////////////シェーダーに設定するところ///////////////////////

	//シェーダーリソースビュー設定
	Direct3D::GetContext()->CSSetShaderResources(0, 1, &smplSRV_);
	//サンプラー設定
	Direct3D::GetContext()->CSSetSamplers(0, 1, &smplSampler);
	//コンスタントバッファ設定
	Direct3D::GetContext()->CSSetConstantBuffers(0, 1, &pConputeConstantBuffer_);
	//作成したコンピュートシェーダを設定
	Direct3D::GetContext()->CSSetShader(pComputeShader, nullptr, 0);
	//UAVを設定
	Direct3D::GetContext()->CSSetUnorderedAccessViews(0, 1, &UAV_, 0);
	//実行
	ShaderDispatch(imageWidth_, imageHeight_);

	///////////シェーダーからのデータを受け取るところ

	Direct3D::GetContext()->CopyResource(outBuff_, pOutputBuffer_);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	Direct3D::GetContext()->Map(outBuff_, 0, D3D11_MAP_READ, 0, &mappedResource);
	//IMAGE_DATA* p = reinterpret_cast<IMAGE_DATA*>(mappedResource.pData);
	size_t dataSize = (size_t)(imageWidth_ * imageHeight_);
	pPixelData_ = new IMAGE_DATA[dataSize];
	memcpy_s(pPixelData_, sizeof(IMAGE_DATA) * dataSize, mappedResource.pData, mappedResource.RowPitch);
	Direct3D::GetContext()->Unmap(outBuff_, 0);

	D3D11_BUFFER_DESC vDesc;
	vDesc.ByteWidth = sizeof(IMAGE_DATA) * dataSize;
	vDesc.Usage = D3D11_USAGE_DEFAULT;
	vDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vDesc.CPUAccessFlags = 0;
	vDesc.MiscFlags = 0;
	vDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vData;
	vData.pSysMem = pPixelData_;

	hr = Direct3D::GetDevice()->CreateBuffer(&vDesc, &vData, &pVertexBuffer_);

	SAFE_RELEASE(smplSampler);
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

	* blob = nullptr;
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

	const D3D_SHADER_MACRO defines[] = { "EXAMPLE_DEFINE","1",NULL,NULL };
	//シェーダーコンパイル
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
	//シェーダーコンパイル
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
	//シェーダーコンパイル
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
	//シェーダーコンパイル
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
		MessageBox(nullptr, L"シェーダーの作成に失敗", L"エラー", MB_OK);
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
		MessageBox(nullptr, L"インプットレイアウトの作成に失敗", L"エラー", MB_OK);
		return hr;
	}

	D3D11_RASTERIZER_DESC rDesc;
	rDesc.CullMode = D3D11_CULL_NONE;
	rDesc.FillMode = D3D11_FILL_SOLID;
	rDesc.FrontCounterClockwise = TRUE;
	hr = Direct3D::GetDevice()->CreateRasterizerState(&rDesc, &pRasterizer_);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"ラスタライザの作成に失敗", L"エラー", MB_OK);
		return hr;
	}

	SAFE_RELEASE(psBlob);
	SAFE_RELEASE(vsBlob);
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

	//for (size_t i = 0; i < imageWidth_ * imageHeight_; i++)
	//{
	//	XMFLOAT3 Col=pPixelData_[i].color;
	//	int o = 0;
	//}
	SAFE_DELETE(pPixelData_);
}

void ColorChecker::DrawStart()
{
	Direct3D::GetContext()->OMSetRenderTargets(1, &pRTV_, Direct3D::GetDepthStencilView());
	Direct3D::GetContext()->RSSetViewports(1, &vp);
	float clearColor[4] = { 1,1,1,1 };
	//R += 0.01;
	Direct3D::GetContext()->ClearRenderTargetView(pRTV_, clearColor);
	Direct3D::GetContext()->ClearDepthStencilView(Direct3D::GetDepthStencilView(),
		D3D11_CLEAR_DEPTH,
		1.0f,
		0);
}

void ColorChecker::Draw()
{
	//シェーダー変更
	Direct3D::GetContext()->VSSetShader(pVertexShader_, NULL, 0);
	Direct3D::GetContext()->PSSetShader(pPixelShader_, NULL, 0);
	Direct3D::GetContext()->IASetInputLayout(pLayout_);
	Direct3D::GetContext()->RSSetState(pRasterizer_);





	//行列
	XMMATRIX matW = transform.GetLocalScaleMatrix() * Camera::GetBillBoardMatrix() * transform.GetWorldTranslateMatrix();
	CONSTANT_BUFFER cBuffer;
	cBuffer.matWVP = XMMatrixTranspose(transform.GetWorldMatrix() *
		Camera::GetViewMatrix() *
		Camera::GetProjectionMatrix());


	D3D11_MAPPED_SUBRESOURCE subResource;
	Direct3D::GetContext()->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	memcpy_s(subResource.pData, subResource.RowPitch, (void*)(&cBuffer), sizeof(cBuffer));
	Direct3D::GetContext()->Unmap(pConstantBuffer_, 0);
	UINT stride = sizeof(IMAGE_DATA);
	UINT offset = 0;

	ModelManager::SetTransform(hModel_, transform);
	ModelManager::Draw(hModel_);

	Direct3D::GetContext()->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);
	Direct3D::GetContext()->VSSetConstantBuffers(0, 1, &pConstantBuffer_);
	Direct3D::GetContext()->PSSetConstantBuffers(0, 1, &pConstantBuffer_);
	//Direct3D::GetContext()->UpdateSubresource(pConstantBuffer_, 0, nullptr, &cBuffer, 0, 0);

	Direct3D::GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	Direct3D::GetContext()->Draw(imageWidth_ * imageHeight_, 0);
	Direct3D::GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ColorChecker::DrawEnd()
{
	pChain_->Present(0, 0);
}

void ColorChecker::Release()
{
	SAFE_RELEASE(pSampleImage_);
	SAFE_RELEASE(smplSRV_);
	SAFE_RELEASE(pOutputBuffer_);
	SAFE_RELEASE(UAV_);
	SAFE_RELEASE(outBuff_);
}
