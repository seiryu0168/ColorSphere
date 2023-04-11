//インクルード
#include <Windows.h>
#include<stdlib.h>
#include"DirectX_11/Direct3D.h"
#include"DirectX_11/Direct2D.h"
#include"GameObject/Camera.h"
#include"../ColorChecker.h"
#include"DirectX_11/Sprite.h"
#include"GameObject/Transform.h"
#include"ResourceManager/ImageManager.h"
#include"../DebugUI.h"
#include"DirectX_11/Input.h"
#include"GameObject/RootJob.h"
#include"../ImGui/imgui.h"
#include"../ImGui/imgui_impl_dx11.h"
#include"../ImGui/imgui_impl_win32.h"
#include"ResourceManager/Audio.h"


#include"ResourceManager/Text.h"


#pragma comment(lib, "winmm.lib")


//定数宣言
LPCWSTR WIN_CLASS_NAME = L"SampleGame";
LPCWSTR WIN_TITLE_NAME = L"サンプルゲーム";


const int WINDOW_WIDTH = 1920;   //ウィンドウ幅
const int WINDOW_HEIGHT = 1080;	 //ウィンドウ高さ
int pxelUnit;
//プロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

RootJob* pRootJob;

//エントリーポイント
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	//ウィンドウクラス(設計)作成
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);             //この構造体のサイズ
	wc.hInstance = hInstance;                   //インスタンスハンドル
	wc.lpszClassName = WIN_CLASS_NAME;            //ウィンドウクラス名
	wc.lpfnWndProc = WndProc;                   //ウィンドウプロシージャ
	wc.style = CS_VREDRAW | CS_HREDRAW;         //スタイル（デフォルト）
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); //アイコン
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);   //小さいアイコン
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);   //マウスカーソル
	wc.lpszMenuName = NULL;                     //メニュー（なし）
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //背景（白）
	RegisterClassEx(&wc);  //クラスを登録

	//ウィンドウサイズの計算
	RECT winRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);
	int winW = winRect.right - winRect.left;     //ウィンドウ幅
	int winH = winRect.bottom - winRect.top;     //ウィンドウ高さ
	//ウィンドウ作成
	HWND hWnd = CreateWindow(
		WIN_CLASS_NAME,         //ウィンドウクラス名
		WIN_TITLE_NAME,     //タイトルバーに表示する内容
		WS_OVERLAPPEDWINDOW, //スタイル（普通のウィンドウ）
		CW_USEDEFAULT,       //表示位置左（おまかせ）
		CW_USEDEFAULT,       //表示位置上（おまかせ）
		winW,                 //ウィンドウ幅
		winH,                 //ウィンドウ高さ
		NULL,                //親ウインドウ（なし）
		NULL,                //メニュー（なし）
		hInstance,           //インスタンス
		NULL                 //パラメータ（なし）
	);


	//ウィンドウ表示
	ShowWindow(hWnd, nCmdShow);

	//ウィンドウクラス(設計)作成
	WNDCLASSEX wc2;
	wc2.cbSize = sizeof(WNDCLASSEX);             //この構造体のサイズ
	wc2.hInstance = hInstance;                   //インスタンスハンドル
	wc2.lpszClassName = WIN_CLASS_NAME;            //ウィンドウクラス名
	wc2.lpfnWndProc = WndProc;                   //ウィンドウプロシージャ
	wc2.style = CS_VREDRAW | CS_HREDRAW;         //スタイル（デフォルト）
	wc2.hIcon = LoadIcon(NULL, IDI_APPLICATION); //アイコン
	wc2.hIconSm = LoadIcon(NULL, IDI_WINLOGO);   //小さいアイコン
	wc2.hCursor = LoadCursor(NULL, IDC_ARROW);   //マウスカーソル
	wc2.lpszMenuName = NULL;                     //メニュー（なし）
	wc2.cbClsExtra = 0;
	wc2.cbWndExtra = 0;
	wc2.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //背景（白）
	RegisterClassEx(&wc2);  //クラスを登録

	//ウィンドウサイズの計算
	RECT winRect2 = { 0, 0, 800, 600 };
	AdjustWindowRect(&winRect2, WS_OVERLAPPEDWINDOW, FALSE);
	int winW2 = winRect2.right - winRect2.left;     //ウィンドウ幅
	int winH2 = winRect2.bottom - winRect2.top;     //ウィンドウ高さ
	//ウィンドウ作成
	HWND hWnd2 = CreateWindow(
		WIN_CLASS_NAME,         //ウィンドウクラス名
		WIN_TITLE_NAME,     //タイトルバーに表示する内容
		WS_OVERLAPPEDWINDOW, //スタイル（普通のウィンドウ）
		CW_USEDEFAULT,       //表示位置左（おまかせ）
		CW_USEDEFAULT,       //表示位置上（おまかせ）
		winW2,                 //ウィンドウ幅
		winH2,                 //ウィンドウ高さ
		NULL,                //親ウインドウ（なし）
		NULL,                //メニュー（なし）
		hInstance,           //インスタンス
		NULL                 //パラメータ（なし）
	);


	//ウィンドウ表示
	ShowWindow(hWnd2, nCmdShow);

	CoInitialize(nullptr);

	if (FAILED(Direct3D::Initialize(WINDOW_WIDTH, WINDOW_HEIGHT, hWnd)))
	{
		PostQuitMessage(0);
	}
	if (FAILED(D2D::Initialize(WINDOW_WIDTH, WINDOW_HEIGHT, hWnd)))
	{
		PostQuitMessage(0);
	}
	ColorChecker che;
	che.Initialize(winW2, winH2, hWnd2);

	DebugUI::Initialize(hWnd, Direct3D::GetDevice(), Direct3D::GetContext());
	Input::Initialize(hWnd);
	Camera::Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);
	Audio::Initialize();

	pRootJob = new RootJob;
	
	//メッセージループ
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		//メッセージあり
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//メッセージなし
		else
		{
			//FPS制限
			static DWORD countFps = 0;					 //画面更新回数
			static DWORD startTime = timeGetTime();		 //起動時刻取得(ミリ秒)							//ミリ秒＝10^-3
			DWORD nowTime = timeGetTime();				 //今の時刻取得(ミリ秒)							//1000ミリ秒＝1秒
			static DWORD lastUpdateTime = nowTime;		 //最後に更新した時刻(ミリ秒) : Update
			static DWORD lastFixedUpdateTime = nowTime;  //最後に更新した時刻(ミリ秒) : FixedUpdate

			//今の時刻と起動時刻の差が1000ミリ秒超えたら
			//countFpsとstartTimeをリセット
			if (nowTime - startTime >= 1000)
			{
				WCHAR str[16];
				wsprintf(str, L"%u", countFps);
				SetWindowText(hWnd, str);
				countFps = 0;
				startTime = nowTime;//
			}

			//今の時刻と最後に更新した時刻の差*60が1000以上であれば
			if ((nowTime - lastUpdateTime) *60.0f >= 1000.0f)
			{
#ifdef _DEBUG
				DebugUI::StartImGui();
#endif
				//ゲームの処理
				Input::Update();
				Camera::Update();

				lastUpdateTime = nowTime;
				countFps++;
				pRootJob->UpdateSub();
#ifdef _DEBUG
				DebugUI::Debug(pRootJob->FindChild("SceneManager"));
				DebugUI::Log();
				ImGui::Render();
#endif 
				//描画処理
				Direct3D::BeginDraw();
				pRootJob->DrawSub();

				pRootJob->SecondDrawSub();

				ImageManager::Draw();
				pRootJob->ThirdDrawSub();
#ifdef _DEBUG	
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
				ID3D11Texture2D* backBuff;
				Direct3D::GetSwapChain()->GetBuffer(0,__uuidof(ID3D11Texture2D),(LPVOID*)&backBuff);
				che.CalcPixel(backBuff);
				SAFE_RELEASE(backBuff);
				Direct3D::EndDraw();
				che.DrawStart();
				che.Update();
				che.Draw();
				che.DrawEnd();
			}
		}
	}

	Audio::Releace();
	DebugUI::CleanUp();
	ImageManager::AllRelease();
	pRootJob->ReleaseSub();
	Input::Release();
	D2D::Release();
	Direct3D::Release();

	CoUninitialize();
	return 0;
}
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//ウィンドウプロシージャ(なんかあったら呼ばれる関数)
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd,msg,wParam,lParam))
	{
		return true;
	}

	switch (msg)
	{

	case WM_MOUSEMOVE:
		Input::SetMousePosition(LOWORD(lParam), HIWORD(lParam));
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);  //プログラム終了
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

