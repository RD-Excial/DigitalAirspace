// Fill out your copyright notice in the Description page of Project Settings.


#include "PathViewActor.h"
#include "Misc/FileHelper.h"

// Sets default values
APathViewActor::APathViewActor()
{
    PrimaryActorTick.bCanEverTick = false;

    FilePath = FPaths::ProjectDir() + "Content/test.txt";

    LineBatchComponent = CreateDefaultSubobject<ULineBatchComponent>(TEXT("LineBatchComponent"));
    LineBatchComponent->SetupAttachment(RootComponent);

    // 默认设置
    LineThickness = 0.1f;


}

void APathViewActor::SetPoints()
{
    //读取文件并获取位置
    

    // 检查文件是否存在
    if (!FPaths::FileExists(FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("当前位置未找到坐标文件: %s"), *FilePath);
        return;
    }

    // 读取文件内容
    FString FileContent;
    if (!FFileHelper::LoadFileToString(FileContent, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("读取文件失败: %s"), *FilePath);
        return;
    }

    // 按行分割
    TArray<FString> Lines;
    FileContent.ParseIntoArrayLines(Lines);

    // 解析每一行
    for (const FString& Line : Lines)
    {
        FVector Vec;
        TArray<FString> Tokens;
        Line.ParseIntoArray(Tokens, TEXT(" "), true); // 按空格分割

        if (Tokens.Num() >= 3)
        {
            float X = FCString::Atof(*Tokens[0]);
            float Y = FCString::Atof(*Tokens[1]);
            float Z = FCString::Atof(*Tokens[2]);
            Vec = FVector(X, Y, Z);
        }
        else {
            UE_LOG(LogTemp, Warning, TEXT("出错: %s"), *Line);
            Vec = FVector::ZeroVector; // 返回零向量表示解析失败
        }


        if (!Vec.IsZero()) // 确保解析成功
        {
            Coordinates.Add(Vec);
        }
    }
    // 更新网格
    UpdateSplineMesh();
}

void APathViewActor::UpdateSplineMesh()
{
    LineBatchComponent->Flush(); // 清除之前绘制的线条
    
    TArray<FBatchedLine> Lines;
    const FColor LineColor = FColor::Green;
    const float LifeTime = 0.0f; // 0表示永久显示

    // 创建连接所有点的线段
    for (int32 i = 0; i < Coordinates.Num() - 1; ++i)
    {
        //UE_LOG(LogTemp, Warning, TEXT("当前是第%d个坐标，x=%f，y=%f，z=%f"), i, Coordinates[i].X, Coordinates[i].Y, Coordinates[i].Z);
        //UE_LOG(LogTemp, Warning, TEXT("当前是第%d个坐标，x=%f，y=%f，z=%f"), i+1, Coordinates[i + 1].X, Coordinates[i + 1].Y, Coordinates[i + 1].Z);
        Lines.Add(FBatchedLine(
            Coordinates[i],
            Coordinates[i + 1],
            LineColor,
            LifeTime,
            2.0f,
            SDPG_World
        ));
    }
    
    LineBatchComponent->DrawLines(Lines);
}

// Called when the game starts or when spawned
void APathViewActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APathViewActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APathViewActor::Hide(bool show)
{
    LineBatchComponent->SetHiddenInGame(!show);
}

