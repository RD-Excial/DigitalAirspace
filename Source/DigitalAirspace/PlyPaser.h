// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <fstream>
#include <Voronoi/Private/voro++/src/cell.hh>
#include <vector>
#include <sstream>
#include <string>
#include "PlyTArrayStruct.h"
/**
 * 
 */
class DIGITALAIRSPACE_API PlyPaser
{
public:
	PlyPaser(std::string filename);
	~PlyPaser();
	PlyTArrayStruct Start();
	void Write_File();
	bool ValidateMeshData();
	std::string sourcefile;
	std::string aimfile;

	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	FVector ply_Locate=FVector(0,0,0);
	TArray<FColor> VertexColors;
};
