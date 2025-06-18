// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <vector>
#include <string>



//api的前半段，后半段是city=(城市id)&key=(个人key)
const std::string Weather_API_1 = "restapi.amap.com";
const std::string Weather_API_2 = "/v3/weather/weatherInfo?";
const std::string Key_ = "632b7a00012291899f37ceecb1ffef15";
/**
 * 
 */
class DIGITALAIRSPACE_API GetWeatherAPI
{
public:
	GetWeatherAPI(std::string city);
	~GetWeatherAPI();

	void GetWeather();
	void APIPaser(std::string api);

	std::string cityid;
	std::vector<std::string> Weather_message;
};
