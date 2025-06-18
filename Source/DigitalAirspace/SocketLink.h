// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "MySQLLink.h"
#include "SocketSubsystem.h"
#include <thread>

/**
 * 
 */
class DIGITALAIRSPACE_API SocketLink
{
public:
	SocketLink(MySQLLink& m);
	~SocketLink();

	//端口号
	int Port = 174;

    //DECLARE_DELEGATE_OneParam(FOnDataReceived, const FString&);
    //FOnDataReceived OnDataReceived;

    //已弃用
    void Start();

    //http
    void StartHttpLink();
    void RunHttpLink(int p);
    void StopHttpLink();

    //数据库指针
    MySQLLink* m=nullptr;

    //void Stop();
    //void ReceiveMessageFromClient();
    FSocket* ListenSocket = nullptr;
    FSocket* ClientSocket = nullptr;
    FTimerHandle TimerHandle;
    bool bRunning;
    std::atomic<bool> bIsRunning{ false };
    std::thread ServerThread;
    //一些数据的指针

};
