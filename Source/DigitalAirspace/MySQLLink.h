// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#define STATIC_CONCPP
#include "mysql_driver.h"
#include "cppconn/prepared_statement.h"
#include "cppconn/driver.h"
#include "cppconn/exception.h"
#include "cppconn/resultset.h" 
#include "AirBlockClass.h"
#include "CoreMinimal.h"

/**
 * 
 */
class WeatherDataClass {
public:
	FVector af;
	FVector bf;
	FString weather;
	FString wind_direction;
	float wind_power;
};
DECLARE_MULTICAST_DELEGATE_TwoParams(FOn_PLYLocate_Updated, FVector,FString);
DECLARE_MULTICAST_DELEGATE_OneParam(FOn_Airline_Updated, TArray<FVector>);
DECLARE_MULTICAST_DELEGATE_OneParam(FOn_Airgrid_Updated, WeatherDataClass);
class DIGITALAIRSPACE_API MySQLLink
{
public:
	MySQLLink();
	~MySQLLink();

	//��������
	

	//��⵽λ�ø��º�֪ͨ����ģ�͸ı�λ��
	FOn_PLYLocate_Updated ply_update;
	FOn_Airline_Updated airline_update;
	FOn_Airgrid_Updated airgrid_update;
	void UpdatePlyLocate(FVector f,FString fs);
	void UpdateAirlineLocate(TArray<FVector> AllPoints);
	void UpdateAirgrid(WeatherDataClass WDC);

	void SaveBlock(AirBlockClass& Block);
	void GetPLYLocation();
	TArray<FVector> GetBlockMessage(float size, float gridsize, FVector aimloacte);
	//������������
	void SaveWeather(float lon1,float lat1,float lon2,float lat2,FString weather,FString wind_direction,float wind_power);

	// �������λ������
	void SavePoint(FString filename,float lon,float lat, float ele);
	//���ҵ���λ������
	FVector GetPointMessage(FString filename);

	//���溽������
	void SaveAirLine(FString linename,FString points, TArray<FVector> AllPoints);

	

	FString SQLlink = "tcp://localhost:3306";
	FString SQLname = "root";
	FString SQLpassword = "qq897422796";

	sql::mysql::MySQL_Driver* driver;
	sql::Connection* con;
};
