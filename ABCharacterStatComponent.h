// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Components/ActorComponent.h"
#include "ABCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHPIsZeroDelegate) //HP�� 0�� �Ǿ��� �� ������ ��������Ʈ
DECLARE_MULTICAST_DELEGATE(FOnHPChangedDelegate) //HP���� ������ ���� �� ������ ��������Ʈ

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
	void SetNewLevel(int32 NewLevel); //���� ���� �Լ�
	void SetDamage(float NewDamage);
	void SetHP(float NewHP); //HP ��ȭ�Ҷ� ȣ���� �Լ�
	float GetAttack();
	float GetHPRatio(); //���� HP %�� ��ȯ���� �Լ�

	FOnHPIsZeroDelegate OnHPIsZero;
	FOnHPChangedDelegate OnHPChanged;
	
private:
	struct FABCharacterData *CurrentStatData = nullptr; //���� ���� ������
	
	UPROPERTY(EditInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 Level; //���� ����

	//Transient = ����ȭ(Serialization)���� �����ϴ� Ű���� (CurrentHP�� ��� ����ǹǷ� �ʿ���� ��ũ ������ �����ϰ� �ȴ�)
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
	float CurrentHP;
		
};
