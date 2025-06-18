// Fill out your copyright notice in the Description page of Project Settings.

#include "MySQLLink.h"
#include "UserConfig.h"
#include "Engine/World.h"
#include <cppconn/metadata.h>



MySQLLink::MySQLLink()
{
	UUserConfig* Config = GetMutableDefault<UUserConfig>();
	SQLlink = Config->SQLlink;
	SQLname = Config->SQLname;
	SQLpassword = Config->SQLpassword;

	driver = sql::mysql::get_mysql_driver_instance();
	con = driver->connect(TCHAR_TO_UTF8(*SQLlink), TCHAR_TO_UTF8(*SQLname), TCHAR_TO_UTF8(*SQLpassword));
}

MySQLLink::~MySQLLink()
{
}

void MySQLLink::GetPLYLocation() {
	UE_LOG(LogTemp, Log, TEXT("正在连接到数据库并保存数据！"));
	try
	{
		
		sql::Statement* stamt = con->createStatement();

		//stamt->execute("create DATABASE " "AirGridSystem");
		stamt->execute("USE AirGridSystem");
		stamt->execute("DROP TABLE IF EXISTS PLY_models");
		std::string createTableSQL =
			"id INT AUTO_INCREMENT PRIMARY KEY,"
			"file_name VARCHAR(255) NOT NULL COMMENT '点云模型文件名',"
			"file_path VARCHAR(512) COMMENT '文件存储路径',"
			"coordinate_x DECIMAL(10, 2) COMMENT 'X坐标位置',"
			"coordinate_y DECIMAL(10, 2) COMMENT 'Y坐标位置',"
			"coordinate_z DECIMAL(10, 2) COMMENT 'Z坐标位置',"
			"latitude DECIMAL(10, 6) COMMENT '纬度',"
			"longitude DECIMAL(10, 6) COMMENT '经度',"
			"created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
			"updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,"
			"UNIQUE INDEX idx_file_name(file_name),"
			"INDEX idx_location(coordinate_x, coordinate_y),"
			") ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COMMENT = '点云模型位置表'";

		stamt->execute(createTableSQL);
		//stamt->execute("INSERT INTO website(id, name, domain) VALUES (1, 'pome', 'htts://www.pome.cc')");
		//UE_LOG(LogTemp, Log, TEXT("aaabbb"));
		//写入数据库
		delete stamt;
		delete con;

		UE_LOG(LogTemp, Log, TEXT("数据已保存！"));
	}
	catch (const std::runtime_error err)
	{
		UE_LOG(LogTemp, Warning, TEXT("runtime_error"));
	}
	catch (const sql::SQLException&)
	{
		UE_LOG(LogTemp, Warning, TEXT("SQLException"));
	}
	catch (const std::bad_alloc&)
	{
		UE_LOG(LogTemp, Warning, TEXT("bad_alloc"));
	}
}

void savelittleblock(AirBlockClass::AirBlock& block,sql::Statement& stamt) {
	std::string query = "INSERT INTO air_blocks ("
		"id,grid_x, grid_y, grid_z, level, "
		"world_center_x, world_center_y, world_center_z, size, "
		"state, towerid, weather, wind_power, wind_direction, parent_id) "
		"VALUES (" +
		std::to_string(block.id) + ", " +
		std::to_string(block.GridCoordinates.X) + ", " +
		std::to_string(block.GridCoordinates.Y) + ", " +
		std::to_string(block.GridCoordinates.Z) + ", " +
		std::to_string(block.level) + ", " +
		std::to_string(block.World_center_Location.X) + ", " +
		std::to_string(block.World_center_Location.Y) + ", " +
		std::to_string(block.World_center_Location.Z) + ", " +
		std::to_string(block.size) + ", " +
		std::to_string(block.state) + ", " +
		"'" + block.towerid + "', " +
		"'" + block.weather + "', " +
		std::to_string(block.wind_power) + ", " +
		"'" + block.wind_direction + "', " +
		(block.parent? std::to_string(block.parent->id) : "NULL") +
		") ON DUPLICATE KEY UPDATE " +
		"state=VALUES(state), weather=VALUES(weather), "
		"wind_power=VALUES(wind_power), wind_direction=VALUES(wind_direction)";
	stamt.execute(query);
	for (auto& b : block.children) {
		savelittleblock(*b, stamt);
	}
}

