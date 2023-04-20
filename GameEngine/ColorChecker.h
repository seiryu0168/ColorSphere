#pragma once
#include"Engine/DirectX_11/Direct3D.h"
#include"Engine/GameObject/Transform.h"
class ColorChecker
{
private:
	//色の情報と頂点情報として使う
	struct IMAGE_DATA
	{
		XMVECTOR color;
	};
	struct CONSTANT_BUFFER
	{
		XMMATRIX matWVP;
		XMMATRIX matNormal;
	};
	struct CONPUTE_CONSTANT_BUFFER
	{
		XMFLOAT2 threadGroup;
	};
	int hModel_;
	int dispatchX_;
	int dispatchY_;

	int imageWidth_;
	int imageHeight_;
	Transform transform;
	
	D3D11_TEXTURE2D_DESC texDesc;//テクスチャの設定
	ID3D11Texture2D* pTexture;//シェーダーリソースビュー用のテクスチャ
	
	ID3D11ShaderResourceView* smplSRV;//シェーダーリソースビュー
	ID3D11Buffer* pOutputBuffer;//シェーダーの出力を受け取るバッファ
	ID3D11Buffer* outBuff;//CPUが読めるバッファ

	D3D11_VIEWPORT vp_;//ビューポート

	//シェーダー関連の変数
	ID3D11ComputeShader* pComputeShader;//コンピュ－トシェーダー
	ID3D11VertexShader* pVertexShader_;//頂点シェーダー
	ID3D11PixelShader* pPixelShader_;//ピクセルシェーダー
	ID3D11RasterizerState* pRasterizer_;//ラスタライザー
	ID3D11InputLayout* pLayout_;//インプットレイアウト

	//シェーダーに入れる情報の変数
	ID3D11Buffer* pConstantBuffer_;//コンスタントバッファ
	ID3D11Buffer* pConputeConstantBuffer_;//コンピュ－トシェーダー用のコンスタントバッファ
	ID3D11Buffer* pVertexBuffer_;//頂点シェーダー用のバッファ
	IMAGE_DATA* pPixelData_;//画像のRGB値

	//描画の為の仕組みに関する変数
	IDXGIDevice* pDevice2_;
	IDXGIAdapter* pAdapter_;
	IDXGIFactory* pFactory_;
	IDXGISwapChain* pChain_;
	ID3D11RenderTargetView* pRTV_;
	ID3D11SamplerState* pSampler_;
	HRESULT InitComputeShader(LPCWSTR fileName, LPCSTR entryPoint, ID3D11Device* device, ID3DBlob** blob);
	HRESULT InitVertexShader(LPCWSTR fileName, LPCSTR entryPoint, ID3D11Device* device, ID3DBlob** blob);
	HRESULT InitGeometryShader(LPCWSTR fileName, LPCSTR entryPoint, ID3D11Device* device, ID3DBlob** blob);
	HRESULT InitPixelShader(LPCWSTR fileName, LPCSTR entryPoint, ID3D11Device* device, ID3DBlob** blob);

	HRESULT InitShader();
public:
	ColorChecker();
	~ColorChecker();

	void Initialize(int width, int height, HWND hWnd);
	void SetSampleBuffer(ID3D11Texture2D* buff);
	void ShaderDispatch(int imageWidth, int imageHeight);
	void GetShaderResult(ID3D11Buffer* resultBuffer);
	void CalcPixel(ID3D11Texture2D* buff);
	void Update();
	void DrawStart();
	void Draw();
	void DrawEnd();
	void Release();

};

