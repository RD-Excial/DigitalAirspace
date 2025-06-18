// Fill out your copyright notice in the Description page of Project Settings.


#include "JsonRead.h"
#include "Serialization/JsonReader.h"
#include "MySQLLink.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"
#include <JsonUtilities/Public/JsonObjectConverter.h>

JsonRead::JsonRead(FString JsonString, MySQLLink& m1)
{
    m = &m1;
    UE_LOG(LogTemp, Log, TEXT("正在调用json函数"));
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);
    FString Out_action;
    TMap<FString, FString> weatherdata_map;
    if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("正在提取json"));
        // 提取 action 字段
        if (JsonObject->HasField(TEXT("action")))
        {
            UE_LOG(LogTemp, Log, TEXT("action"));
            //如果是add
            if (JsonObject->GetStringField(TEXT("action")) == TEXT("add")) {
                UE_LOG(LogTemp, Log, TEXT("add"));
                // 提取子对象
                const TSharedPtr<FJsonObject>* DataObject;
                if (JsonObject->TryGetObjectField(TEXT("weatherdata"), DataObject) && DataObject->IsValid())
                {
                    UE_LOG(LogTemp, Log, TEXT("weatherdata"));
                    Weather_read(DataObject);
                }
                if(JsonObject->TryGetObjectField(TEXT("Point_file"), DataObject) && DataObject->IsValid()){
                    UE_LOG(LogTemp, Log, TEXT("pointfile"));
                    Pointfile_read(DataObject);
                }
                if (JsonObject->TryGetObjectField(TEXT("Airline"), DataObject) && DataObject->IsValid()) {
                    UE_LOG(LogTemp, Log, TEXT("airline"));
                    Airline_read(DataObject);
                }

            }

            //如果收到的指令是get
            if (JsonObject->GetStringField(TEXT("action")) == TEXT("get")) {
                // 提取 data 子对象
                const TSharedPtr<FJsonObject>* DataObject;
                if (JsonObject->TryGetObjectField(TEXT("locate"), DataObject) && DataObject->IsValid())
                {
                    if (ABCdata == nullptr||ABCdata->AirGridCells.Num()==0) {
                        UE_LOG(LogTemp, Warning, TEXT("未检索到空域数据"));
                        return;
                    }
                    UE_LOG(LogTemp, Log, TEXT("成功返回空域数据"));

                }
            }
        }
        
        

        
    }
}

JsonRead::~JsonRead()
{
}

void JsonRead::Weather_read(const TSharedPtr<FJsonObject>* DataObject)
{
    //纬度
    float lat;
    float lat1;
    //经度
    float lon;
    float lon1;
    FString wind_direction;
    // 提取天气的 String 值
    if ((*DataObject)->HasField(TEXT("weather")))
    {
        GetData.weatherdata.weather = (*DataObject)->GetStringField(TEXT("weather"));
    }
    //风向
    if ((*DataObject)->HasField(TEXT("wind_direction")))
    {
        GetData.weatherdata.wind_direction = (*DataObject)->GetStringField(TEXT("wind_direction"));
    }

    // 提取风力的数值
    if ((*DataObject)->HasField(TEXT("wind_power")))
    {
        GetData.weatherdata.wind_power = (float)((*DataObject)->GetNumberField(TEXT("wind_power")));
    }

    const TSharedPtr<FJsonObject>* VectorObject;
    if ((*DataObject)->TryGetObjectField(TEXT("locate1"), VectorObject) && VectorObject->IsValid())
    {
        // 提取经纬度的值
        lon = (*VectorObject)->GetNumberField(TEXT("lon"));
        lat = (*VectorObject)->GetNumberField(TEXT("lat"));
    }
    // 提取 loacte2的值
    if ((*DataObject)->TryGetObjectField(TEXT("locate2"), VectorObject) && VectorObject->IsValid())
    {
        // 提取经纬度的值
        lon1 = (*VectorObject)->GetNumberField(TEXT("lon"));
        lat1 = (*VectorObject)->GetNumberField(TEXT("lat"));
    }

    UE_LOG(LogTemp, Log, TEXT("读取到的天气为%s，风力为%f，"), *(GetData.weatherdata.weather), GetData.weatherdata.wind_power);
    m->SaveWeather(lon, lat, lon1, lat1, GetData.weatherdata.weather,wind_direction, GetData.weatherdata.wind_power);
}

void JsonRead::Pointfile_read(const TSharedPtr<FJsonObject>* DataObject)
{
    FString filename;
    //纬度
    float lat;
    //经度
    float lon;
    //海拔
    float ele;

    // 提取点云名字的 String 值
    if ((*DataObject)->HasField(TEXT("filename")))
    {
        filename = (*DataObject)->GetStringField(TEXT("filename"));
    }

    // 提取 loacte的值
    const TSharedPtr<FJsonObject>* VectorObject;
    if ((*DataObject)->TryGetObjectField(TEXT("locate"), VectorObject) && VectorObject->IsValid())
    {
        // 提取经纬度和海拔的值
        lon = (*VectorObject)->GetNumberField(TEXT("lon"));
        lat = (*VectorObject)->GetNumberField(TEXT("lat"));
        ele = (*VectorObject)->GetNumberField(TEXT("ele"));
    }

    UE_LOG(LogTemp, Log, TEXT("读取到的文件名为%s,经度为%f"), *filename,lon);
    //添加到数据库中
    m->SavePoint(filename, lon, lat, ele);
}

void JsonRead::Airline_read(const TSharedPtr<FJsonObject>* DataObject)
{
    FString filename;
    //纬度
    float lat;
    //经度
    float lon;
    //海拔
    float ele;
    TArray<FVector> AllPoints;

    // 提取点云名字的 String 值
    if ((*DataObject)->HasField(TEXT("linename")))
    {
        filename = (*DataObject)->GetStringField(TEXT("linename"));
    }

    // 提取 loacte的值
    const TArray<TSharedPtr<FJsonValue>>* LocateArray;
    if ((*DataObject)->TryGetArrayField("locates", LocateArray))
    {
        FString locatesString;
        TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&locatesString);
        FJsonSerializer::Serialize(MakeShareable(new FJsonValueArray(*LocateArray)), TEXT(""), Writer);
        Writer->Close();
        for (const auto& LocateVal : *LocateArray)
        {
            UE_LOG(LogTemp, Log, TEXT("aaa"));
            const TArray<TSharedPtr<FJsonValue>>& PointArray = LocateVal->AsArray();
            if (PointArray.Num() >= 3)
            {
                lon = FCString::Atod(*(PointArray[0]->AsString()));
                lat = FCString::Atod(*(PointArray[1]->AsString()));
                ele = FCString::Atod(*(PointArray[2]->AsString()));
                UE_LOG(LogTemp, Log, TEXT("读取到经度为%f"),lon);
                AllPoints.Add(FVector(lon, lat, ele));
            }
        }
        //添加到数据库中
        m->SaveAirLine(filename,locatesString,AllPoints);
        
    }

    //UE_LOG(LogTemp, Log, TEXT("读取到的文件名为%s,经度为%f"), *filename, lon);
    
}