void MySQLLink::SaveBlock(AirBlockClass& Block)
{

	UE_LOG(LogTemp, Log, TEXT("正在连接到数据库并保存数据！"));
	try
	{
		
		sql::Statement* stamt = con->createStatement();

		//stamt->execute("create DATABASE " "AirGridSystem");
		stamt->execute("USE AirGridSystem");
		stamt->execute("DROP TABLE IF EXISTS air_blocks");
		std::string createTableSQL =
			"CREATE TABLE air_blocks("
			"id INT NOT NULL PRIMARY KEY,"
			"grid_x FLOAT NOT NULL COMMENT '网格X坐标',"
			"grid_y FLOAT NOT NULL COMMENT '网格Y坐标',"
			"grid_z FLOAT NOT NULL COMMENT '网格Z坐标',"
			"level INT NOT NULL COMMENT '网格层级(0=根节点)',"
			"world_center_x FLOAT NOT NULL COMMENT '世界中心X坐标',"
			"world_center_y FLOAT NOT NULL COMMENT '世界中心Y坐标',"
			"world_center_z FLOAT NOT NULL COMMENT '世界中心Z坐标',"
			"size FLOAT NOT NULL COMMENT '网格物理尺寸',"
			"state INT NOT NULL COMMENT '0=可通行,1=不可通行,2=混合状态',"
			"towerid VARCHAR(50) COMMENT '电线塔id',"
			"weather VARCHAR(50) COMMENT '天气情况',"
			"wind_power INT COMMENT '风力等级',"
			"wind_direction VARCHAR(20) COMMENT '风向',"
			"parent_id INT COMMENT '父节点ID',"
			"INDEX idx_grid_coords(grid_x, grid_y, grid_z, level),"
			"INDEX idx_spatial(world_center_x, world_center_y, world_center_z),"
			"INDEX idx_parent(parent_id),"
			"INDEX idx_level(level),"
			"INDEX idx_state(state),"
			"FOREIGN KEY(parent_id) REFERENCES air_blocks(id) ON DELETE CASCADE"
			") ENGINE = InnoDB";

		stamt->execute(createTableSQL);
		//stamt->execute("INSERT INTO website(id, name, domain) VALUES (1, 'pome', 'htts://www.pome.cc')");
		//UE_LOG(LogTemp, Log, TEXT("aaabbb"));
		//写入数据库
		for (auto& ABC1 : Block.AirGridCells) {
			for (auto& ABC2 : ABC1) {
				for (auto& ABC3 : ABC2) {
					savelittleblock(ABC3,*stamt);
				}
			}
		}
		delete stamt;
		delete con;

		UE_LOG(LogTemp, Log, TEXT("数据已保存！"));

		//更新系统内数据

	}
	catch (const std::runtime_error err)
	{
		UE_LOG(LogTemp, Warning, TEXT("runtime_error"));
	}
	catch (const sql::SQLException&)
	{
		UE_LOG(LogTemp, Warning, TEXT("SQLException"));
	}
	catch (const std::bad_alloc&)
	{
		UE_LOG(LogTemp, Warning, TEXT("bad_alloc"));
	}

	
}

