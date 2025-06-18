// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include <unordered_map>
#include <string>
#include "AirBlockClass.h"
#include "DigitalAirspaceGameModeBase.generated.h"
/**
 * 
 */
UCLASS()
class DIGITALAIRSPACE_API ADigitalAirspaceGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	//设定重复执行
	FTimerHandle TimerHandle;
	//区块类
	class Block_Class {
	public:
		//0是左下角坐标，1是右上角坐标
		float x0, y0, x1, y1;
		float Block_Num_x, Block_Num_y;
		bool isHidden;
		TArray<AActor*> Block_Actors;
		//构造函数

		Block_Class(float x0_, float y0_, float max_block_size) {
			x0 = x0_;
			y0 = y0_;
			x1 = x0 + max_block_size;
			y1 = y0 + max_block_size;
			Block_Num_x = (x0 + x1) / 2.0f;
			Block_Num_y = (y0 + y1) / 2.0f;
			isHidden = true;
		}

		//初始化区块类存储的actor指针
		void Block_Actors_init(int actors_num) {
			Block_Actors.Reserve(actors_num);
		}

	};
	
	//存储所有区块的字典<区块左下角坐标，区块对象>
	std::unordered_map <std::string, Block_Class* > All_Blocks_map;

	//存储已经加载的区块
	std::unordered_map <std::string, Block_Class* > Loading_Blocks_map;

	//存储需要加载的区块，方便计算
	std::unordered_map <std::string, Block_Class* > NeedLoading_Blocks_map;

	//存储需要隐藏的区块
	std::unordered_map <std::string, Block_Class* > Hidden_Blocks_map;
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void BeginPlay() override;
	//统计所有区块
	void CountBlocks();
	//系统启动后计算第一次需要加载的区块
	void FirstLoadBlocks();
	//计算需要更新的区块
	void PreUpdateBlock();
	//进行区块更新
	void UpdateBlock();
	void TriggerAsyncTask();

	
public:
	AActor* player_camera;
	//区块边长，单位虚幻自带
	int max_block_size = 1000;
	//区块加载边长,单位区块
	int max_load_size = 10;
	//地图边长，单位区块
	int max_size = 100;
	//摄像头位置x
	int camera_x;
	//摄像头位置y
	int camera_y;
	FDelegateHandle DirWatcherHandle;
};
