// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// 유틸리티 매크로
DECLARE_LOG_CATEGORY_EXTERN(DEBUG, Log, All);
#define INFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define PRINT_INFO() UE_LOG(DEBUG, Warning, TEXT("%s"), *INFO)
#define PRINT_LOG(fmt, ...) UE_LOG(DEBUG, Warning, TEXT("%s %s"), *INFO, *FString::Printf(fmt, ##__VA_ARGS__))
