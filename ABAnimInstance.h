// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Animation/AnimInstance.h"
#include "ABAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate)
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate)

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UABAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void PlayAttackMontage();
	void JumpToAttackMontageSection(int32 NewSection); //���� �������� �Ѿ�� �Լ�

	FOnNextAttackCheckDelegate OnNextAttackCheck; //NextAttackCheck ��Ƽ���̰� �߻��� ������ ABCharacter�� �̸� ������ ��������Ʈ
	FOnAttackHitCheckDelegate OnAttackHitCheck; //AttackHitCheck ��Ƽ���̰� �߻��� ������ ABCharacter�� �̸� ������ ��������Ʈ
	void SetDeadAnim() { IsDead = true; }

private:
	UFUNCTION() //�𸮾� �����Ͱ� �ش� �Լ��� ã�� �� �ֵ��� �ϴ� ��ũ��
	void AnimNotify_AttackHitCheck(); //�ִϸ��̼� ��Ƽ���� ������ �Լ� (AttackHitCheck)

	UFUNCTION()
	void AnimNotify_NextAttackCheck(); //���� ��Ƽ���� ������ �Լ� (NextAttackCheck)

	FName GetAttackMontageSectionName(int32 Section); //��Ÿ�� ���� �̸� ��ȯ �Լ�

private:
	//�� float������ �������Ʈ Anim Graph���� ����� �� �ֵ��� UPROPERTY ��ũ�� �߰�
	//BlueprintReadOnly = �������Ʈ���� ���� �б⸸ ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float CurrentPawnSpeed; //���� ���� �ӵ����� �̿��� �ȱ�, �ٱ⸦ ������ ��

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsInAir; //���� �ִϸ��̼� ����� ���� ���� ���߿� �ִ��� üũ�� ����

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage *AttackMontage; //Combo Attack Anim ��Ÿ��

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsDead;
};
