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
	void JumpToAttackMontageSection(int32 NewSection); //다음 섹션으로 넘어가는 함수

	FOnNextAttackCheckDelegate OnNextAttackCheck; //NextAttackCheck 노티파이가 발생할 때마다 ABCharacter에 이를 전달할 델리게이트
	FOnAttackHitCheckDelegate OnAttackHitCheck; //AttackHitCheck 노티파이가 발생할 때마다 ABCharacter에 이를 전달할 델리게이트
	void SetDeadAnim() { IsDead = true; }

private:
	UFUNCTION() //언리얼 에디터가 해당 함수를 찾을 수 있도록 하는 매크로
	void AnimNotify_AttackHitCheck(); //애니메이션 노티파이 감지용 함수 (AttackHitCheck)

	UFUNCTION()
	void AnimNotify_NextAttackCheck(); //역시 노티파이 감지용 함수 (NextAttackCheck)

	FName GetAttackMontageSectionName(int32 Section); //몽타주 섹션 이름 반환 함수

private:
	//이 float변수를 블루프린트 Anim Graph에서 사용할 수 있도록 UPROPERTY 매크로 추가
	//BlueprintReadOnly = 블루프린트에서 값을 읽기만 함
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float CurrentPawnSpeed; //현재 폰의 속도값을 이용해 걷기, 뛰기를 결정할 것

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsInAir; //점프 애니메이션 재생을 위해 현재 공중에 있는지 체크할 변수

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage *AttackMontage; //Combo Attack Anim 몽타주

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsDead;
};
