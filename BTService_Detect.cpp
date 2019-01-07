// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_Detect.h"
#include "ABAIController.h"
#include "ABCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect");
	Interval = 1.0f; //Ž�� ����
}

//������ Ž�� ���ݸ��� �ѹ��� ȣ��Ǵ� �Լ�
//OwnerComp = ������ �����̺�� Ʈ��
void UBTService_Detect::TickNode(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn *ControllingPawn = OwnerComp.GetAIOwner()->GetPawn(); //���� �÷��̾ ��Ʈ���ϰ� �ִ� ��
	if (ControllingPawn == nullptr)
	{
		return;
	}
	UWorld *World = ControllingPawn->GetWorld();
	FVector Center = ControllingPawn->GetActorLocation();
	float DetectRadius = 600.0f; //Ž�� ���� : 6m
	
	if (World == nullptr)
	{
		return;
	}
	TArray<FOverlapResult> OverlapResults; //�ݰ� ���� ������ ��� ĳ���� ���� �迭
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);
	bool bResult = World->OverlapMultiByChannel //�ݰ� ���� ��� ĳ���͸� �����ϴ� �Լ�
	(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2, //���� ������ Collision ä����
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParam
	);

	if (bResult == true) //������ �����ߴٸ�
	{
		for (auto OverlapResult : OverlapResults) //������ ��� ĳ���͵鿡 ����
		{
			AABCharacter *ABCharacter = Cast<AABCharacter>(OverlapResult.GetActor()); //ĳ���� ������ ���� ��
			if (ABCharacter && ABCharacter->GetController()->IsPlayerController()) //���� �� ĳ���Ͱ� �÷��̾ �����ϴ� ĳ���Ͷ��
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, ABCharacter); //�����̺�� Ʈ���� TargetKey�� �÷��̾� ĳ���͸� ����
				DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);
				DrawDebugPoint(World, ABCharacter->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);
				DrawDebugLine(World, ControllingPawn->GetActorLocation(), ABCharacter->GetActorLocation(), FColor::Blue, false, 0.2f);
				return;
			}
		}
	}
	else //������ �������� ���ߴٸ� TargetKey�� nullptr�� �ٲ��ش�
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, nullptr);
	}

	DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
}