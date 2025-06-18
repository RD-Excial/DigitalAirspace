// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include <fstream>
#include <map>
#include <functional>
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Misc/Paths.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CheatManager.h"
#include "HAL/PlatformFilemanager.h"
#include "CoreMinimal.h"
#include "Engine/World.h"
#include "UserConfig.h"
#include "MyProjects.h"
#include "GameFramework/Character.h"
#include "GetWeatherAPI.h"
#include "SocketLink.h"
#include "EngineUtils.h"
#include "JsonRead.h"
#include "Components/SplineComponent.h"
#include <chrono>
#include <iomanip>
#include "Misc/FileHelper.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"
#include <Misc/MessageDialog.h>

FVector LatLon_to_UE(FVector f)
{
    const UUserConfig* Config = GetDefault<UUserConfig>();
    FVector r;
    //X对应经度，Y对应纬度，Z对应高度
    r.X = Config->base_x + 6378137 * FMath::Cos(Config->base_latitude * (Config->P / 180)) * (f.X - Config->base_longitude) * (Config->P / 180) * 100;
    r.Y = Config->base_y + 6378137 * (f.Y - Config->base_latitude) * (Config->P / 180) * 100;
    r.Z = f.Z * 100;
    return r;
}
//天气对照表,分别为晴天，阴天，雨天，雨雪天，雪天五类
const std::map<std::string, std::string> weatherMap = {
        {"晴朗", "晴天"},
        {"晴", "晴天"},
        {"少云", "晴天"},
        {"晴间多云", "晴天"},
        {"多云", "晴天"},
        {"阴", "阴天"},
        {"有风", "晴天"},
        {"平静", "晴天"},
        {"微风", "晴天"},
        {"和风", "晴天"},
        {"清风", "晴天"},
        {"强风/劲风", "晴天"},
        {"疾风", "晴天"},
        {"大风", "晴天"},
        {"烈风", "晴天"},
        {"风暴", "晴天"},
        {"狂爆风", "晴天"},
        {"飓风", "晴天"},
        {"热带风暴", "晴天"},
        {"霾", "阴天"},
        {"中度霾", "阴天"},
        {"重度霾", "阴天"},
        {"严重霾", "阴天"},
        {"雨天", "雨天"},
        {"阵雨", "雨天"},
        {"雷阵雨", "雨天"},
        {"雷阵雨并伴有冰雹", "雨天"},
        {"小雨", "雨天"},
        {"中雨", "雨天"},
        {"大雨", "雨天"},
        {"暴雨", "雨天"},
        {"大暴雨", "雨天"},
        {"特大暴雨", "雨天"},
        {"强阵雨", "雨天"},
        {"强雷阵雨", "雨天"},
        {"极端降雨", "雨天"},
        {"毛毛雨/细雨", "雨天"},
        {"雨", "雨天"},
        {"小雨-中雨", "雨天"},
        {"中雨-大雨", "雨天"},
        {"大雨-暴雨", "雨天"},
        {"暴雨-大暴雨", "雨天"},
        {"大暴雨-特大暴雨", "雨天"},
        {"雨雪天", "雨雪天"},
        {"雨雪天气", "雨雪天"},
        {"雨夹雪", "雨雪天"},
        {"阵雨夹雪", "雨雪天"},
        {"冻雨", "雨雪天"},
        {"雪天", "雪天"},
        {"雪", "雪天"},
        {"阵雪", "雪天"},
        {"小雪", "雪天"},
        {"中雪", "雪天"},
        {"大雪", "雪天"},
        {"暴雪", "雪天"},
        {"小雪-中雪", "雪天"},
        {"中雪-大雪", "雪天"},
        {"大雪-暴雪", "雪天"},
        {"浮尘", "阴天"},
        {"扬沙", "阴天"},
        {"沙尘暴", "阴天"},
        {"强沙尘暴", "阴天"},
        {"龙卷风", "阴天"},
        {"雾", "阴天"},
        {"浓雾", "阴天"},
        {"强浓雾", "阴天"},
        {"轻雾", "阴天"},
        {"大雾", "阴天"},
        {"特强浓雾", "阴天"},
        {"热", "晴天"},
        {"冷", "阴天"},
        {"未知", "阴天"}
}; \