TArray<FVector> MySQLLink::GetBlockMessage(float size,float gridsize,FVector aimloacte)
{
	UE_LOG(LogTemp, Log, TEXT("正在连接到数据库并读取数据！"));
	try
	{
		
		sql::Statement* stamt = con->createStatement();

		//stamt->execute("create DATABASE " "AirGridSystem");
		stamt->execute("USE AirGridSystem");
		sql::PreparedStatement* pstmt = con->prepareStatement(R"(
            SELECT * FROM air_blocks
        WHERE level = 6
          AND (world_center_x + size/2) > 100 
          AND (world_center_x - size/2) < 3000
          AND (world_center_y + size/2) > 100
          AND (world_center_y - size/2) < 3000
          AND (world_center_z + size/2) > 100
          AND (world_center_z - size/2) < 3000
    )");

		pstmt->setDouble(1, aimloacte.X - size);
		pstmt->setDouble(2, aimloacte.X + size);
		pstmt->setDouble(3, aimloacte.Y - size);
		pstmt->setDouble(4, aimloacte.Y + size);
		pstmt->setDouble(5, aimloacte.Z - size);
		pstmt->setDouble(6, aimloacte.Z + size);

		sql::ResultSet* res = pstmt->executeQuery();
		
		//返回一个数组

		UE_LOG(LogTemp, Log, TEXT("数据已保存！"));
	}
	catch (const std::runtime_error err)
	{
		UE_LOG(LogTemp, Warning, TEXT("runtime_error"));
	}
	catch (const sql::SQLException&)
	{
		UE_LOG(LogTemp, Warning, TEXT("SQLException"));
	}
	catch (const std::bad_alloc&)
	{
		UE_LOG(LogTemp, Warning, TEXT("bad_alloc"));
	}
	return TArray<FVector>();
}

FVector Latitude_Longitude_to_UE(FVector f)
{
	const UUserConfig* Config = GetDefault<UUserConfig>();
	FVector r;
	//X对应经度，Y对应纬度，Z对应高度
	r.X = Config->base_x + 6378137 * FMath::Cos(Config->base_latitude * (Config->P / 180)) * (f.X - Config->base_longitude) * (Config->P / 180) * 100;
	r.Y = Config->base_y + 6378137 * (f.Y - Config->base_latitude) * (Config->P / 180) * 100;
	r.Z = f.Z * 100;
	return r;
}
void MySQLLink::SaveWeather(float lon1, float lat1, float lon2, float lat2, FString weather, FString wind_direction, float wind_power)
{
	const UUserConfig* Config = GetDefault<UUserConfig>();
	FVector f1 = Latitude_Longitude_to_UE(FVector(lon1, lat1, 0));
	FVector f2 = Latitude_Longitude_to_UE(FVector(lon2, lat2, 0));
	try
	{
		
		sql::Statement* stamt = con->createStatement();

		//stamt->execute("create DATABASE " "AirGridSystem");
		stamt->execute("USE AirGridSystem");
		sql::PreparedStatement* pstmt = con->prepareStatement(R"(
            UPDATE air_blocks
			SET 
			weather = ?,
			wind_power = ?,
			wind_direction = ?
			WHERE 
			grid_x BETWEEN ? AND ?
			AND grid_y BETWEEN ? AND ?
    )");
		pstmt->setString(1, TCHAR_TO_UTF8(*weather));
		pstmt->setDouble(2, wind_power);
		pstmt->setString(3, TCHAR_TO_UTF8(*wind_direction));
		pstmt->setDouble(4, f1.X/20000 * 20000);
		pstmt->setDouble(5, f2.X/20000*20000);
		pstmt->setDouble(6, f1.Y/20000 * 20000);
		pstmt->setDouble(7, f2.Y/ 20000 * 20000);
		sql::ResultSet* res = pstmt->executeQuery();

		UE_LOG(LogTemp, Log, TEXT("网格天气数据已修改！x为%f和%f,y为%f和%f"), f1.X, f2.X,f1.Y,f2.Y);
		//更新系统内网格天气
		WeatherDataClass WDC;
		WDC.af = f1;
		WDC.bf = f2;
		WDC.weather = weather;
		WDC.wind_direction = wind_direction;
		WDC.wind_power = wind_power;
		UpdateAirgrid(WDC);
	}
	catch (const std::runtime_error err)
	{
		UE_LOG(LogTemp, Warning, TEXT("runtime_error"));
	}
	catch (const sql::SQLException&)
	{
		UE_LOG(LogTemp, Warning, TEXT("SQLException"));
	}
	catch (const std::bad_alloc&)
	{
		UE_LOG(LogTemp, Warning, TEXT("bad_alloc"));
	}
}

