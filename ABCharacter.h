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
	void SetControlMode(EControlMode NewControlMode); //��Ʈ�� ��� ���� �Լ�

	//���� ������ ���� �ʱ� ���� �ݵ�� �����ϴ� ���� ����
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
	class UABCharacterStatComponent *CharacterStat; //���� ĳ���� ���Ȱ��� ���� ������Ʈ�� ���� (���Ͱ� �ƴ϶� ���� ������Ʈ�� �Ф�)

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent *SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent *Camera;

	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent *HPBarWidget; //UWidgetComponent = ���Ϳ� UI ������ ������ �� �ֵ��� �𸮾󿡼� �����ϴ� Ŭ����
	//������ �̰��� ����Ϸ��� ArenaBattle.Build.cs ���Ͽ��� PublicDependencyModuleNames.AddRange()���� "UMG"�� �߰��ؾ� �Ѵ�

private:
	void UpDown(float NewAxisValue);
	void LeftRight(float NewAxisValue);
	void LookUp(float NewAxisValue);
	void Turn(float NewAxisValue);

	void ViewChange(); //Shift+V�� �Ҵ��س���
	void Attack(); //���콺 ���ʹ�ư

	UFUNCTION() //�������Ʈ�� ���õ� �Լ��� UFUNCTION ��ũ�θ� �����ؾ� �Ѵ�
	void OnAttackMontageEnded(UAnimMontage *Montage, bool bInterrupted); //��������Ʈ �Լ�

	void AttackStartComboState(); //������ ���۵ɶ� ȣ���� �Լ�
	void AttackEndComboState(); //������ ����ɶ� ȣ���� �Լ�

	void AttackCheck(); //���� ���� üũ �Լ�

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsAttacking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool CanNextCombo; //���� �޺��� �̵��� �� �ִ��� ����

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsComboInputOn; //�޺� �Է������� ����

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentCombo; //���� �������� �޺� ī��Ʈ

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 MaxCombo; //ĳ���Ͱ� ����� �� �ִ� �ִ� �޺� ī��Ʈ

	UPROPERTY()
	class UABAnimInstance *ABAnim; //ABCharacter Ŭ�������� �ִϸ��̼� �ν��Ͻ��� ���� ����� �����̹Ƿ� ��� ������ �����صд�

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRange;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRadius;


};