TArray<AActor*> RuntimeObjects;

//初始化函数
void UMainMenu::NativeConstruct()
{
    Super::NativeConstruct();

    //获取配置文件数据
    GetUserInI();
    //连接按钮和函数
    if (ImportPointCloudMap)
    {
        ImportPointCloudMap->OnClicked.AddDynamic(this, &UMainMenu::PointCloudButtonClicked);
    }

    if (ChangeWeather) {
        ChangeWeather->OnClicked.AddDynamic(this, &UMainMenu::WeatherAPIButtonClicked);
    }
    if (ChangeWeather_diy) {
        ChangeWeather_diy->OnClicked.AddDynamic(this, &UMainMenu::WeatherAPIButton_diy_Clicked);
    }
    if (change_position_button) {
        //UE_LOG(LogTemp, Warning, TEXT("成功找到按钮"));
        change_position_button->OnClicked.AddDynamic(this, &UMainMenu::Change_BaseCoordinate);
    }
    if (test_button_load) {
        test_button_load->OnClicked.AddDynamic(this, &UMainMenu::LoadAirGridSystem);
        
    }
    if (button_loadpath) {
        button_loadpath->OnClicked.AddDynamic(this, &UMainMenu::LoadPath);
    }
    if (CheckBox_GridShow)
    {
        CheckBox_GridShow->OnCheckStateChanged.AddDynamic(this, &UMainMenu::OnGridShowChange);
    }
    if (CheckBox_AirLineShow)
    {
        CheckBox_AirLineShow->OnCheckStateChanged.AddDynamic(this, &UMainMenu::OnAirLineShowChange);
    }
    if (change_position_button_latlon) {
        change_position_button_latlon->OnClicked.AddDynamic(this, &UMainMenu::Quickmove_latlon);
    }
    if (change_position_button_xy) {
        change_position_button_xy->OnClicked.AddDynamic(this, &UMainMenu::Quickmove_xy);
    }
    else {
        //UE_LOG(LogTemp, Warning, TEXT("没有找到按钮"));
    }

    //寻找天气对象
    for (TActorIterator<AWeatherSystemMaster> It(GetWorld()); It; ++It)
    {
        if (It->GetName() == "WeatherSystemMaster1_Blueprint")
        {
            weathercontroller = *It;
            //UE_LOG(LogTemp, Warning, TEXT("成功找到天气组件"));
            //修改天气
            weathercontroller->AlwaysSunny = true;
            break;
        }
    }

    //寻找天色对象
    UClass* BlueprintActorClass = LoadClass<AActor>(nullptr, TEXT("/Game/GoodSky/Blueprint/BP_GoodSky.BP_GoodSky_C"));
    if (BlueprintActorClass)
    {
        FActorSpawnParameters SpawnParams;
        SkyActor = GetWorld()->SpawnActor<AActor>(BlueprintActorClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("没找到天色组件"));
    }


    //初始化数据库实例
    m = new MySQLLink();
    //设定点云文件读取路径
    AutoLoadPoint();

    BindToMySQLLink(m);
    
    //建立http连接
    if (m) {
        SocketLink* s = new SocketLink(*m);
        s->StartHttpLink();
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("数据库实例创建失败"));
    }
    
    //寻找用于演示飞行路径的无人机
    TArray<AActor*> MeshActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStaticMeshActor::StaticClass(), MeshActors);
    for (AActor* ama : MeshActors) {
        if (ama->ActorHasTag("show_uav")) {
            UE_LOG(LogTemp, Warning, TEXT("找到了演示用无人机"));
            show_uav = ama;
        }
    }

    //生成一个天气测试
    /*FString awfs = "clear day";
    FActorSpawnParameters SpawnParams;
    AWeatherWordActor* aw = GetWorld()->SpawnActor<AWeatherWordActor>(
        AWeatherWordActor::StaticClass(),
        FVector(0, 0, 0),
        FRotator::ZeroRotator,
        SpawnParams
    );
    aw->IniWWA(awfs, FVector(0, 0, 300));*/
}

