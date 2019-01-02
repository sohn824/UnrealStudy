// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "ABPawn.generated.h"

UCLASS()
class ARENABATTLE_API AABPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AABPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController *NewController) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category = Collision)
	UCapsuleComponent *Capsule; //Capsule = 폰의 움직임을 담당하는 충돌 컴포넌트 (폰을 대표)

	UPROPERTY(VisibleAnywhere, Category = Visual)
	USkeletalMeshComponent *Mesh; //Skeletal Mesh = 캐릭터 에셋을 보여주고 애니메이션을 보여주는 역할

	UPROPERTY(VisibleAnywhere, Category = Movement)
	UFloatingPawnMovement *Movement; //Floating Pawn Movement = 플레이어의 입력에 따라 캐릭터가 움직이도록 설정해주는 컴포넌트

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent *SpringArm; //Spring Arm = 3인칭 시점으로 카메라 구도를 편리하게 설정할 수 있는 컴포넌트

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent *Camera; //Camera = 폰에 카메라 컴포넌트를 부착하면 카메라가 바라보는 게임 세계를 플레이어에게 전송

private:
	void UpDown(float NewAxisValue);
	void LeftRight(float NewAxisValue);

};
