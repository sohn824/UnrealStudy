// Fill out your copyright notice in the Description page of Project Settings.

#include "ABGameInstance.h"

UABGameInstance::UABGameInstance()
{
	FString CharacterDataPath = TEXT("/Game/GameData/ABCharacterData.ABCharacterData");
	static ConstructorHelpers::FObjectFinder<UDataTable>
		DT_ABCHARACTER(*CharacterDataPath);
	if (!DT_ABCHARACTER.Succeeded())
	{
		UE_LOG(LogClass, Warning, TEXT("Character Data Load Failed"));
		return;
	}
	ABCharacterTable = DT_ABCHARACTER.Object;
	if (ABCharacterTable->GetRowMap().Num() == 0)
	{
		UE_LOG(LogClass, Warning, TEXT("There is no data"));
		return;
	}
}

void UABGameInstance::Init()
{
	Super::Init();
}

FABCharacterData *UABGameInstance::GetABCharacterData(int32 Level) //캐릭터 데이터 정보 반환
{
	return ABCharacterTable->FindRow<FABCharacterData>(*FString::FromInt(Level), TEXT(""));
}