//每tick调用
void UMainMenu::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
    Super::NativeTick(MyGeometry, InDeltaTime);
    //UE_LOG(LogTemp, Warning, TEXT("正在每帧调用"));
    //更新天气信息
    if (is_getting_message_from_api) {
        if (FPlatformTime::Seconds() - LastAPICallTime >= API_CallInterval) {
            WeatherAPIButtonClicked();
            LastAPICallTime = FPlatformTime::Seconds();
        }
    }
    //更新演示用无人机位置
    Moveuav(InDeltaTime);

    //更新当前位置
    UE_to_Latitude_Longitude();

    //显示空域网格
    if (ABC && ShowGrid) {
        ABC->DrawDebugGrid(GetWorld(),1);
        for (const TPair<FVector, AWeatherWordActor*> awwa : allWeatherWord) {
            awwa.Value->isshow = true;
        }
    }
    else if(!ShowGrid){
        for (const TPair<FVector, AWeatherWordActor*> awwa : allWeatherWord) {
            awwa.Value->isshow = false;
        }
    }
}

// 获取配置
void UMainMenu::GetUserInI()
{
    
    UUserConfig* Config = GetMutableDefault<UUserConfig>();
    // 创建配置文件
    Config->SaveConfig();
    //UE_LOG(LogTemp, Warning, TEXT("创建配置文件成功"));
    WatchDir = Config->PointFilePath;
    base_latitude = Config->base_latitude;
    base_longitude = Config->base_longitude;
    base_x = Config->base_x;
    base_y = Config->base_y;
}


//UE转经纬度
void UMainMenu::UE_to_Latitude_Longitude() {
    float x = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation().X;
    float y = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation().Y;
    longitude = base_longitude - (x - base_x) / 100 / (R * FMath::Cos(base_longitude) * (P / 180));
    if (longitude > 180|| longitude<-180) {
        longitude = -longitude;
    }
    latitude = base_latitude + (y - base_y) / 100 / (R * FMath::Cos(base_latitude) * (P / 180));
    APlayerController* p1 = UGameplayStatics::GetPlayerController(this, 0);
    float f1 = p1->GetControlRotation().Yaw;
    // 转换为地理方位角（yaw为90°=北）
    FString f_string;
    if (f1 > 90 && f1 < 180) {
        f_string = TEXT("北偏东") + FString::Printf(TEXT("%f"), f1 - 90) + TEXT("°");
    }
    if (f1 > 180 && f1 < 270) {
        f_string = TEXT("南偏东") + FString::Printf(TEXT("%f"), 270 - f1) + TEXT("°");
    }
    if (f1 > 270 && f1 < 360) {
        f_string = TEXT("南偏西") + FString::Printf(TEXT("%f"), f1 - 270) + TEXT("°");
    }
    if (f1 > 0 && f1 < 90) {
        f_string = TEXT("北偏西") + FString::Printf(TEXT("%f"), 90 - f1) + TEXT("°");
    }
    FString p_string = FString::Printf(TEXT("%s\nx：%f，y：%f\n经度：%f\n纬度：%f"),*f_string, x, y, longitude, latitude);
    Position_Text->SetText(FText::FromString(p_string));
}

FVector UMainMenu::Latitude_Longitude_to_UE(FVector f)
{
    FVector r;
    r.X = base_x + 6378137 * FMath::Cos(base_latitude * (P / 180)) * (f.X - base_longitude) * (P / 180) * 100;
    r.Y = base_y + 6378137 * (f.Y - base_latitude) * (P / 180) * 100;
    r.Z = f.Z * 100;
    return r;
}

//更改基础位置信息
void UMainMenu::Change_BaseCoordinate() {
    base_x = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation().X;
    base_y = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation().Y;
    base_longitude = FCString::Atof(*(longitude_input->GetText().ToString()));
    base_latitude = FCString::Atof(*(latitude_input->GetText().ToString()));
    
    // 获取配置
    UUserConfig* Config = GetMutableDefault<UUserConfig>();
    Config->base_latitude= base_latitude;
    Config->base_longitude= base_longitude;
    Config->base_x= base_x;
    Config->base_y= base_y;
    // 创建配置文件
    Config->SaveConfig();
    
}