void MySQLLink::UpdatePlyLocate(FVector f,FString fs)
{
	if (ply_update.IsBound()) {
		UE_LOG(LogTemp, Log, TEXT("UpdatePlyLocate委托步骤：broadcast"));
		ply_update.Broadcast(f, fs);
	}
	
}

void MySQLLink::UpdateAirlineLocate(TArray<FVector> AllPoints)
{
	UE_LOG(LogTemp, Log, TEXT("UpdateAirlineLocate委托步骤：broadcast"));
	if (airline_update.IsBound()) {
		UE_LOG(LogTemp, Log, TEXT("UpdateAirlineLocate已经绑定！"));
		airline_update.Broadcast(AllPoints);
	}
}

void MySQLLink::UpdateAirgrid(WeatherDataClass WDC)
{
	UE_LOG(LogTemp, Log, TEXT("UpdateAirgrid委托步骤：broadcast"));
	
	if (airgrid_update.IsBound()) {
		UE_LOG(LogTemp, Log, TEXT("UpdateAirgrid已经绑定！"));
		airgrid_update.Broadcast(WDC);
	}
}

void MySQLLink::SavePoint(FString filename, float lon, float lat,float ele)
{
	UE_LOG(LogTemp, Log, TEXT("正在连接到数据库并保存点云数据！"));
	try
	{
		std::unique_ptr<sql::Statement> stmt(con->createStatement());
		stmt->execute("CREATE DATABASE IF NOT EXISTS AirGridSystem");
		stmt->execute("USE AirGridSystem");
		// 2. 直接使用 Statement 创建表
		stmt->execute(R"(
            CREATE TABLE IF NOT EXISTS Point_file (
                id INT AUTO_INCREMENT PRIMARY KEY COMMENT '点云文件id',
                file_name VARCHAR(255) NOT NULL COMMENT '点云模型文件名',
				longitude DECIMAL(10, 6) NOT NULL COMMENT '经度',
                latitude DECIMAL(10, 6) NOT NULL COMMENT '纬度',
				elevation DECIMAL(10, 6) NOT NULL COMMENT '海拔'
            ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
        )");
		// 3. 插入数据
		int count = 0;
		if (stmt) {
			std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("SELECT COUNT(*) FROM Point_file WHERE file_name = ?"));
			pstmt->setString(1, TCHAR_TO_UTF8(*filename));
			std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
			if (res->next()) {
				count = res->getInt(1);
				UE_LOG(LogTemp, Log, TEXT("数据库已经存在%s"), *filename);
			}
		}
		//如果存在就更新
		if (count > 0) {
			UE_LOG(LogTemp, Log, TEXT("正在调用委托"));
			std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(R"(
            UPDATE Point_file
			SET
				longitude = ? ,
				latitude = ? ,
				elevation = ?
				WHERE
				file_name = ?
        )"));
			FVector UElocation = Latitude_Longitude_to_UE(FVector(lon, lat, ele));
			pstmt->setDouble(1, lon);
			pstmt->setDouble(2, lat);
			pstmt->setDouble(3, ele);
			pstmt->setString(4, TCHAR_TO_UTF8(*filename));
			pstmt->executeUpdate(); // 使用 executeUpdate 插入数据
			//把经纬度转换为虚幻引擎坐标后加载到对应位置
			UpdatePlyLocate(Latitude_Longitude_to_UE(FVector(lon, lat, ele)),filename);
			
		}
		//如果不存在就插入
		else {
			std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(R"(
            INSERT INTO Point_file (
                file_name, longitude, latitude, elevation
            ) VALUES (?, ?, ?, ?)
        )"));
			pstmt->setString(1, TCHAR_TO_UTF8(*filename));
			pstmt->setDouble(2, lon);
			pstmt->setDouble(3, lat);
			pstmt->setDouble(4, ele);
			pstmt->executeUpdate(); // 使用 executeUpdate 插入数据
		}

		UE_LOG(LogTemp, Log, TEXT("点云位置数据已保存！"));
	}
	catch (const std::runtime_error err)
	{
		UE_LOG(LogTemp, Warning, TEXT("保存点云数据runtime_error"));
	}
	catch (const sql::SQLException&)
	{
		UE_LOG(LogTemp, Warning, TEXT("保存点云数据SQLException"));
	}
	catch (const std::bad_alloc&)
	{
		UE_LOG(LogTemp, Warning, TEXT("保存点云数据bad_alloc"));
	}
}

