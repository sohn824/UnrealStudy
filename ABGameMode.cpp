// Fill out your copyright notice in the Description page of Project Settings.

#include "ABGameMode.h"
#include "ABCharacter.h"
#include "ABPlayerController.h"

AABGameMode::AABGameMode()
{
	DefaultPawnClass = AABCharacter::StaticClass(); //게임 모드에 폰(사용자가 조종 가능한 액터)을 지정
	PlayerControllerClass = AABPlayerController::StaticClass(); //게임 모드에 플레이어 컨트롤러를 지정

	//블루 프린트의 클래스 정보를 C++에서 가져올 수 있다
	/*
	static ConstructorHelpers::FClassFinder<APawn>
		BP_PAWN_C(TEXT("/Game/ThirdPersonBP/Blueprints/ThirdPersonCharacter.ThirdPersonCharacter_C")); //블루프린트 에셋 경로 뒤에 _C를 붙일 경우 에셋의 클래스 정보를 가져올 수 있다
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
