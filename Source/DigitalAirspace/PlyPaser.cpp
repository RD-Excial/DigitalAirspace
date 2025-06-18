// Fill out your copyright notice in the Description page of Project Settings.


#include "PlyPaser.h"
#include <unordered_set>

//大端转小端
template <typename T>
T optimized_swap(const T& value) {
    static_assert(std::is_arithmetic_v<T>,
        "Only supports arithmetic types");

    union {
        T value;
        char bytes[sizeof(T)];
    } source, dest;

    source.value = value;

    // 手动反转字节顺序
    for (size_t i = 0; i < sizeof(T); ++i) {
        dest.bytes[i] = source.bytes[sizeof(T) - 1 - i];
    }

    return dest.value;
}

namespace std {
    template <>
    struct hash<FVector> {
        std::size_t operator()(const FVector& Vector) const {
            return std::hash<float>()(Vector.X) ^ std::hash<float>()(Vector.Y) ^ std::hash<float>()(Vector.Z);
        }
    };

    template <>
    struct equal_to<FVector> {
        bool operator()(const FVector& lhs, const FVector& rhs) const {
            return lhs.X == rhs.X && lhs.Y == rhs.Y && lhs.Z == rhs.Z;
        }
    };
}

PlyPaser::PlyPaser(std::string filename)
{
    sourcefile = filename;
}

PlyPaser::~PlyPaser()
{
}



