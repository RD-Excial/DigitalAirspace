// Fill out your copyright notice in the Description page of Project Settings.


#include "GetWeatherAPI.h"
#define WIN32_LEAN_AND_MEAN
#include "httplib.h"


GetWeatherAPI::GetWeatherAPI(std::string city)
{
	cityid = city;
}

GetWeatherAPI::~GetWeatherAPI()
{
}

void GetWeatherAPI::GetWeather() {
	std::string weatherAPI = Weather_API_1 + Weather_API_2 + "city=" + cityid + "&key=" + Key_;
    // 创建一个httplib客户端
    httplib::Client client(Weather_API_1,80);
    
    // 发送GET请求
    auto res = client.Get(Weather_API_2 + "city=" + cityid + "&key="+Key_);
    FString u1 = UTF8_TO_TCHAR(weatherAPI.c_str());
    //UE_LOG(LogTemp, Warning, TEXT("获取天气中"));
    //UE_LOG(LogTemp, Warning, TEXT("res状态为%d"), res->status);
    // 检查响应是否有效
    if (res&&res->status==200) {
        // 打印响应体
        FString unrealString = UTF8_TO_TCHAR(res->body.c_str());
        //UE_LOG(LogTemp, Warning, TEXT("api返回信息为%s"), *unrealString);
        //解析res->body
        APIPaser(res->body);
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("获取天气信息失败"));
    }
    
    return;
}

void GetWeatherAPI::APIPaser(std::string api) {
    size_t pos = api.find('[');
    std::string item;

    if (pos != std::string::npos) {
        api = api.substr(pos + 2); // 截取[{后面的内容
    }
    
    std::stringstream ss(api);
    for (int i = 0; i < 7; i++) {
        
        std::string old_substr;
        std::string new_substr;
        std::getline(ss, item, ',');
        if (i == 2) continue;
        if (i == 0) {
            old_substr = "province";
            new_substr = "省份";
        }
        if (i == 1) {
            old_substr = "city";
            new_substr = "城市";
        }
        if (i == 3) {
            old_substr = "weather";
            new_substr = "天气";
        }
        if (i == 4) {
            old_substr = "temperature";
            new_substr = "温度";
        }
        if (i == 5) {
            old_substr = "winddirection";
            new_substr = "风向";
        }
        if (i == 6) {
            old_substr = "windpower";
            new_substr = "风力";
        }
        // 找到子字符串的位置
        size_t pos2 = item.find(old_substr);

        // 如果找到子字符串，进行替换
        if (pos2 != std::string::npos) {
            item.replace(pos2, old_substr.length(), new_substr);
        }
        //FString unrealString = UTF8_TO_TCHAR(item.c_str());
        Weather_message.push_back(item);
        //UE_LOG(LogTemp, Warning, TEXT("%s"), *unrealString);
    }


    
}