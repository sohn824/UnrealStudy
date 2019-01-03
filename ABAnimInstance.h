// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Animation/AnimInstance.h"
#include "ABAnimInstance.generated.h"

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

private:
	//이 float변수를 블루프린트 Anim Graph에서 사용할 수 있도록 UPROPERTY 매크로 추가
	//BlueprintReadOnly = 블루프린트에서 값을 읽기만 함
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float CurrentPawnSpeed; //현재 폰의 속도값을 이용해 걷기, 뛰기를 결정할 것

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsInAir; //점프 애니메이션 재생을 위해 현재 공중에 있는지 체크할 변수
};
