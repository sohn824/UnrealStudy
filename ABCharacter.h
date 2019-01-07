// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Character.h"
#include "ABCharacter.generated.h"

UCLASS()
class ARENABATTLE_API AABCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	enum class EControlMode
	{
		GTA,
		DIABLO,
		NPC
	};
	void SetControlMode(EControlMode NewControlMode); //컨트롤 모드 설정 함수

	//버그 방지를 위해 초기 값은 반드시 설정하는 것이 좋다
	EControlMode CurrentControlMode = EControlMode::GTA;
	FVector DirectionToMove = FVector::ZeroVector;

	float ArmLengthTo = 0.0f;
	FRotator ArmRotationTo = FRotator::ZeroRotator;
	float ArmLengthSpeed = 0.0f;
	float ArmRotationSpeed = 0.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& 
		DamageEvent, class AController *EventInstigator, AActor *DamageCauser) override;
	virtual void PossessedBy(AController *NewController) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool CanSetWeapon();
	void SetWeapon(class AABWeapon *NewWeapon);
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	class AABWeapon *CurrentWeapon;

	UPROPERTY(VisibleAnywhere, Category = Stat)
	class UABCharacterStatComponent *CharacterStat; //만든 캐릭터 스탯관리 액터 컴포넌트를 부착 (액터가 아니라 액터 컴포넌트임 ㅠㅠ)

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent *SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent *Camera;

	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent *HPBarWidget; //UWidgetComponent = 액터에 UI 위젯을 부착할 수 있도록 언리얼에서 제공하는 클래스
	//하지만 이것을 사용하려면 ArenaBattle.Build.cs 파일에서 PublicDependencyModuleNames.AddRange()에서 "UMG"를 추가해야 한다

private:
	void UpDown(float NewAxisValue);
	void LeftRight(float NewAxisValue);
	void LookUp(float NewAxisValue);
	void Turn(float NewAxisValue);

	void ViewChange(); //Shift+V로 할당해놓음
	void Attack(); //마우스 왼쪽버튼

	UFUNCTION() //블루프린트와 관련될 함수는 UFUNCTION 매크로를 선언해야 한다
	void OnAttackMontageEnded(UAnimMontage *Montage, bool bInterrupted); //델리게이트 함수

	void AttackStartComboState(); //공격이 시작될때 호출할 함수
	void AttackEndComboState(); //공격이 종료될때 호출할 함수

	void AttackCheck(); //공격 판정 체크 함수

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsAttacking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool CanNextCombo; //다음 콤보로 이동할 수 있는지 여부

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsComboInputOn; //콤보 입력중인지 여부

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentCombo; //현재 실행중인 콤보 카운트

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 MaxCombo; //캐릭터가 사용할 수 있는 최대 콤보 카운트

	UPROPERTY()
	class UABAnimInstance *ABAnim; //ABCharacter 클래스에서 애니메이션 인스턴스를 자주 사용할 예정이므로 멤버 변수로 선언해둔다

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRange;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRadius;


};
