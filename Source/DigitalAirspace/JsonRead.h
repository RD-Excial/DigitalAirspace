// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MySQLLink.h"
#include "AirBlockClass.h"

/**
 * 
 */
class DIGITALAIRSPACE_API JsonRead
{
public:
	USTRUCT()
	struct Weatherdata {
		//GENERATED_BODY()
		FString weather;
		float wind_power;
		FString wind_direction;
		//假设天气范围是矩形的对角顶点，则可以确定一个矩形，矩形范围内的网格都设置为该天气
		FVector locate1;
		FVector locate2;
	};

	//json数据格式
	USTRUCT()
	struct Json_Read_struct
	{
		//GENERATED_BODY()
		FString action;
		//FString Locate;
		Weatherdata weatherdata;
	};
	
	JsonRead(FString JsonString,MySQLLink& m1);
	~JsonRead();

	//读取天气数据
	void Weather_read(const TSharedPtr<FJsonObject>* DataObject);
	//读取点云文件位置数据
	void Pointfile_read(const TSharedPtr<FJsonObject>* DataObject);
	//读取航线数据
	void Airline_read(const TSharedPtr<FJsonObject>* DataObject);
	Json_Read_struct GetData;
	AirBlockClass* ABCdata;

	//数据库指针
	MySQLLink* m;
};
