// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "ArenaBattle.h"
#include "Blueprint/UserWidget.h"
#include "ABCharacterWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABCharacterWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void BindCharacterStat(class UABCharacterStatComponent *NewCharacterStat); //캐릭터 스탯과 매핑하는 함수


private:
	TWeakObjectPtr<class UABCharacterStatComponent> CurrentCharacterStat; //TWeakObjectPtr = 약포인터 (무엇에 의해서건 자신의 오브젝트가 소멸되면 자동으로 비워지는 포인터)
																		  //UI와 캐릭터가 다른 액터라면 사용하는 것이 좋음 (지금은 같은 액터이기 때문에 그냥 포인터를 써도 되지만 학습을 위해서)
};
