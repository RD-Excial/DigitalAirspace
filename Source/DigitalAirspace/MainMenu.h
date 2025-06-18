// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Blueprint/UserWidget.h"
#include "Components/ComboBoxString.h"
#include "Components/TextBlock.h"
#include "PlyTArrayStruct.h"
#include "Kismet/GameplayStatics.h"     // UGameplayStatics
#include "Engine/StaticMeshActor.h"     // AStaticMeshActor
#include "filewatch.hpp"
#include "WeatherWordActor.h"
#include "AirBlockClass.h"
#include "DynamicMeshActor.h"
#include "MySQLLink.h"
#include "CoreMinimal.h"
#include "Components/CheckBox.h"
#include "PathViewActor.h"
#include "PlyPaser.h"
#include "D:\EPIC\UE_4.27\Engine\Plugins\Marketplace\WeatherSystemPlugin\Source\WeatherSystem\Public\WeatherSystemMaster.h"
#include "MainMenu.generated.h"
/**
 * 
 */
UCLASS()
class DIGITALAIRSPACE_API UMainMenu : public UUserWidget
{

	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    TArray<FString> obj_file_string;
    TArray<FString> Inf_Weather;
    FString Weather_Fstring;
    FString Time_Fstring;
    AWeatherSystemMaster* weathercontroller;

    //读取配置文件
    void GetUserInI();

    //初始经度
    float base_latitude = 0;
    //初始纬度
    float base_longitude = 0;
    //初始x
    float base_x = 0;
    //初始y
    float base_y = 0;
    //当前经度
    float latitude = 0;
    //当前纬度
    float longitude = 0;

    std::string filename;

    UFUNCTION(BlueprintCallable,Category = "ChangeTime")
    void Change_Time_();
    

    UPROPERTY(EditAnywhere, Category = "Blueprint")
    TSubclassOf<AActor> GoodSkyClass;

    AActor* SkyActor;

    bool is_getting_message_from_api=false;

    float LastAPICallTime = 0.0f;
    const float API_CallInterval = 60.0f;
    //地球平均半径
    const float R = 6378137;
    //3.14
    const float P = 3.1415926;

protected:
    //按钮
    UPROPERTY(meta = (BindWidget))
    class UButton* ImportPointCloudMap;
    UPROPERTY(meta = (BindWidget))
    class UButton* ChangeWeather;
    UPROPERTY(meta = (BindWidget))
    class UButton* ChangeWeather_diy;
    UPROPERTY(meta = (BindWidget))
    class UButton* test_button_load;
    UPROPERTY(meta = (BindWidget))
    class UButton* change_position_button;
    UPROPERTY(meta = (BindWidget))
    class UButton* button_loadpath;

    //文本
    UPROPERTY(meta = (BindWidget))
    class UEditableText* citycode_input;
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Weather_Text;
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Position_Text;
    UPROPERTY(meta = (BindWidget))
    class UComboBoxString* WeatherChangeChoose;
    UPROPERTY(meta = (BindWidget))
    class UEditableText* time_input;
    UPROPERTY(meta = (BindWidget))
    class UEditableText* winddirection_input;
    UPROPERTY(meta = (BindWidget))
    class UEditableText* windpower_input;
    UPROPERTY(meta = (BindWidget))
    class UEditableText* province_input;
    UPROPERTY(meta = (BindWidget))
    class UEditableText* latitude_input;
    UPROPERTY(meta = (BindWidget))
    class UEditableText* longitude_input;

    //快速移动
    UPROPERTY(meta = (BindWidget))
    class UEditableText* longitude_input_set;
    UPROPERTY(meta = (BindWidget))
    class UEditableText* latitude_input_set;
    UPROPERTY(meta = (BindWidget))
    class UEditableText* x_input_set;
    UPROPERTY(meta = (BindWidget))
    class UEditableText* y_input_set;
    UPROPERTY(meta = (BindWidget))
    class UButton* change_position_button_latlon;
    UPROPERTY(meta = (BindWidget))
    class UButton* change_position_button_xy;
    //选框
    UPROPERTY(meta = (BindWidget))
    class UCheckBox* CheckBox_GridShow;
    UPROPERTY(meta = (BindWidget))
    class UCheckBox* CheckBox_AirLineShow;
    
    UFUNCTION()
    void PointCloudButtonClicked();
    UFUNCTION()
    void WeatherAPIButtonClicked();
    UFUNCTION()
    void OnTextChanged();
    UFUNCTION()
    void WeatherAPIButton_diy_Clicked();

    UFUNCTION()
    void LoadAirGridSystem();

    UFUNCTION()
    void LoadPath();

    //网格和航迹显示
    UFUNCTION()
    void OnGridShowChange(bool show);
    UFUNCTION()
    void OnAirLineShowChange(bool show);

    //快速移动函数
    UFUNCTION()
    void Quickmove_latlon();
    UFUNCTION()
    void Quickmove_xy();

    FString SelectedFilePath;
    void OpenFileDialog();
    void plytomesh(FVector Location);
    void LoadPlyMesh(PlyPaser& ply2mesh, FVector Location);

    void ChangeWorldWeather();

    //ue4坐标转经纬度
    void UE_to_Latitude_Longitude();
    //经纬度转ue4坐标
    FVector Latitude_Longitude_to_UE(FVector f);
    UFUNCTION()
    void Change_BaseCoordinate();

    //自动加载点云文件
    void AutoLoadPoint();
    //点云监控路径
    FString WatchDir="";
    TUniquePtr<filewatch::FileWatch<std::string>> FileWatcher;
    void PointFileChange(const std::string path, const filewatch::Event change_type);
    FDelegateHandle DirWatcherHandle;

    //路径数组
    TArray<APathViewActor*> allPath;

    //网格
    AirBlockClass* ABC=nullptr;
    bool ShowGrid = false;
    bool ShowLine = false;
    //天气文字
    TMap<FVector,AWeatherWordActor*> allWeatherWord;

    //数据库类实例
    MySQLLink* m=nullptr;
    // 绑定委托
    void BindToMySQLLink(class MySQLLink* m_bind);

    // 回调函数
    UFUNCTION()
    void ChangeLocate(FVector f,FString fs);
    UFUNCTION()
    void ShowPath(TArray<FVector> AllPoints);
    void ChangeGridMessage(WeatherDataClass WDC);

    //所有点云模型
    TMap<FString, ADynamicMeshActor*> allPlyMesh;

    //演示用无人机
    AActor* show_uav = nullptr;
    TArray<FVector> uav_line;
    bool ismove = false;
    void Moveuav(float DeltaTime);
    float move_speed = 100.0f;
};
