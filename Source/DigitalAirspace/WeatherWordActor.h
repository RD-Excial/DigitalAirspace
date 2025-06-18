// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "WeatherWordActor.generated.h"

UCLASS()
class DIGITALAIRSPACE_API AWeatherWordActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeatherWordActor();

	UTextRenderComponent* TextComp = nullptr;
	void IniWWA(FString weather,FVector location);
	void ChangeWeather(FString weather);
	bool isshow = false;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
