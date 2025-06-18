// Copyright Epic Games, Inc. All Rights Reserved.


#include "DigitalAirspaceGameModeBase.h"
#include "EngineUtils.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include <unordered_set>
#include "HAL/FileManager.h"
#include "Misc/Paths.h"
#include <Misc/MessageDialog.h>
#include "Framework/Notifications/NotificationManager.h" 
#include "Internationalization/Text.h"  // FText 支持
#include "Internationalization/Internationalization.h"  // LOCTEXT 宏
#include <Widgets/Notifications/SNotificationList.h>
#include <IDesktopPlatform.h>
//#include "Misc/Async.h"


void ADigitalAirspaceGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);
    


}

void ADigitalAirspaceGameModeBase::BeginPlay() {
    Super::BeginPlay();


    //FileWatcher = MakeShareable(new DirectoryWatcher(WatchDir));
    //FileWatcher->OnFileChanged.BindUObject(this, &ADigitalAirspaceGameModeBase::OnFileChanged);

    
    // 统计所有物体
    //CountBlocks();

    // 获取第一个玩家控制器
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController) {
        // 通过玩家控制器获取相机管理器
        player_camera = PlayerController->PlayerCameraManager;
        if (player_camera) {
            UE_LOG(LogTemp, Log, TEXT("找到了摄像头！"));
        }
    }

    //FirstLoadBlocks();

    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle,
        this,
        &ADigitalAirspaceGameModeBase::TriggerAsyncTask,
        0.5f,
        true // 循环执行
    );

    //设定点云文件夹并自动进行点云模型导入
    //AutoUpdatePointCloud();
}
void ADigitalAirspaceGameModeBase::TriggerAsyncTask()
{
    // 启动子线程计算需要更新的区块
    //AsyncTask(ENamedThreads::AnyThread,[this]() {
    //    PreUpdateBlock();
    //});
    //PreUpdateBlock();
    //在主线程更新区块
    //UpdateBlock();
}







//系统加载完成后统计所有区块
void ADigitalAirspaceGameModeBase::CountBlocks() {
    //i是全地图左下角的点（上右为正
    int left = -1 * max_block_size * max_size / 2;
    UE_LOG(LogTemp, Warning, TEXT("left = %d"), left);
    for (int x = left; x < -left;) {
        for (int y = left; y < -left;) {
            Block_Class* newblock = new Block_Class(x, y, max_block_size);
            //把初始化的区块类保存起来
            std::string key_xy_string = std::to_string(x) + "_" + std::to_string(y);
            All_Blocks_map[key_xy_string] = newblock;
            //UE_LOG(LogTemp, Warning, TEXT("初始化区块坐标为: %s"), *FString(key_xy_string.c_str()));
            y += max_block_size;
        }
        x += max_block_size;
    }
    //读取actor并按照所在区块的中心位置保存到区块类中
    UWorld* World = GetWorld();
    if (!World) return;

    // 遍历所有 Actor
    for (TActorIterator<AActor> It(World); It; ++It)
    {
        AActor* Actor = *It;
        if (Actor->ActorHasTag("DontHidden")) continue;
        if (Actor->GetName() == "BP_GoodSky") continue;
        if (Actor->GetName() == "WeatherSystemMaster1_Blueprint") continue;
        
        //if (Actor->GetName() == "BP_GoodSky") continue;
        //计算actor所在区块
        //int_x,y分别是区块的左下角的xy坐标
        int int_x = Actor->GetActorLocation().X;
        if (int_x < 0) {
            int_x -= max_block_size;
        }
        int_x /= max_block_size;
        int_x *= max_block_size;

        int int_y = Actor->GetActorLocation().Y;
        if (int_y < 0) {
            int_y -= max_block_size;
        }
        int_y /= max_block_size;
        int_y *= max_block_size;
        
        //保存到区块中
        std::string key_xy_string = std::to_string(int_x) + "_" + std::to_string(int_y);
        All_Blocks_map[key_xy_string]->Block_Actors.Add(Actor);
        //Actor->SetActorHiddenInGame(true);
    }

    
}

void ADigitalAirspaceGameModeBase::FirstLoadBlocks() {
    Loading_Blocks_map.clear();
    PreUpdateBlock();

    //把所有isHidden为true的区块的actor隐藏起来
    for (auto& block : All_Blocks_map) {
        //UE_LOG(LogTemp, Warning, TEXT("加载区块坐标为%f,%f"), block.second->x0, block.second->y0);
        if (block.second->isHidden == false) continue;
        for (AActor* actor : block.second->Block_Actors) {
            actor->SetActorHiddenInGame(true);
        }
    }
}

void ADigitalAirspaceGameModeBase::PreUpdateBlock() {
    NeedLoading_Blocks_map.clear();
    TArray<std::string> loadkey;
    //获取摄像头位置并计算区块坐标
    camera_x = player_camera->GetActorLocation().X;
    if (camera_x < 0) {
        camera_x -= max_block_size;
    }
    camera_x /= max_block_size;
    camera_x *= max_block_size;

    camera_y = player_camera->GetActorLocation().Y;
    if (camera_y < 0) {
        camera_y -= max_block_size;
    }
    camera_y /= max_block_size;
    camera_y *= max_block_size;

    //计算需要更新的区块
    int left_x = camera_x - max_block_size * max_load_size;
    int left_y = camera_y - max_block_size * max_load_size;
    for (int x = left_x; x < camera_x + max_block_size * max_load_size;) {
        for (int y = left_y; y < camera_y + max_block_size * max_load_size;) {
            //把需要加载的区块类保存起来
            std::string key_xy_string = std::to_string(x) + "_" + std::to_string(y);
            loadkey.Add(key_xy_string);
            NeedLoading_Blocks_map[key_xy_string] = All_Blocks_map[key_xy_string];
            NeedLoading_Blocks_map[key_xy_string]->isHidden = false;
            y += max_block_size;
        }
        x += max_block_size;
    }

    //UE_LOG(LogTemp, Warning, TEXT("摄像头所在区块的左下角坐标为%d,%d"), camera_x,camera_y);
    
    //找到需要设置为隐藏的区块
    //删除已有的之后剩下的就是需要设置为隐藏的
    for (std::string key : loadkey) {
        Loading_Blocks_map.erase(key); // 直接erase更简洁
    }

    Hidden_Blocks_map.clear();
    Hidden_Blocks_map = Loading_Blocks_map;
    Loading_Blocks_map.clear();
    Loading_Blocks_map = NeedLoading_Blocks_map;
    
    
}

void ADigitalAirspaceGameModeBase::UpdateBlock() {

    //UE_LOG(LogTemp, Warning, TEXT("正在更新区块"));
    //把所有isHidden为true的区块的actor隐藏起来
    for (auto& block : Hidden_Blocks_map) {
        for (AActor* actor : block.second->Block_Actors) {
            actor->SetActorHiddenInGame(true);
        }
    }
    for (auto& block : Loading_Blocks_map) {
        for (AActor* actor : block.second->Block_Actors) {
            actor->SetActorHiddenInGame(false);
        }
    }
    
}
