// Fill out your copyright notice in the Description page of Project Settings.

#include "ABPlayerController.h"

void AABPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AABPlayerController::Possess(APawn *aPawn) //��Ʈ�ѷ��� ���� �����Ѵ�
{
	Super::Possess(aPawn);
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputMode;  //UI�� �����ϰ� ���ӿ��Ը� �Է��� ����
	SetInputMode(InputMode);
}