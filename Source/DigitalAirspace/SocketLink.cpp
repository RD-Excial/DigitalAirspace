// Fill out your copyright notice in the Description page of Project Settings.


#include "SocketLink.h"
#define WIN32_LEAN_AND_MEAN
#include "httplib.h"
#include "JsonRead.h"
#include <Misc/MessageDialog.h>
SocketLink::SocketLink(MySQLLink& m1)
{
    m = &m1;
}

SocketLink::~SocketLink()
{
    StopHttpLink();
    if (ListenSocket)
    {
        ISocketSubsystem::Get()->DestroySocket(ListenSocket);
        ListenSocket = nullptr;
    }
}

void SocketLink::Start()
{
    /*// 创建TCP Socket
    ListenSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("LocalSocketServer"), false);

    // 绑定到本地地址
    TSharedRef<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    bool bIsValid;
    InternetAddr->SetIp(TEXT("0.0.0.0"), bIsValid);
    InternetAddr->SetPort(Port);
    ListenSocket->Bind(*InternetAddr);
    ListenSocket->SetNonBlocking(false);
    ListenSocket->Listen(10); // 参数为最大连接队列数
    UE_LOG(LogTemp, Log, TEXT("启动服务器成功"));
    // 开始异步任务监听客户端连接
    AsyncTask(ENamedThreads::AnyThread, [this]()
        {
            while (true) // 循环监听
            {
                if (ListenSocket)
                {
                    //UE_LOG(LogTemp, Log, TEXT("ListenSocket连接成功"));
                    // 阻塞等待客户端连接
                    FSocket* ClientSocket = ListenSocket->Accept(TEXT("IncomingConnection"));
                    if (ClientSocket)
                    {
                        UE_LOG(LogTemp, Log, TEXT("客户端连接成功"));
                        while (ClientSocket && ClientSocket->GetConnectionState() == ESocketConnectionState::SCS_Connected)
                        {
                            uint32 Size;
                            if (ClientSocket->HasPendingData(Size)) // 检查是否有数据可读取
                            {
                                UE_LOG(LogTemp, Log, TEXT("有数据可读取"));
                                TArray<uint8> ReceivedData;
                                ReceivedData.SetNumUninitialized(Size);

                                int32 BytesRead = 0;
                                ClientSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), BytesRead);

                                if (BytesRead > 0)
                                {
                                    // 将字节数组转换为 FString
                                    FString ReceivedMessage = FString(UTF8_TO_TCHAR(ReceivedData.GetData()));
                                    UE_LOG(LogTemp, Log, TEXT("收到来自客户端的信息: %s"), *ReceivedMessage);

                                    // 通知主线程处理数据
                                    AsyncTask(ENamedThreads::GameThread, [this,ReceivedMessage]()
                                        {
                                            UE_LOG(LogTemp, Log, TEXT("回到主线程处理socket数据"));
                                            // 调用jsonread
                                            JsonRead newJsonRead(ReceivedMessage);
                                        });
                                }
                                else {
                                    //关闭连接
                                    ClientSocket->Close(); // 先尝试关闭
                                    ISocketSubsystem::Get()->DestroySocket(ClientSocket);
                                    ClientSocket = nullptr;
                                    UE_LOG(LogTemp, Log, TEXT("客户端已断开连接"));
                                    break;
                                }
                            }
                        }
                            // 避免线程占用过多CPU资源，适当休眠
                        FPlatformProcess::Sleep(0.01f);
                    }
                    else {
                        UE_LOG(LogTemp, Log, TEXT("客户端连接失败"));
                    }
                }

                // 避免线程占用过多CPU资源，适当休眠
                FPlatformProcess::Sleep(0.01f);
            }
        });*/
    
    
}

void SocketLink::StartHttpLink()
{
    if (bIsRunning) return;

    bIsRunning = true;
    ServerThread = std::thread(&SocketLink::RunHttpLink, this, Port);
}

void SocketLink::RunHttpLink(int p)
{
    httplib::Server server;

    // 绑定POST请求处理
    server.Post("/action", [this](const httplib::Request& req, httplib::Response& res) {
        // 将请求体传递到游戏线程处理
        FString Body(req.body.c_str());
        MySQLLink* m1 = this->m;
        AsyncTask(ENamedThreads::GameThread, [Body,m1]() {
            // 在游戏线程中处理数据（示例：打印日志）
            //UE_LOG(LogTemp, Warning, TEXT("收到的json字符串为: %s"), *Body);
            FText DialogText = FText::FromString(*Body);
            EAppReturnType::Type ReturnType = FMessageDialog::Open(EAppMsgType::Ok, DialogText);
            UE_LOG(LogTemp, Warning, TEXT("创建窗口完毕"));
            JsonRead newJsonRead(Body, *m1);
            });

        // 返回JSON响应
        res.set_content(R"({"status": "success"})", "application/json");
        });

    // 启动服务器（阻塞调用，直到 bIsRunning 设为 false）
    server.listen("0.0.0.0", Port);
}

void SocketLink::StopHttpLink()
{
    if (bIsRunning) {
        bIsRunning = false;
        if (ServerThread.joinable()) {
            ServerThread.join(); // 等待线程结束
        }
    }
}


