#include <DxLib.h>
#include <array>
#include <cassert>
#include "Game.h"
#include "InputState.h"
#include "SoundManager.h"
#include "Scene/SceneManager.h"
#include "Scene/TitleScene.h"
#include "ThreeDimensionEffectManager.h"

// �v���O������ WinMain ����n�܂�܂�
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	// Window���[�h�ݒ�
	ChangeWindowMode(Game::window_mode);

	// Window���ݒ�
	SetMainWindowText(Game::title_text);

	// ��ʃT�C�Y�̐ݒ�
	SetGraphMode(Game::screen_width, Game::screen_height, Game::color_depth);

	// �Q�[�����ɃE�B���h�E���[�h��؂�ւ��Ă��O���t�B�b�N�n���h�������Z�b�g���Ȃ�
	SetChangeScreenModeGraphicsSystemResetFlag(false);

	// �ق��̃E�B���h�E��I�����Ă��Ă������悤�ɂ���
	SetAlwaysRunFlag(false);

	// �E�B���h�E�̃T�C�Y��ύX�\�ɂ���
	SetWindowSizeChangeEnableFlag(true);

	// DirectX9���g�p����悤�ɂ���B(DirectX11����)
	// Effekseer���g�p����ɂ͕K���ݒ肷��B
	SetUseDirect3DVersion(DX_DIRECT3D_9EX);

	// ********** �t�H���g�̃��[�h **********
	LPCSTR font_path = "Data/Font/nicoca_v2.ttf"; // �ǂݍ��ރt�H���g�t�@�C���̃p�X
	if (AddFontResourceEx(font_path, FR_PRIVATE, NULL) < 0)
	{
		return -1;
	}
	
	// �c�w���C�u��������������
	if (DxLib_Init() == -1)
	{
		// �G���[���N�����璼���ɏI��
		return -1;
	}

	// �_�u���o�b�t�@���[�h
	// ����ʂɕ`��
	SetDrawScreen(DX_SCREEN_BACK);

	auto& effectManager = ThreeDimensionEffectManager::GetInstance();
	auto& soundManager = SoundManager::GetInstance();
	InputState input;
	SceneManager sceneManager;

	if (effectManager.Init() == -1)
	{
		return -1;
	}

	//VERTEX2DSHADER Vert[6];

	//int imgHandle = MakeScreen(Game::screen_width, Game::screen_height, true);
	//int psHandle = LoadPixelShader("SampleMono.pso");
	//assert(psHandle != -1);

	//// ���_�f�[�^�̏���
	//Vert[0].pos = VGet(0.0f, 0.0f, 0.0f);
	//Vert[1].pos = VGet(Game::screen_width - 1, 0.0f, 0.0f);
	//Vert[2].pos = VGet(0.0f, Game::screen_height - 1, 0.0f);
	//Vert[3].pos = VGet(Game::screen_width - 1, Game::screen_height - 1, 0.0f);
	//Vert[0].dif = GetColorU8(255, 255, 255, 255);
	//Vert[0].spc = GetColorU8(0, 0, 0, 0);
	//Vert[0].u = 0.0f; Vert[0].v = 0.0f;
	//Vert[1].u = 1.0f; Vert[1].v = 0.0f;
	//Vert[2].u = 0.0f; Vert[2].v = 1.0f;
	//Vert[3].u = 1.0f; Vert[3].v = 1.0f;
	//Vert[0].su = 0.0f; Vert[0].sv = 0.0f;
	//Vert[1].su = 1.0f; Vert[1].sv = 0.0f;
	//Vert[2].su = 0.0f; Vert[2].sv = 1.0f;
	//Vert[3].su = 1.0f; Vert[3].sv = 1.0f;
	//Vert[0].rhw = 1.0f;
	//Vert[1].rhw = 1.0f;
	//Vert[2].rhw = 1.0f;
	//Vert[3].rhw = 1.0f;
	//Vert[4] = Vert[2];
	//Vert[5] = Vert[1];


	//SetUseTextureToShader(0, imgHandle);

	//SetUsePixelShader(psHandle);


	/*std::array< VERTEX2DSHADER, 4 > vertex;
	std::array< unsigned short, 6 > vertexIndex;

	auto Settings = [](VERTEX2DSHADER& vtx, float x, float y, float u, float v)
	{
		vtx.pos = VGet(x, y, 0.0f);
		vtx.u = u;
		vtx.v = v;
		vtx.su = u;
		vtx.sv = v;
		vtx.rhw = 1.0f;
		vtx.dif = GetColorU8(255, 255, 255, 255);
		vtx.spc = GetColorU8(0, 0, 0, 0);
	};*/

	//Settings(vertex[0], 100.0f, 100.0f, 0.0f, 0.0f);
	//Settings(vertex[1], 600.0f, 100.0f, 1.0f, 0.0f);
	//Settings(vertex[2], 100.0f, 600.0f, 0.0f, 1.0f);
	//Settings(vertex[3], 600.0f, 600.0f, 1.0f, 1.0f);

	//vertexIndex = { 0, 1, 2, 1, 2, 3 };

	
//	SetPSConstSF(GetConstIndexToShader("alpha", psHandle), 1.0f);
//	SetPSConstSF(GetConstIndexToShader("mosLv", psHandle), 0.5f);

//	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	sceneManager.ChangeScene(new TitleScene(sceneManager));

	// �ُ킪�N�������ɏI��
	while (ProcessMessage() == 0)
	{
		LONGLONG time = GetNowHiPerformanceCount();

		// ��ʂ̃N���A
		ClearDrawScreen();

		input.Update();
		sceneManager.Update(input);
		effectManager.Update();

	//	SetDrawScreen(imgHandle);
		sceneManager.Draw(input);
		effectManager.Draw();

	//	SetDrawScreen(DX_SCREEN_BACK);
		// �`��
	//	DrawPrimitive2DToShader(Vert, 6, DX_PRIMTYPE_TRIANGLELIST);
	//	DrawGraph(0, 0, imgHandle, true);

		// FPS(Frame Per Second)�̎擾
		auto fps = GetFPS();

		// �`�施�ߐ��̎擾
		auto drawcall = GetDrawCallCount();

		DrawFormatString(10, 30, 0x000000, "FPS = %2.2f", fps);
		DrawFormatString(10, 60, 0x000000, "DC = %d", drawcall);

		// ����ʂ�\��ʂ����ւ���
		ScreenFlip();

		// esc�L�[����������I������
		if (CheckHitKey(KEY_INPUT_ESCAPE))	break;

		// fps��60�ɌŒ�
		while (GetNowHiPerformanceCount() - time < 16667)
		{

		}
	}

	effectManager.End();

	// �c�w���C�u�����g�p�̏I������
	DxLib_End();

	// �\�t�g�̏I�� 
	return 0;
}
