// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "AIController.h"
#include "ABAIController.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABAIController : public AAIController
{
	GENERATED_BODY()
public:
	AABAIController();
	virtual void Possess(APawn *InPawn) override;
	
	static const FName HomePosKey; //블랙보드 키값 (시작 위치)
	static const FName PatrolPosKey; //블랙보드 키값 (순찰 위치)
	static const FName TargetKey; //블랙보드 키값 (타겟)

private:
	UPROPERTY()
	class UBehaviorTree *BTAsset;
	
	UPROPERTY()
	class UBlackboardData *BBAsset;
};
