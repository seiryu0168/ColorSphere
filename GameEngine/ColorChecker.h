#pragma once
#include"Engine/DirectX_11/Direct3D.h"
#include"Engine/GameObject/Transform.h"
class ColorChecker
{
private:
	//�F�̏��ƒ��_���Ƃ��Ďg��
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
	
	D3D11_TEXTURE2D_DESC texDesc;//�e�N�X�`���̐ݒ�
	ID3D11Texture2D* pTexture;//�V�F�[�_�[���\�[�X�r���[�p�̃e�N�X�`��
	
	ID3D11ShaderResourceView* smplSRV;//�V�F�[�_�[���\�[�X�r���[
	ID3D11Buffer* pOutputBuffer;//�V�F�[�_�[�̏o�͂��󂯎��o�b�t�@
	ID3D11Buffer* outBuff;//CPU���ǂ߂�o�b�t�@

	D3D11_VIEWPORT vp_;//�r���[�|�[�g

	//�V�F�[�_�[�֘A�̕ϐ�
	ID3D11ComputeShader* pComputeShader;//�R���s���|�g�V�F�[�_�[
	ID3D11VertexShader* pVertexShader_;//���_�V�F�[�_�[
	ID3D11PixelShader* pPixelShader_;//�s�N�Z���V�F�[�_�[
	ID3D11RasterizerState* pRasterizer_;//���X�^���C�U�[
	ID3D11InputLayout* pLayout_;//�C���v�b�g���C�A�E�g

	//�V�F�[�_�[�ɓ������̕ϐ�
	ID3D11Buffer* pConstantBuffer_;//�R���X�^���g�o�b�t�@
	ID3D11Buffer* pConputeConstantBuffer_;//�R���s���|�g�V�F�[�_�[�p�̃R���X�^���g�o�b�t�@
	ID3D11Buffer* pVertexBuffer_;//���_�V�F�[�_�[�p�̃o�b�t�@
	IMAGE_DATA* pPixelData_;//�摜��RGB�l

	//�`��ׂ̈̎d�g�݂Ɋւ���ϐ�
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

