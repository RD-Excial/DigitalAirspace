// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicMeshActor.h"
#include "D:/EPIC/UE_4.27/Engine/Plugins/Runtime/ProceduralMeshComponent/Source/ProceduralMeshComponent/Public/ProceduralMeshComponent.h"



// Sets default values
ADynamicMeshActor::ADynamicMeshActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
    RootComponent = MeshComponent;
}

// Called when the game starts or when spawned
void ADynamicMeshActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADynamicMeshActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADynamicMeshActor::CreateMesh(const TArray<FVector>& Vertices, const TArray<int32>& Triangles) {
    TArray<FVector> Normals;
    TArray<FVector2D> UVs;
    TArray<FColor> VertexColors;
    TArray<FProcMeshTangent> Tangents;

    // Generate Normals, UVs, Tangents, and Vertex Colors if needed (optional)

    // 创建网格部分
    try
    {
        //CreateMeshSections(Vertices, Triangles);
        MeshComponent->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);
        UE_LOG(LogTemp, Log, TEXT("Mesh section created successfully."));
    }
    catch (const std::exception& e)
    {
        UE_LOG(LogTemp, Error, TEXT("Exception caught: %s"), e.what());
    }
    catch (...)
    {
        UE_LOG(LogTemp, Error, TEXT("Unknown exception caught."));
    }
    // 设置材质（如需要）
    // MeshComponent->SetMaterial(0, YourMaterialInstance);
}

// 分块处理数据并创建网格体
void ADynamicMeshActor::CreateMeshSections(const TArray<FVector>& Vertices, const TArray<int32>& Triangles)
{
    int32 TotalVertices = Vertices.Num();
    int32 TotalTriangles = Triangles.Num();
    int32 MaxVerticesPerSection = Triangles.Num()/3;
    TArray<int32> SectionTriangles;
    SectionTriangles.Add(0);
    SectionTriangles.Add(1);
    SectionTriangles.Add(2);

    for (int32 i = 0; i < TotalTriangles; i += 3)
    {
        //system("pause");
        //int32 SectionVerticesCount = FMath::Min(MaxVerticesPerSection, TotalVertices - i);
        TArray<FVector> SectionVertices;

        // 分块处理顶点数据
        SectionVertices.Add(Vertices[Triangles[i]]);
        SectionVertices.Add(Vertices[Triangles[i+1]]);
        SectionVertices.Add(Vertices[Triangles[i+2]]);
        UE_LOG(LogTemp, Log, TEXT("面索引%f %f %f "), SectionVertices[0].X, SectionVertices[0].Y, SectionVertices[0].Z);
        UE_LOG(LogTemp, Log, TEXT("面索引%f %f %f "), SectionVertices[1].X, SectionVertices[1].Y, SectionVertices[1].Z);
        UE_LOG(LogTemp, Log, TEXT("面索引%f %f %f "), SectionVertices[2].X, SectionVertices[2].Y, SectionVertices[2].Z);
        // 分块处理索引数据
        UE_LOG(LogTemp, Log, TEXT("开始分段创建网格."));
        
        UE_LOG(LogTemp, Log, TEXT("网格第%d段生成中"), i);
        // 创建网格体部分
        MeshComponent->CreateMeshSection(0, SectionVertices, SectionTriangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);
        //system("pause");
    }
}