PlyTArrayStruct PlyPaser::Start() {
    UE_LOG(LogTemp, Warning, TEXT("启动转换"));

    std::ifstream infile(sourcefile);
    //std::ofstream obj_file(aimfile);
    std::string line;




    bool reading_vertices = false;
    bool reading_faces = false;
    bool is_end = false;
    bool is_binary = false;
    bool is_big = false;
    bool is_double = false;
    int vertex_count = 0;
    int face_count = 0;
    int vertex_property_num = 0;
    bool up_face = false;


    // 检查文件是否成功打开
    if (!infile.is_open()) {
        UE_LOG(LogTemp, Warning, TEXT("无法打开文件"));
        PlyTArrayStruct Result;
        return Result;
    }

    while (std::getline(infile, line)) {
        //先找坐标
        

        // 如果找到关键词，则从头开始读取
        if (line.find("binary") != std::string::npos) {
            infile.seekg(0, std::ios::beg); // 将文件指针移到文件开头
            is_binary = true;
            break; // 退出循环，重新开始读取
        }
        if (line.find("format") != std::string::npos) {
            infile.seekg(0, std::ios::beg);
            break;
        }
    }
    infile.close();
    //如果是二进制数据
    if (is_binary) {
        infile.open(sourcefile, std::ios::binary);
    }
    else {
        infile.open(sourcefile, std::ios::in);
    }
    //找到二进制数据的开始点
    while (std::getline(infile, line)) {
        FString HappyString(UTF8_TO_TCHAR(line.c_str()));
        UE_LOG(LogTemp, Warning, TEXT("正在读取字符串为%s"), *HappyString);
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        // 读取坐标值
        /*if (token == "Airspace") {
            float x, y, z;
            iss >> x >> y >> z;
            ply_Locate = FVector(x, y, z);
            UE_LOG(LogTemp, Warning, TEXT("找到了点云模型坐标:%f,%f,%f"), ply_Locate.X, ply_Locate.Y, ply_Locate.Z);
        }*/
        //解析大小端
        if (line.find("format") != std::string::npos) {
            if (line.find("big") != std::string::npos) {
                is_big = true;
            }
            else if (line.find("little") != std::string::npos) {
                is_big = false;
            }
        }
        // 1. 解析顶点数量
        if (token == "element" && !reading_vertices) {

            std::string type;
            iss >> type;
            if (type == "vertex") {
                reading_vertices = true;
                iss >> vertex_count;
                //UE_LOG(LogTemp, Warning, TEXT("顶点数%d"), vertex_count);
            }
        }
        // 检查顶点属性是否是double类型
        if (line.find("property double x") != std::string::npos) {
            is_double = true;
        }
        
        //if (line.find("property float32") != std::string::npos&&!reading_faces) {

        //    vertex_property_num++;
        //}


        // 2. 解析面数量
        if (token == "element" && !reading_faces) {
            std::string type;
            iss >> type;
            if (type == "face") {
                reading_faces = true;
                iss >> face_count;
                //UE_LOG(LogTemp, Warning, TEXT("面数%d"), face_count);
            }
        }

        if (line.find("header") != std::string::npos) {
            UE_LOG(LogTemp, Warning, TEXT("结束文件头"));
            // 结束文件头解析
            break;
        }
    }// while   

    if(is_binary){
        // 第二步：读取二进制数据
        UE_LOG(LogTemp, Warning, TEXT("开始读取二进制数据"));
        // 1. 读取顶点数据（假设顶点属性为float x, y, z）
        float useless_float;
        //double useless_double;
        unsigned int useless_int;
        for (int i = 0; i < vertex_count; ++i) {
            FVector v;
            if (is_double) {
                double x, y, z;
                infile.read((char*)(&x), sizeof(double));
                infile.read((char*)(&y), sizeof(double));
                infile.read((char*)(&z), sizeof(double));

                // 处理字节序
                if (is_big) {
                    x = optimized_swap(x);
                    y = optimized_swap(y);
                    z = optimized_swap(z);
                }

                v.X = (float)x;
                v.Y = (float)y;
                v.Z = (float)z;
                infile.read((char*)(&useless_int), sizeof(uint8_t));
                infile.read((char*)(&useless_int), sizeof(uint8_t));
                infile.read((char*)(&useless_int), sizeof(uint8_t));
            }
            else {
                infile.read((char*)(&v.X), sizeof(float));
                infile.read((char*)(&v.Y), sizeof(float));
                infile.read((char*)(&v.Z), sizeof(float));
                infile.read((char*)(&useless_float), sizeof(float));
                infile.read((char*)(&useless_int), sizeof(uint8_t));
                infile.read((char*)(&useless_int), sizeof(uint8_t));
                infile.read((char*)(&useless_int), sizeof(uint8_t));
                //UE_LOG(LogTemp, Warning, TEXT("顶点数据为%f %f %f"), v.X, v.Y, v.Z);
                // 处理字节序
                if (is_big) {
                    v.X = optimized_swap(v.X);
                    v.Y = optimized_swap(v.Y);
                    v.Z = optimized_swap(v.Z);
                }
            }
            //换算到引擎中要大一点
            //v.X *= 1000;
            //v.Y *= 1000;
            //v.Z *= 1000;
            //UE_LOG(LogTemp, Warning, TEXT("处理后的顶点数据为%f %f %f"), v.X, v.Y, v.Z);
            Vertices.Add(v);
        }

        // 2. 读取面数据（假设面属性为list uchar int vertex_indices）
        for (int i = 0; i < face_count; ++i) {
            uint8_t f_vertex_count;
            infile.read(reinterpret_cast<char*>(&f_vertex_count), sizeof(uint8_t));

            for (uint8_t j = 0; j < f_vertex_count; ++j) {
                int32_t index;
                infile.read(reinterpret_cast<char*>(&index), sizeof(int32_t));

                // 处理字节序
                if (is_big) {
                    index = optimized_swap(index);
                }
                //UE_LOG(LogTemp, Warning, TEXT("处理后的三角数据为%d"), index);
                Triangles.Add(index);
            }
            //faces.push_back(face);
        }//for
            
    }
    else {
        // 使用std::getline读取文件中的每一行，直到文件末尾
        for (int i = 0; i < vertex_count; ++i) {
            FVector v;
            FVector ClourV;
            std::getline(infile, line);
            std::istringstream iss(line);
            iss >> v.X >> v.Y >> v.Z;
            iss >> ClourV.X >> ClourV.Y >> ClourV.Z;
            //换算到引擎中要大一点
            //v.X *= 1000;
            //v.Y *= 1000;
            //v.Z *= 1000;
            Vertices.Add(v);
        }
        int a1 = 0;
        // 2. 读取面数据（假设面属性为list uchar int vertex_indices）
        for (int i = 0; i < face_count; ++i) {
            std::getline(infile, line);
            std::istringstream iss(line);
            int num = 0;
            iss >> num;
            
            //UE_LOG(LogTemp, Warning, TEXT("数据为%d"),num);
            for (int j = 0; j < num; j++) {
                int index;
                iss >> index;
                Triangles.Add(index);
                
            }
        }//for
    }
    // 关闭文件
    infile.close();

    //数据清洗
    //ValidateMeshData();
    PlyTArrayStruct Result(Vertices,Triangles);
    return Result;
}

