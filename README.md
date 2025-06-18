# 面向电力巡检的数字化空域系统文档

[TOC]

## 环境配置

### 开发环境

操作系统：Windows11

虚幻引擎版本：虚幻4.27

visual studio版本：2022

### 项目初始化步骤

从github上clone到指定位置，确保在clone前安装了GitHub LFS，以下载项目内大型文件

如果项目包含`Binaries`、`Intermediate`和`.vs`目录，在首次编译前删除它们

右键点击`.uproject`文件，选择“Generate Visual Studio project files”生成`.sln`文件

用Visual Studio打开生成的`.sln`文件并编译

使用虚幻引擎打开`.uproject`文件并运行

## 代码结构说明

```
/Source
    ├── DigitalAirspace
    │   ├── AirBlockClass # 空域网格模块
    │   ├── DigitalAirspace # 已弃用
    │   ├── DigitalAirspaceGameModeBase # 动态加载相关，已弃用
    │   ├── DynamicMeshActor # 把ply数据转换为mesh模型的数据
    │   ├── FileWatch.hpp # 文件夹监视用插件，只有这个hpp文件
    │   ├── GetWeatherAPI # 从API获取天气
    │   ├── httplib # http连接用插件
    │   ├── JsonRead # 读取json方法
    │   ├── MainMenu # ui界面，包括大部分软件启动时需要运行的模块的初始化
    │   ├── MyProjects # 已弃用
    │   ├── MySQLLink # 数据库模块
    │   ├── PathViewActor # 路径显示模块
    │   ├── PlyPaser # ply文件解析
    │   ├── PlyTArrayStruct # ply数据结构体类型
    │   ├── PointCloud2OBJ # 已弃用
    │   ├── SocketLink # socket接口模块
    │   ├── UserConfig # 用户配置
    │   ├── WeatherWordActor # 更改天气文字
    ├── ThirdParty      # 第三方库
    └── DigitalAirspace.Target.cs  # 编译配置
```

大部分函数的具体行为请参照注释

## 重要模块代码说明

### AirBlockClass

在MainMenu.cpp的LoadAirGridSystem函数中创建-调用构造函数初始化AirBlockClass类-调用UpdateGridFromWorld函数逐个网格扫描障碍物-使用CalculateChildAirBlock函数进行八叉树划分，对每个子网格进行障碍物扫描；

DrawDebug和DrawDebugGrid函数用于显示网格，每帧都会进行显示判断，由ui选框控制

### JsonRead

主要是给SocketLink类调用的，根据不同的body执行不同的解析，具体如下

气象数据主要字段包括：天气、风力、风向、两个不同的经纬度。经纬度用于计算该气象数据的范围。具体参考如下所示。

```
{"action": "add",
"weatherdata": {
"weather": "晴天",
"wind_power": 12.3,
"wind_direction": "东北",
"locate1": {
    "lon": 1.0,
    "lat": 2.0
} ,
"locate2": {
"lon": 3.0,
    "lat": 4.0
}
}
}
```

对于点云文件数据的JSON设计，包括经纬度、海拔和点云文件名。具体参考如下：

```
{"action": "add",
"Point_file": {
"filename": "abc.ply",
"locate": {
    "lon": 1.0,
    "lat": 2.0,
	"ele": 2.0
}
}
}
```

对于航线数据的JSON数据设计，由航线名称和数个坐标组成。具体参考如下：

```
{"action": "add",
"Airline": {
"linename": "abc ",
"locates": [
    [116.404, 39.915,100],
    [116.405, 39.916,100]
  ]
}
}
```

### MainMenu

大部分的主要逻辑都在这个类中调用和执行。NativeConstruct()是初始化函数，正常情况下系统的起点就是这里。下面介绍一下NativeConstruct()函数内的组成

首先是函数绑定。形如

```
if (ImportPointCloudMap)
```

是绑定按钮和函数，if内的ImportPointCloudMap是按钮的名字，可以在内容浏览器-ui-StartMenu蓝图里面找到。

