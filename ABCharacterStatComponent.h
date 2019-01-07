// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Components/ActorComponent.h"
#include "ABCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHPIsZeroDelegate) //HP가 0이 되었을 때 실행할 델리게이트
DECLARE_MULTICAST_DELEGATE(FOnHPChangedDelegate) //HP값에 변동이 있을 때 실행할 델리게이트

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;

public:
	void SetNewLevel(int32 NewLevel); //레벨 설정 함수
	void SetDamage(float NewDamage);
	void SetHP(float NewHP); //HP 변화할때 호출할 함수
	float GetAttack();
	float GetHPRatio(); //현재 HP %를 반환해줄 함수

	FOnHPIsZeroDelegate OnHPIsZero;
	FOnHPChangedDelegate OnHPChanged;
	
private:
	struct FABCharacterData *CurrentStatData = nullptr; //현재 스탯 데이터
	
	UPROPERTY(EditInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 Level; //현재 레벨

	//Transient = 직렬화(Serialization)에서 제외하는 키워드 (CurrentHP는 계속 변경되므로 필요없는 디스크 공간을 낭비하게 된다)
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
	float CurrentHP;
		
};