FVector MySQLLink::GetPointMessage(FString filename)
{
	UE_LOG(LogTemp, Warning, TEXT("正在获取数据库点云数据"));
	FVector f(0,0,-1);
	try {
		
		std::unique_ptr<sql::Statement> stmt(con->createStatement());
		stmt->execute("USE AirGridSystem");

		std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
			"SELECT * FROM Point_file WHERE file_name = ?"
		));
		pstmt->setString(1, TCHAR_TO_UTF8(*filename));

		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		while (res->next()) {
			f.X = res->getDouble("longitude");
			f.Y = res->getDouble("latitude");
			f.Z = res->getDouble("elevation");
			//把经纬度转换为虚幻引擎坐标
			f = Latitude_Longitude_to_UE(f);
		}
		
	}
	catch (const std::runtime_error err)
	{
		UE_LOG(LogTemp, Warning, TEXT("runtime_error"));
	}
	catch (const sql::SQLException&)
	{
		UE_LOG(LogTemp, Warning, TEXT("SQLException"));
	}
	catch (const std::bad_alloc&)
	{
		UE_LOG(LogTemp, Warning, TEXT("bad_alloc"));
	}
	return f;
}

void MySQLLink::SaveAirLine(FString linename, FString points, TArray<FVector> AllPoints)
{
	UE_LOG(LogTemp, Log, TEXT("正在连接到数据库并保存航线数据！"));
	try
	{
		std::unique_ptr<sql::Statement> stmt(con->createStatement());
		stmt->execute("CREATE DATABASE IF NOT EXISTS AirGridSystem");
		stmt->execute("USE AirGridSystem");
		// 2. 直接使用 Statement 创建表
		stmt->execute(R"(
            CREATE TABLE IF NOT EXISTS Airlines (
                id INT AUTO_INCREMENT PRIMARY KEY COMMENT '航线id',
                line_name VARCHAR(255) NOT NULL COMMENT '航线名',
				locates JSON NOT NULL COMMENT '存储格式: [[经度,纬度,高度],[经度,纬度,高度],...]'
            ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
        )");
		// 3. 插入数据
		int count = 0;
		if (stmt) {
			std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("SELECT COUNT(*) FROM Airlines WHERE line_name = ?"));
			pstmt->setString(1, TCHAR_TO_UTF8(*linename));
			std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
			if (res->next()) {
				count = res->getInt(1);
			}
		}
		//如果存在就更新
		if (count > 0) {
			std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(R"(
				UPDATE Airlines
				SET locates = ?
				WHERE line_name = ?
			)"));
			pstmt->setString(1, TCHAR_TO_UTF8(*points));   // 新的locates值
			pstmt->setString(2, TCHAR_TO_UTF8(*linename)); // 要更新的line_name
			pstmt->executeUpdate();
			UE_LOG(LogTemp, Log, TEXT("数据库已经存在%s航线"), *linename);
		}
		//如果不存在就插入
		else {
			std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(R"(
            INSERT INTO Airlines (
                line_name, locates
            ) VALUES (?, ?)
        )"));
			pstmt->setString(1, TCHAR_TO_UTF8(*linename));
			pstmt->setString(2, TCHAR_TO_UTF8(*points));
			pstmt->executeUpdate(); // 使用 executeUpdate 插入数据
		}

		UE_LOG(LogTemp, Log, TEXT("航线位置数据已保存！"));

		//显示航迹
		
		UpdateAirlineLocate(AllPoints);
	}
	catch (const std::runtime_error err)
	{
		UE_LOG(LogTemp, Warning, TEXT("保存航线数据runtime_error"));
	}
	catch (const sql::SQLException&)
	{
		UE_LOG(LogTemp, Warning, TEXT("保存航线数据SQLException"));
	}
	catch (const std::bad_alloc&)
	{
		UE_LOG(LogTemp, Warning, TEXT("保存航线数据bad_alloc"));
	}
}


