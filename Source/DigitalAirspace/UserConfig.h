// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UserConfig.generated.h"

/**
 * 
 */
UCLASS(config = UserConfig, DefaultConfig)
class DIGITALAIRSPACE_API UUserConfig : public UObject
{
	GENERATED_BODY()
public:
    // 玩家可修改的变量（支持在编辑器和运行时修改）
    UPROPERTY(config, EditAnywhere, Category = "Player Settings")
    float base_x = 10.0f;

    UPROPERTY(config, EditAnywhere, Category = "Player Settings")
    float base_y = 0.0f;

    UPROPERTY(config, EditAnywhere, Category = "Player Settings")
    //初始经度
    float base_latitude = 0;

    UPROPERTY(config, EditAnywhere, Category = "Player Settings")
    //初始纬度
    float base_longitude = 0;

    UPROPERTY(config, EditAnywhere, Category = "Player Settings")
    //3.14
    float P = 0;

    UPROPERTY(config, EditAnywhere, Category = "Player Settings")
    //点云文件读取路径
    FString PointFilePath = "D:/6t6";

    //数据库相关
    UPROPERTY(config, EditAnywhere, Category = "Player Settings")
    FString SQLlink = "tcp://localhost:3306";
    UPROPERTY(config, EditAnywhere, Category = "Player Settings")
    FString SQLname = "root";
    UPROPERTY(config, EditAnywhere, Category = "Player Settings")
    FString SQLpassword = "qq897422796";


};
