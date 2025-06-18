// Fill out your copyright notice in the Description page of Project Settings.


#include "PointCloud2OBJ.h"



// 顶点结构体
struct Vertex {
    float x, y, z;
};

// 面结构体（存储顶点索引）
struct Face {
    std::vector<int> indices;
};

// 字节序转换工具（若主机字节序与文件不同则反转字节）
template <typename T>
T swap_endian(const T& value) {
    T result = value;
    char* bytes = reinterpret_cast<char*>(&result);
    std::reverse(bytes, bytes + sizeof(T));
    return result;
}

PointCloud2OBJ::PointCloud2OBJ(std::string sf, std::string af)
{
	sourcefile = sf;
	aimfile = af;
}

PointCloud2OBJ::~PointCloud2OBJ()
{
}

void PointCloud2OBJ::Start() {
	UE_LOG(LogTemp, Warning, TEXT("启动转换"));

    std::ifstream infile(sourcefile);
    std::ofstream obj_file(aimfile);
    std::string line;


    std::vector<Vertex> vertices;
    std::vector<Face> faces;

    bool reading_vertices = false;
    bool reading_faces = false;
    bool is_binary = false;
    bool is_big = false;
    int vertex_count = 0;
    int face_count = 0;
    

    // 检查文件是否成功打开
    if (!infile.is_open()) {
        UE_LOG(LogTemp, Warning, TEXT("无法打开文件"));
        return;
    }
    if (!obj_file.is_open()) {
        UE_LOG(LogTemp, Warning, TEXT("无法打开文件"));
        return;
    }

    while (std::getline(infile, line)) {
        // 如果找到关键词，则从头开始读取
        if (line.find("binary") != std::string::npos) {
            infile.seekg(0, std::ios::beg); // 将文件指针移到文件开头
            is_binary = true;
            break; // 退出内层循环，重新开始读取
        }
        if (line.find("format") != std::string::npos) {
            infile.seekg(0, std::ios::beg);
            break;
        }
    }
    
    infile.close();
    if (is_binary) {
        infile.open(sourcefile, std::ios::binary);
        while (std::getline(infile, line)) {
            FString HappyString(UTF8_TO_TCHAR(line.c_str()));
            UE_LOG(LogTemp, Warning, TEXT("%s"), *HappyString);
            std::istringstream iss(line);
            std::string token;
            iss >> token;

            // 1. 解析顶点数量
            if (token == "element" && !reading_vertices) {
                
                std::string type;
                iss >> type;
                if (type == "vertex") {
                    reading_vertices = true;
                    iss >> vertex_count;
                    vertices.reserve(vertex_count);
                    //UE_LOG(LogTemp, Warning, TEXT("顶点数%d"), vertex_count);
                }
            }
            // 2. 解析面数量
            if (token == "element" && !reading_faces) {
                
                
                std::string type;
                iss >> type;
                if (type == "face") {
                    reading_faces = true;
                    iss >> face_count;
                    faces.reserve(face_count);
                    //UE_LOG(LogTemp, Warning, TEXT("面数%d"), face_count);
                }
            }
            //解析大小段
            if (line.find("format") != std::string::npos) {
                if (line.find("big") != std::string::npos) {
                    is_big = true;
                }
                else if (line.find("little") != std::string::npos) {
                    is_big = false;
                }
            }

            if (line == "end header") {
                reading_faces = true;
                reading_vertices = true;
                UE_LOG(LogTemp, Warning, TEXT("结束文件头"));
                // 结束文件头解析
            }

            // 第二步：读取二进制数据
            if (reading_vertices && reading_faces) {
                UE_LOG(LogTemp, Warning, TEXT("开始读取数据"));
                // 1. 读取顶点数据（假设顶点属性为float x, y, z）
                for (size_t i = 0; i < vertex_count; ++i) {
                    Vertex v;
                    infile.read(reinterpret_cast<char*>(&v.x), sizeof(float));
                    infile.read(reinterpret_cast<char*>(&v.y), sizeof(float));
                    infile.read(reinterpret_cast<char*>(&v.z), sizeof(float));

                    // 处理字节序
                    if (is_big) {
                        v.x = swap_endian(v.x);
                        v.y = swap_endian(v.y);
                        v.z = swap_endian(v.z);
                    }

                    vertices.push_back(v);
                }

                // 2. 读取面数据（假设面属性为list uchar int vertex_indices）
                for (size_t i = 0; i < face_count; ++i) {
                    uint8_t f_vertex_count;
                    infile.read(reinterpret_cast<char*>(&f_vertex_count), sizeof(uint8_t));

                    Face face;
                    for (uint8_t j = 0; j < f_vertex_count; ++j) {
                        int32_t index;
                        infile.read(reinterpret_cast<char*>(&index), sizeof(int32_t));

                        // 处理字节序
                        if (is_big) {
                            index = swap_endian(index);
                        }
                        face.indices.push_back(index);
                    }
                    faces.push_back(face);
                }//for
            }//if
        }//while
    
    }
    else {
        infile.open(sourcefile);
        // 使用std::getline读取文件中的每一行，直到文件末尾
        while (std::getline(infile, line)) {
            std::istringstream iss(line);
            std::string token;
            iss >> token;

            // 1. 解析顶点数量
            if (token == "element" && !reading_vertices) {
                std::string type;
                iss >> type;
                if (type == "vertex") {
                    iss >> vertex_count;
                    vertices.reserve(vertex_count);
                }
            }
            // 2. 解析面数量
            else if (token == "element" && !reading_faces) {
                std::string type;
                iss >> type;
                if (type == "face") {
                    iss >> face_count;
                    faces.reserve(face_count);
                }
            }
            // 3. 开始读取顶点数据
            else if (token == "end header") {
                reading_vertices = true;
            }
            // 4. 读取顶点坐标
            else if (reading_vertices && vertices.size() < vertex_count) {
                Vertex v;
                iss >> v.x >> v.y >> v.z;
                vertices.push_back(v);
            }
            // 5. 读取面索引
            else if (vertices.size() == vertex_count && faces.size() < face_count) {
                int num_vertices;
                iss >> num_vertices; // 面的顶点数（如3表示三角形）
                Face face;
                for (int i = 0; i < num_vertices; ++i) {
                    int idx;
                    iss >> idx;
                    face.indices.push_back(idx);
                }
                faces.push_back(face);
            }
        }
    }

    

    // 关闭文件
    infile.close();

    // 第二步：写入OBJ文件
    // 1. 写入顶点
    for (const auto& v : vertices) {
        obj_file << "v " << v.x << " " << v.y << " " << v.z << "\n";
    }
    obj_file << face_count;
    // 2. 写入面（自动三角化）
    for (const auto& f : faces) {
        obj_file << "f ";
        for (const auto& idx : f.indices) {
            obj_file << (idx + 1) << " "; // OBJ索引从1开始
        }
        obj_file << "\n";
    }
    obj_file.close();
    UE_LOG(LogTemp, Warning, TEXT("完成文件转换"));
    return;
}
