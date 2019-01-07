// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_Detect.h"
#include "ABAIController.h"
#include "ABCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect");
	Interval = 1.0f; //탐지 간격
}

//설정한 탐지 간격마다 한번씩 호출되는 함수
//OwnerComp = 본인의 비헤이비어 트리
void UBTService_Detect::TickNode(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn *ControllingPawn = OwnerComp.GetAIOwner()->GetPawn(); //현재 플레이어가 컨트롤하고 있는 폰
	if (ControllingPawn == nullptr)
	{
		return;
	}
	UWorld *World = ControllingPawn->GetWorld();
	FVector Center = ControllingPawn->GetActorLocation();
	float DetectRadius = 600.0f; //탐지 범위 : 6m
	
	if (World == nullptr)
	{
		return;
	}
	TArray<FOverlapResult> OverlapResults; //반경 내에 감지된 모든 캐릭터 정보 배열
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);
	bool bResult = World->OverlapMultiByChannel //반경 내에 모든 캐릭터를 감지하는 함수
	(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2, //내가 설정한 Collision 채널임
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParam
	);

	if (bResult == true) //감지에 성공했다면
	{
		for (auto OverlapResult : OverlapResults) //감지된 모든 캐릭터들에 대해
		{
			AABCharacter *ABCharacter = Cast<AABCharacter>(OverlapResult.GetActor()); //캐릭터 정보를 얻어온 후
			if (ABCharacter && ABCharacter->GetController()->IsPlayerController()) //만약 그 캐릭터가 플레이어가 조종하는 캐릭터라면
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, ABCharacter); //비헤이비어 트리의 TargetKey에 플레이어 캐릭터를 설정
				DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);
				DrawDebugPoint(World, ABCharacter->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);
				DrawDebugLine(World, ControllingPawn->GetActorLocation(), ABCharacter->GetActorLocation(), FColor::Blue, false, 0.2f);
				return;
			}
		}
	}
	else //감지에 성공하지 못했다면 TargetKey를 nullptr로 바꿔준다
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, nullptr);
	}

	DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
}