// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/LineBatchComponent.h"
#include "PathViewActor.generated.h"

UCLASS()
class DIGITALAIRSPACE_API APathViewActor : public AActor
{
	GENERATED_BODY()
	
public:	

	FVector Start = FVector(0,0,0);
	FVector End = FVector(100, 100, 100);
	FString FilePath;
	TArray<FVector> Coordinates;
	// Sets default values for this actor's properties
	APathViewActor();
	// 设置起点和终点
	void SetPoints();

	// 更新线条外观
	void UpdateSplineMesh();

	UPROPERTY(VisibleAnywhere)
	ULineBatchComponent* LineBatchComponent;

	// 线条粗细
	UPROPERTY(EditAnywhere, Category = "Spline")
	float LineThickness=10.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Hide(bool show);

};
