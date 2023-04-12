#pragma once
#include"Engine/DirectX_11/Direct3D.h"
#include"Engine/GameObject/Transform.h"
class ColorChecker
{
private:
	//�F�̏��ƒ��_���Ƃ��Ďg��
	struct IMAGE_DATA
	{
		XMFLOAT3 color;
	};
	struct CONSTANT_BUFFER
	{
		XMMATRIX matWVP;
		XMMATRIX matNormal;
	};
	struct CONPUTE_C_BUFFER
	{
		XMFLOAT2 dispatch;
		XMFLOAT2 uv;
	};
		XMFLOAT2 dispatch;
	
	int hModel_;
	int dispatchX_;
	int dispatchY_;

	int imageWidth_;
	int imageHeight_;
	float R;
	Transform transform;

	//�V�F�[�_�[�֘A�̕ϐ�
	ID3D11ShaderResourceView* smplSRV_;//�V�F�[�_�[���\�[�X�r���[
	ID3D11UnorderedAccessView* UAV_; //�V�F�[�_�[����̏o�͂��󂯎��z(UAV:UnorderedAccessView)
	ID3D11Buffer* outBuff_;//UAV�̏����󂯎��
	ID3D11Buffer* pOutputBuffer_;//�V�F�[�_�[����̏o�͂��󂯎��z��p�ӂ��邽�߂̃o�b�t�@
	ID3D11Buffer* pSampleBuffer_;//
	ID3D11Buffer* pResultBuffer_;//

	ID3D11ComputeShader* pComputeShader;
	ID3D11VertexShader* pVertexShader_;
	ID3D11GeometryShader* pGeometryShader_;
	ID3D11PixelShader* pPixelShader_;
	ID3D11RasterizerState* pRasterizer_;
	ID3D11InputLayout* pLayout_;
	
	//�V�F�[�_�[�ɓ������̕ϐ�
	ID3D11Buffer* pConstantBuffer_;
	ID3D11Buffer* pConputeConstantBuffer_;
	ID3D11Buffer* pVertexBuffer_;
	IMAGE_DATA* pPixelData_;

	//�`��ׂ̈̎d�g�݂Ɋւ���ϐ�
	IDXGIDevice* pDevice2_;
	IDXGIAdapter* pAdapter_;
	IDXGIFactory* pFactory_;
	IDXGISwapChain* pChain_;
	ID3D11RenderTargetView* pRTV_;
	ID3D11SamplerState* pSampler_;
	ID3D11ShaderResourceView* pSRV_;
	ID3D11Texture2D* pSampleImage_;
	HRESULT InitComputeShader(LPCWSTR fileName,LPCSTR entryPoint, ID3D11Device* device,ID3DBlob** blob);
	HRESULT InitVertexShader(LPCWSTR fileName, LPCSTR entryPoint, ID3D11Device* device, ID3DBlob** blob);
	HRESULT InitGeometryShader(LPCWSTR fileName, LPCSTR entryPoint, ID3D11Device* device, ID3DBlob** blob);
	HRESULT InitPixelShader(LPCWSTR fileName, LPCSTR entryPoint, ID3D11Device* device, ID3DBlob** blob);
	
	HRESULT InitShader();
public:
	ColorChecker();
	~ColorChecker();

	void Initialize(int width,int height,HWND hWnd);
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

