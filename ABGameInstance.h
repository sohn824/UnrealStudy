// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "ABGameInstance.generated.h"

USTRUCT(BlueprintType) //이것도 선언해줘야함
struct FABCharacterData : public FTableRowBase //언리얼 엔진에서 제공하는 FTableRowBase 구조체 상속
{
	GENERATED_BODY()

public:
	//사용할 CSV 파일의 각 열의 이름과 동일한 멤버 변수를 타입에 맞춰 선언한다
	FABCharacterData() : Level(1), MaxHP(100.0f), Attack(10.0f), DropExp(10), NextExp(30) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 DropExp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 NextExp;
};

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UABGameInstance();

	virtual void Init() override; //게임 앱이 초기화되면 호출되는 함수
	FABCharacterData *GetABCharacterData(int32 Level);

private:
	UPROPERTY()
	class UDataTable *ABCharacterTable; //언리얼에서 제공되는 데이터 테이블을 관리할 수 있는 오브젝트
};
