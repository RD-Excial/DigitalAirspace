// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class DIGITALAIRSPACE_API PlyTArrayStruct
{
public:
    TArray<FVector> Vertices;
    TArray<int32> Triangles;

    // ���캯���������ʼ����

    PlyTArrayStruct(const TArray<FVector>& InVertices, const TArray<int32>& InTriangles)
        : Vertices(InVertices), Triangles(InTriangles) {}
	PlyTArrayStruct();
	~PlyTArrayStruct();
};
