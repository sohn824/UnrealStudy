// Fill out your copyright notice in the Description page of Project Settings.

#include "ABCharacterStatComponent.h"
#include "ABGameInstance.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent()
{
	//캐릭터 스탯 컴포넌트는 스탯에 변경이 일어날 때만 관련 데이터를 처리할 것이므로 TickComponent가 필요없다
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true; //InitializeComponent()함수를 이용해 초기화 로직을 구현하기 위해서 true로 설정해야함

	Level = 1;
}


// Called when the game starts
void UABCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UABCharacterStatComponent::InitializeComponent() //액터의 PostInitializeComponents와 대응하는 함수
{
	Super::InitializeComponent();
	SetNewLevel(Level); //첫 레벨 설정
}

void UABCharacterStatComponent::SetNewLevel(int32 NewLevel)
{
	auto ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())); //GameInstance의 정보 저장

	ABCHECK(nullptr != ABGameInstance)

	CurrentStatData = ABGameInstance->GetABCharacterData(NewLevel); //캐릭터 데이터(엑셀파일 저장했던거) 정보
	
	if (CurrentStatData != nullptr)
	{
		Level = NewLevel;
		SetHP(CurrentStatData->MaxHP); //레벨 업하면 최대 hp로 리셋
	}
	else
	{
		ABLOG(Error, TEXT("Level (%d) data doesn't exist"), NewLevel);
	}
}

void UABCharacterStatComponent::SetDamage(float NewDamage) //캐릭터의 TakeDamage에서 이걸 호출할거임 (이 액터 컴포넌트가 데미지 계산을 담당하기 위해)
{
	ABCHECK(nullptr != CurrentStatData)
	//CurrentHP = FMath::Clamp<float>(CurrentHP - NewDamage, 0.0f, CurrentStatData->MaxHP); //CurrentHp = CurrentHP - NewDamage를 하되, 그 값은 0.0f~ MaxHP 사이로 제한된다 
	//if (CurrentHP <= 0.0f)
	//{
	//	OnHPIsZero.Broadcast(); //현재 체력이 0 이하가 되었을 경우 델리게이트로 알려준다 (그 후 캐릭터에서 델리게이트를 호출해 람다식으로 죽는 처리를 함)
	//}

	SetHP(FMath::Clamp<float>(CurrentHP - NewDamage, 0.0f, CurrentStatData->MaxHP)); //CurrentHP를 CurrentHP - NewDamage로 설정하되, 그 값은 0.0f~ MaxHP 사이로 제한된다
}

void UABCharacterStatComponent::SetHP(float NewHP)
{
	CurrentHP = NewHP;
	OnHPChanged.Broadcast(); //HP 변경됐다고 OnHPChanged 델리게이트에 알림
	if (CurrentHP < KINDA_SMALL_NUMBER) //float값을 0과 비교할 때 미세한 오차를 측정하기 위해 아주 작은 숫자인 KINDA_SMALL_NUMBER 매크로를 사용한다
	{
		CurrentHP = 0.0f;
		OnHPIsZero.Broadcast(); //HP 0됐다고 OnHPIsZero 델리게이트에 알림
	}
}

float UABCharacterStatComponent::GetAttack() //캐릭터 스탯 데이터표에서 Attack 값을 얻어오는 함수
{
	ABCHECK(CurrentStatData != nullptr, 0.0f);
	return CurrentStatData->Attack;
}

float UABCharacterStatComponent::GetHPRatio() //현재 HP %를 얻어오는 함수
{
	ABCHECK(CurrentStatData != nullptr, 0.0f); //CurrentStatData가 null이 아닌지 검사

	//MaxHP가 0보다 작은가? 0보다 작으면 0 반환, 아니면 정상값 반환
	return (CurrentStatData->MaxHP < KINDA_SMALL_NUMBER) ? 0.0f : (CurrentHP / CurrentStatData->MaxHP);
}

