// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <string>
#include <fstream>
#include <Voronoi/Private/voro++/src/cell.hh>
#include <vector>
#include <sstream>


/**
 * 
 */
class DIGITALAIRSPACE_API PointCloud2OBJ
{
public:
	PointCloud2OBJ(std::string sf, std::string af);
	~PointCloud2OBJ();
	void Start();
	

	std::string sourcefile;
	std::string aimfile;
};
