// Fill out your copyright notice in the Description page of Project Settings.

#include "ABAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName AABAIController::HomePosKey(TEXT("HomePos"));
const FName AABAIController::PatrolPosKey(TEXT("PatrolPos"));
const FName AABAIController::TargetKey(TEXT("Target"));

AABAIController::AABAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData>
		BBObject(TEXT("/Game/AI/BB_ABCharacter.BB_ABCharacter")); //블랙보드 멤버변수 지정
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree>
		BTObject(TEXT("/Game/AI/BT_ABCharacter.BT_ABCharacter")); //비헤이비어 트리 멤버변수 지정
	if(BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}
}

void AABAIController::Possess(APawn * InPawn)
{
	Super::Possess(InPawn);
	if (UseBlackboard(BBAsset, Blackboard))
	{
		Blackboard->SetValueAsVector(HomePosKey, InPawn->GetActorLocation()); //HomePos 키값 지정
		if (!RunBehaviorTree(BTAsset))
		{
			ABLOG(Error, TEXT("AIController couldn't run behavior tree"));
		}
	}
}
