// Fill out your copyright notice in the Description page of Project Settings.


#include "WeatherWordActor.h"
#include "Engine/Font.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AWeatherWordActor::AWeatherWordActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	TextComp = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRender"));
	//static ConstructorHelpers::FObjectFinder<UFont> FontObj(TEXT("Font'/Game/ui/MSYH_Font.MSYH_Font'"));
	//if (FontObj.Succeeded())
	//{
	//	TextComp->SetFont(FontObj.Object);
	//	UE_LOG(LogTemp, Warning, TEXT("字体创建成功"));
	//}
	TextComp->SetWorldSize(3000.0f);
}

void AWeatherWordActor::IniWWA(FString weather, FVector location)
{
	
	TextComp->SetText(FText::FromString(weather));
	TextComp->SetWorldLocation(location);
	TextComp->SetHorizontalAlignment(EHTA_Center);
	UE_LOG(LogTemp, Warning, TEXT("创建了天气文字%s"),*weather);
	//RootComponent = TextComp; // 或者 Attach到其他组件
}

void AWeatherWordActor::ChangeWeather(FString weather)
{
	TextComp->SetText(FText::FromString(weather));
}

// Called when the game starts or when spawned
void AWeatherWordActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeatherWordActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!TextComp) return;
	


	// 获取玩家摄像机
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	float Distance = FVector::Dist(PC->GetPawn()->GetActorLocation(), GetActorLocation());
	float VisibleRange = 20000.0f; // 1000cm = 10米

	if (Distance <= VisibleRange&&isshow)
	{
		TextComp->SetVisibility(true);
	}
	else
	{
		TextComp->SetVisibility(false);
	}
	if (PC && PC->PlayerCameraManager)
	{
		FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();

		// 让文字面向摄像机
		FVector ToCamera = CameraLocation - TextComp->GetComponentLocation();
		FRotator LookAtRotation = FRotationMatrix::MakeFromX(ToCamera).Rotator();

		// 只旋转Yaw（保持文字不倒）
		LookAtRotation.Pitch = 0.0f;
		LookAtRotation.Roll = 0.0f;

		TextComp->SetWorldRotation(LookAtRotation);
	}
}

