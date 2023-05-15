#pragma once
#include <vector>

enum class LayerType
{
	Background,	// 背景
	Enemied,	// 敵
	Event,		// イベント
};

// マップの広さ * レイヤー数なので、2つのベクターを入れ子にしている
using MapData_t = std::vector<std::vector<unsigned char>>;

// ステージ情報を管理するクラス
class Stage
{
	MapData_t mapData_;
	int mapWidth_;
	int mapHeight_;
public:
	void Load(const char* filePath);
	const MapData_t& GetMapData()const;
	const int GetChipId(int layerId, int chipX, int chipY)const;
	void GetMapSize(int& width, int& height);
private:
	/// <summary>
	/// データの内容をXY転置する
	/// </summary>
	/// <param name="layerId">レイヤーID</param>
	void TransposeMapData(int layerId);
};