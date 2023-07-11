#include "PlatinumDataLoader.h"
#include <DxLib.h>
#include <string>
#include <cassert>

void PlatinumDataLoader::Load(const char* filePath)
{
	// FMFヘッダー(Platinumのドキュメントに書いてある)
	struct Header
	{
		int8_t id[4];			// 識別子(FMF_)		1 * 4バイト
		uint32_t size;			// データサイズ		4バイト
		uint32_t mapWidth;		// マップの幅(200)  4バイト
		uint32_t mapHeight;		// マップの高さ		4バイト
		uint8_t chipWidth;		// チップ(セル１個)の幅		1バイト
		uint8_t chipHeight;		// チップ(セル１個)の高さ	1バイト
		uint8_t layerCount;		// レイヤー数		1バイト
		uint8_t bitCount;		// １セルあたりのビット数(÷8でバイト数)　1バイト
	};
	// 20バイト

	Header header;
	int handle = FileRead_open(filePath);
	FileRead_read(&header, sizeof(header), handle);

	std::string strId;
	strId.resize(4);
	std::copy_n(header.id, 4, strId.begin());

	if (strId != "FMF_")
	{
		// ロード失敗
		assert(0);
	}

	mapWidth_ = header.mapWidth;
	mapHeight_ = header.mapHeight;

	int layerCount = header.layerCount;

	// レイヤー1個当たりのサイズを計算する
	// マップの幅 * マップの高さ * (チップ１個あたりのバイト数)
	int layerDataSize = header.mapWidth * header.mapHeight * (header.bitCount / 8);

	mapData_.resize(layerCount);
	for (auto& layer : mapData_)
	{
		layer.resize(layerDataSize);
		FileRead_read(layer.data(), layerDataSize, handle);
	}

	FileRead_close(handle);

	// 実行時メモリ効率化のために、データを加工
	for (int layerIdx = 0; layerIdx < layerCount; layerIdx++)
	{
		TransposeMapData(layerIdx);
	}
}

const MapData_t& PlatinumDataLoader::GetMapData() const
{
	return mapData_;
}

const int PlatinumDataLoader::GetChipId(int layerId, int chipX, int chipY) const
{
	assert(chipX < mapWidth_);
	assert(chipY < mapHeight_);
	auto index = chipY + chipX * mapHeight_;
	return mapData_[layerId][index];
}

void PlatinumDataLoader::GetMapSize(int& width, int& height)
{
	width = mapWidth_;
	height = mapHeight_;
}

void PlatinumDataLoader::TransposeMapData(int layerId)
{
	// いったんコピーしておく
	// 理由としてはコピーせずに転置しょうとすると元のデータが消える
	auto temp = mapData_[layerId];

	for (int y = 0; y < mapHeight_; y++)
	{
		for (int x = 0; x < mapWidth_; x++)
		{
			// 通常のXY指定の場合
			// y * mapWidth_ + xとういう風になる
			// さらに、横200縦15は維持する必要がある
			// 縦に並べていきたい
			// 0の隣は1段下にした
			int idxSrc = y * mapWidth_ + x;		// Source Index
			int idxDst = y + mapHeight_ * x;	// Destination Index
			mapData_[layerId][idxDst] = temp[idxSrc];
		}
	}
}