```
ImportPointCloudMap->OnClicked.AddDynamic(this, &UMainMenu::PointCloudButtonClicked);
```

这段代码表示把ImportPointCloudMap按钮绑定到PointCloudButtonClicked函数上。

里面有些按钮和函数已经弃用，可以在ui蓝图看见有些按钮已经被挪开了。注意并不全是按钮，可以在头文件查看这些名字分别对应什么类型的类。

然后是一些内容的初始化，比如天气、时间的显示，数据库实例，点云文件读取路径。

```
BindToMySQLLink(m);
```

这个是委托绑定，用来在接收到路径显示、改变点云模型位置和改变网格信息的信息之后自动调用，有点类似回调。这个会在mysqllink类中详细介绍。

之后是建立http连接，这个在socketlink类中介绍。



接下来是NativeTick函数，这个就是每tick都调用的函数。里面用布尔值控制空域网格的显示，ui界面的选框控制这个布尔值的改变。



AutoLoadPoint()负责监控指定文件夹，用FileWatch.hpp提供的方法实现的。

```
 FileWatcher=MakeUnique<filewatch::FileWatch<std::string>>(std::string(TCHAR_TO_UTF8(*WatchDir)),
     [this](const std::string& path, const filewatch::Event change_type) {
         PointFileChange(path, change_type);
     }
 );
```

上面的回调函数用来在检测到文件夹文件发生变化之后调用PointFileChange函数，参数path是路径，change_type是文件发生变化的类型。之后调用GetPointMessage(Fpath)根据点云名字获取点云位置信息，用plytomesh在子线程读取点云文件，读取完毕后回到主线程加载模型。



Moveuav用来在航迹显示的时候显示无人机飞行。

### MySQLLink

头文件里的委托函数如下

```
类外
DECLARE_MULTICAST_DELEGATE_TwoParams(FOn_PLYLocate_Updated, FVector,FString);
DECLARE_MULTICAST_DELEGATE_OneParam(FOn_Airline_Updated, TArray<FVector>);
DECLARE_MULTICAST_DELEGATE_OneParam(FOn_Airgrid_Updated, WeatherDataClass);
类里
FOn_PLYLocate_Updated ply_update;
FOn_Airline_Updated airline_update;
FOn_Airgrid_Updated airgrid_update;
```

以更新点云位置为例

```
void MySQLLink::UpdatePlyLocate(FVector f,FString fs)
{
	if (ply_update.IsBound()) {
		UE_LOG(LogTemp, Log, TEXT("UpdatePlyLocate委托步骤：broadcast"));
		ply_update.Broadcast(f, fs);
	}
	
}
```

这个函数在负责保存和更新点云文件的SavePoint函数中调用，如果ply_update绑定了委托，就会通知所有委托执行绑定的函数，也就是MainMenu类中的下面这个函数里面的ChangeLocate函数。f, fs分别是这个函数需要的两个参数。

```
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
```

### PlyPaser

这个类主要负责处理点云文件，一行一行读取，根据文件提供的信息来读取文件，最终把点数据和面数据分别保存到内存里，颜色的保存还没做。

### SocketLink

主要是负责和外部系统建立连接，用的是httplib插件实现的，里面都有详细的注释。主要就是接收到http连接之后把body传给jsonread

### WeatherWordActor

用来显示天气文字在网格里面。有详细的注释。目前的问题是没法用中文，因为我创建不出来中文字体资产，虚幻引擎自己不支持这个actor使用中文字体，另外就是比较急没时间弄。

## 待完成内容

1、外部信息传入并更新系统内网格的信息这块，还没想好是更新大网格还是小网格，所以也没做数据库信息和系统信息的同步。

2、天气文字不支持中文

3、塔的标记还没有完全实现，只做到分辨出标记，没有后续保存。

## 常见问题

部分头文件找不到位置：因为有部分头文件是绝对路径，把它的前面部分改成你自己的虚幻引擎所在的位置。