//监控文件夹
void UMainMenu::AutoLoadPoint()
{
    /*FString WatchDir;
    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    if (DesktopPlatform)
    {
        // 初始化文件对话框参数
        FString DefaultPath = FPaths::ProjectDir(); // 默认路径为项目目录
        FString OutFile;

        // 打开文件对话框
        bool bOpened = DesktopPlatform->OpenDirectoryDialog(
            nullptr,
            TEXT("选择一个文件夹"),
            DefaultPath,
            OutFile
        );
        // 保存选择的文件路径
        WatchDir = OutFile;
    }*/
    //遍历所有文件
    // 获取文件管理器实例
    IFileManager& FileManager = IFileManager::Get();

    // 定义文件搜索模式（* 匹配所有文件）
    FString SearchPattern = WatchDir + "/*.ply";
    UE_LOG(LogTemp, Warning, TEXT("文件搜索路径: %s"), *SearchPattern);
    // 遍历文件
    TArray<FString> FoundFiles;
    FileManager.FindFiles(FoundFiles, *SearchPattern, true, false); // nullptr 表示不包含子文件夹
    
    // 加载点云文件
    for (const FString& FilePath : FoundFiles)
    {
        UE_LOG(LogTemp, Warning, TEXT("找到了文件: %s"), *FilePath);
        SelectedFilePath = WatchDir +"/"+ FilePath;
        plytomesh(m->GetPointMessage(FilePath));
    }

    FileWatcher=MakeUnique<filewatch::FileWatch<std::string>>(std::string(TCHAR_TO_UTF8(*WatchDir)),
        [this](const std::string& path, const filewatch::Event change_type) {
            PointFileChange(path, change_type);
        }
    );
}



void UMainMenu::PointFileChange(const std::string path, const filewatch::Event change_type)
{
    FString Fpath = UTF8_TO_TCHAR(path.c_str());
    switch (change_type)
    {
    case filewatch::Event::added:
        
        //根据文件名在数据库中查找坐标
        UE_LOG(LogTemp, Log, TEXT("增加的文件为%s"), *Fpath);
        SelectedFilePath = WatchDir + "/" + Fpath;
        plytomesh(m->GetPointMessage(Fpath));
        break;
    case filewatch::Event::removed:
        // 文件被删
        break;
    case filewatch::Event::modified:
        // 文件被改
        break;
    case filewatch::Event::renamed_old:
    case filewatch::Event::renamed_new:
        // 文件被重命名
        break;
    }
}

void UMainMenu::BindToMySQLLink(MySQLLink* m_bind)
{
    if (m_bind && IsValid(this)) { // 双重检查
        UE_LOG(LogTemp, Warning, TEXT("绑定委托到 %s"), *GetName());
        m_bind->ply_update.AddUObject(this, &UMainMenu::ChangeLocate);
        m_bind->airline_update.AddUObject(this, &UMainMenu::ShowPath);
        m_bind->airgrid_update.AddUObject(this, &UMainMenu::ChangeGridMessage);
        // 验证绑定是否成功
        UE_LOG(LogTemp, Warning, TEXT("委托绑定状态: %d"),
            m_bind->airline_update.IsBound());
    }
}

void UMainMenu::ChangeLocate(FVector f,FString fs)
{
    UE_LOG(LogTemp, Warning, TEXT("更新后的%s的坐标为X%f,Y%f,Z%f"),*fs,f.X,f.Y,f.Z);
    if (allPlyMesh.Contains(fs)) {
        allPlyMesh[fs]->SetActorLocation(f);
    }
}

void UMainMenu::ShowPath(TArray<FVector> AllPoints)
{
    UE_LOG(LogTemp, Warning, TEXT("委托正在创建路径显示"));
    APathViewActor* aaa = GetWorld()->SpawnActor<APathViewActor>(
        //APathViewActor::StaticClass(),
        FVector::ZeroVector,  // 位置
        FRotator::ZeroRotator // 旋转
    );
    for (int i = 0; i < AllPoints.Num();i++) {
        AllPoints[i] = LatLon_to_UE(AllPoints[i]);
    }
    aaa->Coordinates = AllPoints;
    uav_line = AllPoints;
    show_uav->SetActorLocation(uav_line[0]);
    ismove = true;
    aaa->UpdateSplineMesh();
    aaa->Hide(true);
    allPath.Add(aaa);
}

