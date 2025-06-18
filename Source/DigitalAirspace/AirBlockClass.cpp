// Fill out your copyright notice in the Description page of Project Settings.


#include "AirBlockClass.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include <DrawDebugHelpers.h>

const int MAX_LEVEL = 6;

AirBlockClass::AirBlockClass(UWorld* w)
{
    world = w;
    AirGridCells.SetNum(GridDimensions.X);
    for (int32 x = 0; x < GridDimensions.X; ++x)
    {
        AirGridCells[x].SetNum(GridDimensions.Y);
        for (int32 y = 0; y < GridDimensions.Y; ++y)
        {
            AirGridCells[x][y].SetNum(GridDimensions.Z);
            for (int32 z = 0; z < GridDimensions.Z; ++z)
            {
                AirBlock& Cell = AirGridCells[x][y][z];
                Cell.GridCoordinates = FIntVector(x, y, z);
                Cell.id = nowid;
                nowid++;
                Cell.state = 0;
                Cell.level = 0;
                Cell.World_center_Location = WorldOrigin + FVector(x * Cell.size + Cell.size * 0.5f, y * Cell.size + Cell.size * 0.5f, z * Cell.size + Cell.size * 0.5f);
            }
        }
    }


    
}

AirBlockClass::~AirBlockClass()
{
}

FVector AirBlockClass::GridToWorld(const FIntVector& GridCoord) const
{
    return WorldOrigin + FVector(
        GridCoord.X * GridSize + GridSize * 0.5f,
        GridCoord.Y * GridSize + GridSize * 0.5f,
        GridCoord.Z * GridSize + GridSize * 0.5f
    );
}

void AirBlockClass::CalculateChildAirBlock(AirBlock& cell)
{
    float quarterSize = cell.size / 4.0f;
    float childSize = cell.size / 2.0f;

    // 8个子网格的中心坐标计算
    for (int32 i = 0; i < 8; i++)
    {
        FVector Offset(
            (i & 1) ? quarterSize : -quarterSize,
            (i & 2) ? quarterSize : -quarterSize,
            (i & 4) ? quarterSize : -quarterSize
        );
        AirBlock* newAirBlock = new AirBlock();
        newAirBlock->World_center_Location = cell.World_center_Location+Offset;
        newAirBlock->size = childSize;
        newAirBlock->GridCoordinates = cell.GridCoordinates;
        newAirBlock->parent = &cell;
        newAirBlock->level = cell.level + 1;
        newAirBlock->id = nowid;
        nowid++;
        cell.children.Add(newAirBlock);
    }
}

void AirBlockClass::UpdateGridFromWorld()
{
    // 进行物理扫描检测障碍物
    for (int32 x = 0; x < GridDimensions.X; ++x)
    {
        for (int32 y = 0; y < GridDimensions.Y; ++y)
        {
            for (int32 z = 0; z < GridDimensions.Z; ++z)
            {
                //world->GetTimerManager().SetTimerForNextTick([this, x, y, z]()
                //    {
                        DetectingObstacles(AirGridCells[x][y][z]);
                //    });
                
                //UE_LOG(LogTemp, Log, TEXT("结束检测"));
            }
        }
    }

    //for (FOverlapResult for1 : Overlaps) {
    //    UE_LOG(LogTemp, Warning, TEXT("检测到的碰撞体为%s"), *(for1.Actor.Get()->GetFName().ToString()));
    //}
}



void AirBlockClass::DetectingObstacles(AirBlock& cell)
{
    TArray<FOverlapResult> Overlaps;
    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = true;
    FVector GridExtent = FVector(cell.size * 0.5f);
    FVector CellCenter = cell.World_center_Location;
    //UE_LOG(LogTemp, Log, TEXT("网格大小为%f,中心为%f,%f,%f"),cell.size,CellCenter.X, CellCenter.Y, CellCenter.Z);
    if (world->OverlapMultiByChannel(
        Overlaps,          // 存储检测到的碰撞体结果数组
        CellCenter,        // 碰撞形状中心的位置（FVector类型）
        FQuat::Identity,   // 碰撞形状的旋转（这里使用无旋转）
        ECC_Visibility,   // 碰撞通道：检测静态世界物体
        FCollisionShape::MakeBox(GridExtent), // 创建盒体碰撞形状，使用给定的范围（GridExtent）
        QueryParams))      // 碰撞查询参数（默认不忽略）
    {
        //UE_LOG(LogTemp, Log, TEXT("找到了碰撞物体区块"));
        // 如果检测到重叠（返回true），则执行以下代码块
        // Overlaps数组中现在包含所有与指定盒体重叠的组件/actor信息
        //UE_LOG(LogTemp, Log, TEXT("找到了不可通行区块"));
        for (auto& ac : Overlaps) {
            //UE_LOG(LogTemp, Log, TEXT("actor为%s"), *(ac.Actor.Get()->GetFName().ToString()));
            if (ac.Actor.Get()->Tags.Num()>0) {
                cell.towerid = TCHAR_TO_UTF8(*ac.Actor.Get()->GetFName().ToString());
                UE_LOG(LogTemp, Log, TEXT("actor有tag，为%s"), *(ac.Actor.Get()->GetFName().ToString()));
            }
        }
        
        if (cell.level >= MAX_LEVEL) {
            cell.state = 1;
        }
        else {
            cell.state = 2;
            //创建八叉树并逐一进行检查
            //UE_LOG(LogTemp, Log, TEXT("正在创建八叉树"));
            CalculateChildAirBlock(cell);
            for (AirBlock* a : cell.children) {
                DetectingObstacles(*a);
            }

        }
            


    }
    //UE_LOG(LogTemp, Log, TEXT("没找到碰撞物体区块"));
}

void DrawDebug(const AirBlockClass::AirBlock& a,UWorld& w) {

    FColor Color = FColor::Red;

    //如果a距离摄像机太远就不绘制
    APlayerController* PC =w.GetFirstPlayerController();
    if (PC && PC->GetPawn())
    {
        FVector PlayerLocation = PC->GetPawn()->GetActorLocation();
        if (FVector::Distance(a.World_center_Location, PlayerLocation) < 20000.0f) {
            DrawDebugBox(
                &w,
                a.World_center_Location,
                FVector(a.size * 0.5f),
                Color,
                false,
                0.2f,
                0,
                10.0f
            );
        }
    }

    
    for (AirBlockClass::AirBlock* ab : a.children) {
        DrawDebug(*ab, w);
    }
}

void AirBlockClass::DrawDebugGrid(UWorld* World, float Duration) const
{
    if (!World) return;
    //UE_LOG(LogTemp, Log, TEXT("开始绘制"));
    for (int32 x = 0; x < GridDimensions.X; ++x)
    {
        for (int32 y = 0; y < GridDimensions.Y; ++y)
        {
            for (int32 z = 0; z < GridDimensions.Z; ++z)
            {
                const AirBlock& Cell = AirGridCells[x][y][z];
                DrawDebug(Cell, *World);
            }
        }
    }
}



// 首先为AirBlock实现一个ToString函数


void AirBlockClass::Save3DAirBlockArrayToFile(const FString& FilePath)
{
    
}

// 调用示例

