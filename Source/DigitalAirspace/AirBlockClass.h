// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <string>

const int MAX_GIRD_SIZE = 20000;

/**
 * 
 */
class DIGITALAIRSPACE_API AirBlockClass
{
public:
	//空域网格类
	class AirBlock {
	public:
		int id=0;
		FIntVector GridCoordinates; // 网格坐标(X,Y,Z)，其实就是编号,只有最大的网格有
		FVector World_center_Location;      // 世界空间中的网格中心位置
		float size=MAX_GIRD_SIZE;                       // 网格物理尺寸
		// 拓扑属性
		int level=0;                        // 层级(0=根节点)
		AirBlock* parent=nullptr;               // 父节点指针
		TArray<AirBlock*> children; // 子节点数组
		int state=0;           // 当前状态,0为可通行,1为不可通行，2为混合状态
		std::string towerid = "-1";
		std::string weather="none";//天气
		int wind_power=0;//风力
		std::string wind_direction="";//风向
	};
	// 最大的网格的所有数据
	TArray<TArray<TArray<AirBlock>>> AirGridCells;
	int nowid = 0;//下一个网格的id，用来给数据库标识父节点
	// 网格大小
	float GridSize=MAX_GIRD_SIZE;
	//起始位置
	FVector WorldOrigin=FVector(0,0,0);
	//网格总数（x，y，z）
	FIntVector GridDimensions= FIntVector(5,5,5);
	UWorld* world;
	int CurrentIndex = 0;
	AirBlockClass(UWorld* w);
	~AirBlockClass();

	//更新空域网格状态
	void UpdateGridFromWorld();

	void DetectingObstacles(AirBlock& cell);

	FVector GridToWorld(const FIntVector& GridCoord) const;

	void CalculateChildAirBlock(AirBlock& cell);

	void DrawDebugGrid(UWorld* World, float Duration) const;
	//FString AirBlockToString(const AirBlock& Block);
	void Save3DAirBlockArrayToFile( const FString& FilePath);

};