void UMainMenu::ChangeGridMessage(WeatherDataClass WDC)
{
    UE_LOG(LogTemp, Warning, TEXT("委托正在更改网格数据%f"),WDC.wind_power);
    int minx = WDC.af.X / 20000;
    int miny = WDC.af.Y / 20000;
    int maxx = WDC.bf.X / 20000;
    int maxy = WDC.bf.Y / 20000;
    if (ABC) {
        for (int i = minx; i <= maxx; i++) {
            for (int k = miny; k <= maxy; k++) {
                for (int j = 0; j < ABC->AirGridCells[i][k].Num(); j++) {
                    ABC->AirGridCells[i][k][j].weather = TCHAR_TO_UTF8(*WDC.weather);
                    ABC->AirGridCells[i][k][j].wind_direction = TCHAR_TO_UTF8(*WDC.wind_direction);
                    ABC->AirGridCells[i][k][j].wind_power = WDC.wind_power;
                    allWeatherWord[FVector(i * 20000 + 10000, k * 20000 + 10000, j * 20000 + 10000)]->ChangeWeather(WDC.weather);
                }
            }
        }
    }
}

void UMainMenu::Moveuav(float DeltaTime)
{
    if (ismove&&uav_line.Num()>1) {
        // 在Tick里
        FVector Direction = (uav_line[1] - show_uav->GetActorLocation()).GetSafeNormal();
        float Distance = FVector::Dist(show_uav->GetActorLocation(), uav_line[1]);
        float Step = move_speed * DeltaTime;
        if (Distance > Step)
        {
            show_uav->SetActorLocation( show_uav->GetActorLocation() + Direction * Step);
        }
        else
        {
            show_uav->SetActorLocation(uav_line[1]); // 到达目标
            uav_line.RemoveAt(0);
        }
        if (uav_line.Num() == 1) {
            ismove = false;
        }
    }
}

void UMainMenu::Change_Time_() {
    
    float time_float=0;
    if (Time_Fstring.Len() != 0) {
        time_float = FCString::Atof(*Time_Fstring);
    }
    
    if (SkyActor)
    {
        // 修改时间
        FProperty* Property = SkyActor->GetClass()->FindPropertyByName(FName("Time of Day ( For Custom Mode )"));
        if (Property)
        {
            //UE_LOG(LogTemp, Warning, TEXT("找到对应变量"));
            // Cast to the appropriate property type (e.g., FIntProperty for int32)
            FFloatProperty* FloatProperty = CastField<FFloatProperty>(Property);
            if (FloatProperty)
            {
                // Set the property value
                FloatProperty->SetPropertyValue_InContainer(SkyActor, time_float);
            }
            else {
                UE_LOG(LogTemp, Warning, TEXT("变量类型不对"));
            }
        }
        else {
            UE_LOG(LogTemp, Warning, TEXT("未找到对应变量"));
        }
        // 进行初始化
        FName FunctionName = FName(TEXT("Init"));
        UFunction* Function = SkyActor->FindFunction(FunctionName);
        if (Function)
        {
            SkyActor->ProcessEvent(Function, nullptr);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("未找到对应函数%s"), *FunctionName.ToString());
        }
        
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BlueprintActorInstance is not valid!"));
    }
}

void UMainMenu::WeatherAPIButton_diy_Clicked() {
    is_getting_message_from_api = false;
    Weather_Fstring = WeatherChangeChoose->GetSelectedOption();
    Time_Fstring = time_input->GetText().ToString();
    ChangeWorldWeather();
    
}

