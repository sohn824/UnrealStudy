// Fill out your copyright notice in the Description page of Project Settings.

#include "ABGameMode.h"
#include "ABCharacter.h"
#include "ABPlayerController.h"

AABGameMode::AABGameMode()
{
	DefaultPawnClass = AABCharacter::StaticClass(); //���� ��忡 ��(����ڰ� ���� ������ ����)�� ����
	PlayerControllerClass = AABPlayerController::StaticClass(); //���� ��忡 �÷��̾� ��Ʈ�ѷ��� ����

	//��� ����Ʈ�� Ŭ���� ������ C++���� ������ �� �ִ�
	/*
	static ConstructorHelpers::FClassFinder<APawn>
		BP_PAWN_C(TEXT("/Game/ThirdPersonBP/Blueprints/ThirdPersonCharacter.ThirdPersonCharacter_C")); //�������Ʈ ���� ��� �ڿ� _C�� ���� ��� ������ Ŭ���� ������ ������ �� �ִ�
	if (BP_PAWN_C.Succeeded())
	{
		DefaultPawnClass = BP_PAWN_C.Class;
	}
	*/
}
void AABGameMode::PostLogin(APlayerController * NewPlayer)
{
	UE_LOG(ArenaBattle, Warning, TEXT("PostLogin Begin"));
	Super::PostLogin(NewPlayer);
	UE_LOG(ArenaBattle, Warning, TEXT("PostLogin End"));
}
