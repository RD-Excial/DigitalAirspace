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
    // ��ҿ��޸ĵı�����֧���ڱ༭��������ʱ�޸ģ�
    UPROPERTY(config, EditAnywhere, Category = "Player Settings")
    float base_x = 10.0f;

    UPROPERTY(config, EditAnywhere, Category = "Player Settings")
    float base_y = 0.0f;

    UPROPERTY(config, EditAnywhere, Category = "Player Settings")
    //��ʼ����
    float base_latitude = 0;

    UPROPERTY(config, EditAnywhere, Category = "Player Settings")
    //��ʼγ��
    float base_longitude = 0;

    UPROPERTY(config, EditAnywhere, Category = "Player Settings")
    //3.14
    float P = 0;

    UPROPERTY(config, EditAnywhere, Category = "Player Settings")
    //�����ļ���ȡ·��
    FString PointFilePath = "D:/6t6";

    //���ݿ����
    UPROPERTY(config, EditAnywhere, Category = "Player Settings")
    FString SQLlink = "tcp://localhost:3306";
    UPROPERTY(config, EditAnywhere, Category = "Player Settings")
    FString SQLname = "root";
    UPROPERTY(config, EditAnywhere, Category = "Player Settings")
    FString SQLpassword = "qq897422796";


};