//加载空域网格系统
void UMainMenu::LoadAirGridSystem()
{
    
    //测试一下空域网格系统
    AirBlockClass* abc = new AirBlockClass(GetWorld());
    ABC = abc;
    abc->UpdateGridFromWorld();

    //显示网格
    //保存到数据库中
    FText DialogText = FText::FromString(TEXT("数据正在保存中"));
    EAppReturnType::Type ReturnType = FMessageDialog::Open(EAppMsgType::Ok, DialogText);
    AsyncTask(ENamedThreads::AnyThread, [this, abc]() {
        MySQLLink m;
        m.SaveBlock(*abc);
        AsyncTask(ENamedThreads::GameThread, [this]()
            {
                FText DialogText = FText::FromString(TEXT("数据保存完毕"));
                EAppReturnType::Type ReturnType = FMessageDialog::Open(EAppMsgType::Ok, DialogText);
                for (int i = 0; i < ABC->AirGridCells.Num();i++) {
                    for (int j = 0; j < ABC->AirGridCells[i].Num(); j++) {
                        for (int k = 0; k < ABC->AirGridCells[i][j].Num(); k++) {
                            FActorSpawnParameters SpawnParams;
                            AWeatherWordActor* aw = GetWorld()->SpawnActor<AWeatherWordActor>(AWeatherWordActor::StaticClass(), FVector(0, 0, 100), FRotator::ZeroRotator, SpawnParams);
                            aw->IniWWA(UTF8_TO_TCHAR(ABC->AirGridCells[i][j][k].weather.c_str()), ABC->AirGridCells[i][j][k].World_center_Location);
                            allWeatherWord.Add(ABC->AirGridCells[i][j][k].World_center_Location,aw);
                        }
                        
                    }
                }
            });

        });
}

//路径显示
void UMainMenu::LoadPath()
{
    APathViewActor* aaa = GetWorld()->SpawnActor<APathViewActor>(
        //APathViewActor::StaticClass(),
        FVector::ZeroVector,  // 位置
        FRotator::ZeroRotator // 旋转
    );

    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    if (DesktopPlatform)
    {
        // 初始化文件对话框参数
        FString DefaultPath = FPaths::ProjectDir(); // 默认路径为项目目录
        FString FileTypes = TEXT("All Files (*.*)|*.*"); // 文件类型过滤器

        TArray<FString> OutFiles;

        // 打开文件对话框
        bool bOpened = DesktopPlatform->OpenFileDialog(
            nullptr,
            TEXT("选择一个路径文件"),
            DefaultPath,
            TEXT(""),
            FileTypes,
            EFileDialogFlags::None,
            OutFiles
        );

        if (bOpened && OutFiles.Num() > 0)
        {
            // 保存选择的文件路径
            aaa->FilePath = OutFiles[0];
            aaa->SetPoints();
            allPath.Add(aaa);
        }
    }
    
    
}

void UMainMenu::OnGridShowChange(bool show)
{
    
    ShowGrid = show;
}

void UMainMenu::OnAirLineShowChange(bool show)
{
    for (APathViewActor * var : allPath)
    {
        if (var) {
            UE_LOG(LogTemp, Warning, TEXT("选框状态"));
            var->Hide(show);
        }
    }
}

//快速移动（未完成）

void UMainMenu::Quickmove_latlon()
{
    float aim_longitude = FCString::Atof(*(longitude_input_set->GetText().ToString()));
    float aim_latitude = FCString::Atof(*(latitude_input_set->GetText().ToString()));
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    UE_LOG(LogTemp, Warning, TEXT("正在移动"));
    if (PC && PC->GetCharacter())
    {
        UE_LOG(LogTemp, Warning, TEXT("成功移动"));
        FVector NewLocation(aim_longitude, aim_latitude, 0);
        PC->GetCharacter()->SetActorLocation(LatLon_to_UE(NewLocation));
    }
    
}

void UMainMenu::Quickmove_xy()
{
}

//处理天气字符串
std::string RemoveCharacters(const std::string& input)
{
    std::string result = input;

    // Remove occurrences of "天气"
    std::string::size_type pos = 0;
    while ((pos = result.find("天气", pos)) != std::string::npos)
    {
        result.erase(pos, std::strlen("天气"));
    }

    // Remove colons and double quotes
    result.erase(std::remove_if(result.begin(), result.end(), [](char c) {
        return c == ':' || c == '\"';
        }), result.end());

    return result;
}

