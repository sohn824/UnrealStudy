// Fill out your copyright notice in the Description page of Project Settings.

#include "ABPlayerController.h"

void AABPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AABPlayerController::Possess(APawn *aPawn) //컨트롤러가 폰에 빙의한다
{
	Super::Possess(aPawn);
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputMode;  //UI를 배제하고 게임에게만 입력을 전달
	SetInputMode(InputMode);
}