// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"

#include "DynamicMeshActor.generated.h"

UCLASS()
class DIGITALAIRSPACE_API ADynamicMeshActor : public AActor
{
    GENERATED_BODY()
public:
    ADynamicMeshActor();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    //UFUNCTION(BlueprintCallable, Category = "Mesh")
    void CreateMesh(const TArray<FVector>& Vertices, const TArray<int32>& Triangles);
    void CreateMeshSections(const TArray<FVector>& Vertices, const TArray<int32>& Triangles);

    //UPROPERTY(VisibleAnywhere)
    UProceduralMeshComponent* MeshComponent;

};