//修改天气
void UMainMenu::ChangeWorldWeather() {
    //根据天气来改变状态
    std::string aimweather;
    std::string weather_string(TCHAR_TO_UTF8(*(Weather_Fstring)));
    weather_string=RemoveCharacters(weather_string);
    auto transform_weather = weatherMap.find(weather_string);
    if (transform_weather != weatherMap.end())
    {
        //UE_LOG(LogTemp, Warning, TEXT("找到了对应天气"));
        aimweather=transform_weather->second;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("没有找到对应天气"));
    }
    if (aimweather == "晴天"|| aimweather == "阴天") {
        weathercontroller->AlwaysRain = false;
        weathercontroller->AlwaysSnow = false;
        weathercontroller->AlwaysSunny = true;
        //UE_LOG(LogTemp, Warning, TEXT("晴天晴天"));
    }
    else if (aimweather == "雨天") {
        weathercontroller->AlwaysRain = true;
        weathercontroller->AlwaysSnow = false;
        weathercontroller->AlwaysSunny = false;
        //UE_LOG(LogTemp, Warning, TEXT("下雨下雨"));
    }
    else if (aimweather == "雨雪天") {
        weathercontroller->AlwaysRain = true;
        weathercontroller->AlwaysSnow = true;
        weathercontroller->AlwaysSunny = false;
    }
    else if (aimweather == "雪天") {
        weathercontroller->AlwaysRain = false;
        weathercontroller->AlwaysSnow = true;
        weathercontroller->AlwaysSunny = false;
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("什么也没找到"));
    }
    FString aaa = UTF8_TO_TCHAR(aimweather.c_str());
    //UE_LOG(LogTemp, Warning, TEXT("%s"),*aaa);

    //根据时间来改变天色
    Change_Time_();
}

void UMainMenu::OnTextChanged() {

}

void UMainMenu::WeatherAPIButtonClicked() {
    //测试天气api
    //UE_LOG(LogTemp, Warning, TEXT("点击了天气按钮"));
    is_getting_message_from_api = true;
    FText citycode = citycode_input->GetText();
    std::string citycode_string = "110101";
    if (!citycode.IsEmpty()) {
        citycode_string = TCHAR_TO_UTF8(*(citycode.ToString()));
    }
    GetWeatherAPI* we1=new GetWeatherAPI(citycode_string);
    we1->GetWeather();


    if (Weather_Text)
    {
        Weather_Fstring = FString( we1->Weather_message[2].c_str());
        // 创建一个FStringBuilder来组合字符串
        std::string weather_text="";

        // 遍历TArray并将每个FString添加到FStringBuilder
        for (const std::string& Str : we1->Weather_message)
        {
            weather_text += Str + "\n";
            //StringBuilder << "\n";
            Inf_Weather.Add(UTF8_TO_TCHAR(Str.c_str()));
        }
        Weather_Fstring = Inf_Weather[2];
        Inf_Weather.Empty();
        weather_text += "时间：";
        auto now = std::chrono::system_clock::now();

        // 将时间点转换为time_t格式
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        // 将time_t格式转换为tm结构体
        std::tm now_tm = *std::localtime(&now_time);

        // 使用stringstream格式化时间
        std::ostringstream oss;
        oss << std::put_time(&now_tm, "%H:%M");
        int hour = now_tm.tm_hour;
        Time_Fstring = FString::Printf(TEXT("%d"), hour);


        weather_text += oss.str()+"\n";
        std::string time_string= oss.str();
        size_t pos = time_string.find(':');  // 找到冒号的位置
        if (pos != std::string::npos) {
            time_string.erase(pos);  // 删除冒号及其后面的内容
        }
        Time_Fstring= UTF8_TO_TCHAR(time_string.c_str());
        // 将组合后的字符串从StringBuilder转换为FString
        weather_text.erase(std::remove(weather_text.begin(), weather_text.end(), '"'), weather_text.end());
        FString unrealString = UTF8_TO_TCHAR(weather_text.c_str());
        //FString CombinedString = StringBuilder.ToString();
        Weather_Text->SetText(FText::FromString(unrealString));
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("输入的城市代码不存在"));
    }

    ChangeWorldWeather();
}