void PlyPaser::Write_File() {
    std::ofstream OutFile(aimfile);

    if (!OutFile.is_open())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to open file"));
        return;
    }

    // 写入顶点数据
    OutFile << "Vertices:\n";
    for (const FVector& Vertex : Vertices)
    {
        OutFile << "X=" << Vertex.X << ", Y=" << Vertex.Y << ", Z=" << Vertex.Z << "\n";
    }

    // 写入三角形索引数据
    OutFile << "\nTriangles:\n";
    for (int32 Index : Triangles)
    {
        OutFile << "F " << Index << "\n";
    }

    OutFile.close();
    UE_LOG(LogTemp, Log, TEXT("Mesh data successfully saved to"));
}


bool PlyPaser::ValidateMeshData()
{

    // 检查顶点数组是否为空
    if (Vertices.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Vertices array is empty."));
        return false;
    }

    // 检查三角形数组是否为空
    if (Triangles.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Triangles array is empty."));
        return false;
    }

    //暂时不用检测
    return true;


    for (int32 i = 0; i < Vertices.Num();)
    {
        if (!(FMath::IsFinite(Vertices[i].X) && FMath::IsFinite(Vertices[i].Y) && FMath::IsFinite(Vertices[i].Z)))
        {
            UE_LOG(LogTemp, Warning, TEXT("移除不存在的点: X=%f, Y=%f, Z=%f"), Vertices[i].X, Vertices[i].Y, Vertices[i].Z);
            Vertices.RemoveAt(i);
        }
        else
        {
            i++;
        }
    }

    std::unordered_set<FVector> UniqueVertices;
    for (int32 i = 0; i < Vertices.Num();)
    {
        if (UniqueVertices.find(Vertices[i]) != UniqueVertices.end())
        {
            UE_LOG(LogTemp, Warning, TEXT("移除重复的点: X=%f, Y=%f, Z=%f"), Vertices[i].X, Vertices[i].Y, Vertices[i].Z);
            Vertices.RemoveAt(i);
        }
        else
        {
            UniqueVertices.insert(Vertices[i]);
            i++;
        }
    }

    UE_LOG(LogTemp, Log, TEXT("数据初步处理完成."));


    // 检查三角形数组中的索引是否超出顶点数组的范围
    for (int32 i = 0; i < Triangles.Num();)
    {
        if (Triangles[i] >= Vertices.Num() || Triangles[i] < 0)
        {
            //UE_LOG(LogTemp, Warning, TEXT("Removing out of range triangle index %d. Vertices array size: %d"), Triangles[i], Vertices.Num());
            Triangles.RemoveAt(i);
        }
        else
        {
            i++;
        }
    }

    // 检查并删除退化三角形
    for (int32 i = 0; i < Triangles.Num()-2; i += 3)
    {
        if (Triangles[i] == Triangles[i + 1] || Triangles[i] == Triangles[i + 2] || Triangles[i + 1] == Triangles[i + 2])
        {
            //UE_LOG(LogTemp, Warning, TEXT("Removing degenerate triangle with indices %d, %d, %d"), Triangles[i], Triangles[i + 1], Triangles[i + 2]);
            Triangles.RemoveAt(i, 3);
            i -= 3;
        }
    }

    // 检查三角形数组的长度是否为3的倍数
    while (Triangles.Num() % 3 != 0)
    {
        Triangles.RemoveAt(Triangles.Num() - 1);
        UE_LOG(LogTemp, Error, TEXT("Triangles array length is not a multiple of 3."));
    }



    // 数据有效
    return true;
}


