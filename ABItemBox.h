// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Actor.h"
#include "ABItemBox.generated.h"

UCLASS()
class ARENABATTLE_API AABItemBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABItemBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

public:	
	UPROPERTY(VisibleAnywhere, Category = Box)
	UBoxComponent *Trigger;

	UPROPERTY(VisibleAnywhere, Category = Box)
	UStaticMeshComponent *Box;

	UPROPERTY(VisibleAnywhere, Category = Effect)
	UParticleSystemComponent *Effect;

	UPROPERTY(EditInstanceOnly, Category = Box)
	TSubclassOf<class AABWeapon> WeaponItemClass; //AABWeapon 클래스 정보 저장

private:
	//OnComponentBeginOverlap은 박스 컴포넌트에 선언되어 있는 델리게이트로, Overlap 이벤트가 발생할 때마다 바인딩한 멤버 함수가 호출된다
	//바인딩할 멤버 함수 선언
	UFUNCTION()
	void OnCharacterOverlap(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	//OnSystemFinished는 파티클 컴포넌트에 선언되어 있는 델리게이트로, 이펙트가 끝날때 호출된다
	//바인딩할 멤버 함수 선언
	UFUNCTION()
	void OnEffectFinished(class UParticleSystemComponent *PSystem);

};
