// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h" //CoreMinimal�� �ּ����� �𸮾� ������Ʈ ����
//#include "EngineMinimal.h" //EngineMinimal�� �� ���� ���� ���
#include "ArenaBattle.h" //EngineMinimal.h �� ���⿡ ���Խ��״�
#include "GameFramework/Actor.h"
#include "Fountain.generated.h"

UCLASS()
class ARENABATTLE_API AFountain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFountain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//Unity�� OnDestroy()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//��� ������Ʈ�� ������ ������ ȣ��
	virtual void PostInitializeComponents() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//���Ͱ� �� ���� StaticMesh Component�� �������� ��������� �� ���� UStaticMeshComponent Ŭ������ �����͸� �����ؾ� �Ѵ�

	UPROPERTY(VisibleAnywhere) //UPROPERTY = �������� �����Ҵ� ������ �𸮾� �������� �ڵ����� �����ϰ� ���ִ� ��ũ��
	UStaticMeshComponent *Body; //�м��� �������� ���־�, �浹���

	UPROPERTY(VisibleAnywhere) //VisibleAnywhere = ������Ʈ�� �Ӽ��� �����쿡�� ������ �� �ְ� ���ִ� Ű����
	UStaticMeshComponent *Water; //���� ���־� ���

	UPROPERTY(VisibleAnywhere)
	UPointLightComponent *Light; //�м����� ���� ���

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent *Splash; //���� �����Ÿ��� ����Ʈ ���

	UPROPERTY(EditAnywhere, Category=ID) //���� ������ ��� ������ UPROPERTY ��ũ�θ� ������ ��� 0�� �ʱⰪ�� �����ȴ�. �����͸� �����ϰ� �ʹٸ� EditAnywhere ���
	int32 ID;

private:
	UPROPERTY(EditAnywhere, Category=Stat, Meta = (AllowPrivateAccess = true)) //private ������ �����Ϳ��� ������ �� ���� (SerializeField)
	float RotateSpeed;
};
