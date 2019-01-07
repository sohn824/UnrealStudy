// Fill out your copyright notice in the Description page of Project Settings.

#include "ABCharacterStatComponent.h"
#include "ABGameInstance.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent()
{
	//ĳ���� ���� ������Ʈ�� ���ȿ� ������ �Ͼ ���� ���� �����͸� ó���� ���̹Ƿ� TickComponent�� �ʿ����
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true; //InitializeComponent()�Լ��� �̿��� �ʱ�ȭ ������ �����ϱ� ���ؼ� true�� �����ؾ���

	Level = 1;
}


// Called when the game starts
void UABCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UABCharacterStatComponent::InitializeComponent() //������ PostInitializeComponents�� �����ϴ� �Լ�
{
	Super::InitializeComponent();
	SetNewLevel(Level); //ù ���� ����
}

void UABCharacterStatComponent::SetNewLevel(int32 NewLevel)
{
	auto ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())); //GameInstance�� ���� ����

	ABCHECK(nullptr != ABGameInstance)

	CurrentStatData = ABGameInstance->GetABCharacterData(NewLevel); //ĳ���� ������(�������� �����ߴ���) ����
	
	if (CurrentStatData != nullptr)
	{
		Level = NewLevel;
		SetHP(CurrentStatData->MaxHP); //���� ���ϸ� �ִ� hp�� ����
	}
	else
	{
		ABLOG(Error, TEXT("Level (%d) data doesn't exist"), NewLevel);
	}
}

void UABCharacterStatComponent::SetDamage(float NewDamage) //ĳ������ TakeDamage���� �̰� ȣ���Ұ��� (�� ���� ������Ʈ�� ������ ����� ����ϱ� ����)
{
	ABCHECK(nullptr != CurrentStatData)
	//CurrentHP = FMath::Clamp<float>(CurrentHP - NewDamage, 0.0f, CurrentStatData->MaxHP); //CurrentHp = CurrentHP - NewDamage�� �ϵ�, �� ���� 0.0f~ MaxHP ���̷� ���ѵȴ� 
	//if (CurrentHP <= 0.0f)
	//{
	//	OnHPIsZero.Broadcast(); //���� ü���� 0 ���ϰ� �Ǿ��� ��� ��������Ʈ�� �˷��ش� (�� �� ĳ���Ϳ��� ��������Ʈ�� ȣ���� ���ٽ����� �״� ó���� ��)
	//}

	SetHP(FMath::Clamp<float>(CurrentHP - NewDamage, 0.0f, CurrentStatData->MaxHP)); //CurrentHP�� CurrentHP - NewDamage�� �����ϵ�, �� ���� 0.0f~ MaxHP ���̷� ���ѵȴ�
}

void UABCharacterStatComponent::SetHP(float NewHP)
{
	CurrentHP = NewHP;
	OnHPChanged.Broadcast(); //HP ����ƴٰ� OnHPChanged ��������Ʈ�� �˸�
	if (CurrentHP < KINDA_SMALL_NUMBER) //float���� 0�� ���� �� �̼��� ������ �����ϱ� ���� ���� ���� ������ KINDA_SMALL_NUMBER ��ũ�θ� ����Ѵ�
	{
		CurrentHP = 0.0f;
		OnHPIsZero.Broadcast(); //HP 0�ƴٰ� OnHPIsZero ��������Ʈ�� �˸�
	}
}

float UABCharacterStatComponent::GetAttack() //ĳ���� ���� ������ǥ���� Attack ���� ������ �Լ�
{
	ABCHECK(CurrentStatData != nullptr, 0.0f);
	return CurrentStatData->Attack;
}

float UABCharacterStatComponent::GetHPRatio() //���� HP %�� ������ �Լ�
{
	ABCHECK(CurrentStatData != nullptr, 0.0f); //CurrentStatData�� null�� �ƴ��� �˻�

	//MaxHP�� 0���� ������? 0���� ������ 0 ��ȯ, �ƴϸ� ���� ��ȯ
	return (CurrentStatData->MaxHP < KINDA_SMALL_NUMBER) ? 0.0f : (CurrentHP / CurrentStatData->MaxHP);
}