void UMainMenu::PointCloudButtonClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("点击了导入点云图按钮"));
    // 导入点云图
    
    //OpenFileDialog();
    UE_LOG(LogTemp, Warning, TEXT("打开的文件路径是%s"),*SelectedFilePath);
    //plytomesh();
}

//(已弃用)打开文件浏览器让用户选择点云图导入,得到的路径保存在SelectedFilePath中
void UMainMenu::OpenFileDialog()
{
    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    if (DesktopPlatform)
    {
        // 初始化文件对话框参数
        FString DefaultPath = FPaths::ProjectDir(); // 默认路径为项目目录
        FString FileTypes = TEXT("All Files (*.*)|*.*"); // 文件类型过滤器

        TArray<FString> OutFiles;

        // 打开文件对话框
        bool bOpened = DesktopPlatform->OpenFileDialog(
            nullptr,
            TEXT("选择一个点云文件"),
            DefaultPath,
            TEXT(""),
            FileTypes,
            EFileDialogFlags::None,
            OutFiles
        );

        if (bOpened && OutFiles.Num() > 0)
        {
            // 保存选择的文件路径
            SelectedFilePath = OutFiles[0];
        }
    }
}



void UMainMenu::plytomesh(FVector Location) {
    std::ifstream ifstream;
    std::string istream_filename = "";
    std::string ostream_filename = "";
    istream_filename = TCHAR_TO_UTF8(*SelectedFilePath);
    ifstream.open(istream_filename);
    if (!ifstream.is_open()) {
        UE_LOG(LogTemp, Warning, TEXT("当前指定文件目录不存在"));
        return;
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("成功打开文件"));
    }
    filename = istream_filename;
    
    
    //UE_LOG(LogTemp, Warning, TEXT("在当前目录下成功创建文件"));
    
    //启用多线程读取文件，然后回到主线程调用loadplymesh渲染
    AsyncTask(ENamedThreads::AnyThread, [this, Location]() {
        std::shared_ptr<PlyPaser> p = std::make_shared<PlyPaser>(filename);
        p->Start();
        //UE_LOG(LogTemp, Log, TEXT("这里是子线程"));
        AsyncTask(ENamedThreads::GameThread, [this, p, Location]()
            {
                //UE_LOG(LogTemp, Log, TEXT("这里是主线程1"));
                LoadPlyMesh(*p, Location);
            });
        });
    //UE_LOG(LogTemp, Log, TEXT("这里是主线程2"));
    
    
    //UE_LOG(LogTemp, Warning, TEXT("文件数据为%d,%d"),ply2mesh.Vertices.Num(), ply2mesh.Triangles.Num());
}

void UMainMenu::LoadPlyMesh(PlyPaser &ply2mesh, FVector Location) {
    FRotator Rotation(0.0f, 0.0f, 0.0f);
    if (ply2mesh.ValidateMeshData())
    {
        //ply2mesh.Write_File();
        // 创建网格体
        ADynamicMeshActor* MeshActor = GetWorld()->SpawnActor<ADynamicMeshActor>(ADynamicMeshActor::StaticClass(), Location, Rotation);
        FString actor_name = UTF8_TO_TCHAR(ply2mesh.sourcefile.c_str());
        UE_LOG(LogTemp, Log, TEXT("点云文件名：%s"), *actor_name);
        int32 LastSlashIndex;
        if (actor_name.FindLastChar('/', LastSlashIndex))
        {
            actor_name = actor_name.RightChop(LastSlashIndex + 1);
            // LastPart 结果为 "C"
        }
        MeshActor->Rename(*actor_name);
        if (MeshActor)
        {
            MeshActor->CreateMesh(ply2mesh.Vertices, ply2mesh.Triangles);
        }
        allPlyMesh.Add(actor_name,MeshActor);
        UE_LOG(LogTemp, Log, TEXT("创建成功"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("创建失败，数据可能存在问题"));
    }
}



