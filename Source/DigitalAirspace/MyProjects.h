// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyProjects.generated.h"
namespace MyGlobalVariables {
    //初始经度
    extern float base_latitude;
    //初始纬度
    extern float base_longitude;
    //初始x
    extern float base_x;
    //初始y
    extern float base_y;

    extern float P;
}


/**
 * 
 */
UCLASS()
class DIGITALAIRSPACE_API UMyProjects : public UGameInstance
{
	GENERATED_BODY()
public:
    //初始经度
    float base_latitude = 0;
    //初始纬度
    float base_longitude = 0;
    //初始x
    float base_x = 0;
    //初始y
    float base_y = 0;

    float P = 3.14;

    FVector Latitude_Longitude_to_UE(FVector f)
    {
        FVector r;
        r.X = base_x + 6378137 * FMath::Cos(base_latitude * (P / 180)) * (f.X - base_longitude) * (P / 180) * 100;
        r.Y = base_y + 6378137 * (f.Y - base_latitude) * (P / 180) * 180;
        r.Z = f.Z * 100;
        return r;
    }
};
