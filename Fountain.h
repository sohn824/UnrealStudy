// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h" //CoreMinimal은 최소한의 언리얼 오브젝트 동작
//#include "EngineMinimal.h" //EngineMinimal은 더 많은 엔진 기능
#include "ArenaBattle.h" //EngineMinimal.h 는 여기에 포함시켰다
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
	//Unity의 OnDestroy()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//모든 컴포넌트의 세팅이 끝나면 호출
	virtual void PostInitializeComponents() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//액터가 두 개의 StaticMesh Component를 가지려면 멤버변수로 두 개의 UStaticMeshComponent 클래스의 포인터를 선언해야 한다

	UPROPERTY(VisibleAnywhere) //UPROPERTY = 포인터의 동적할당 해제를 언리얼 엔진에서 자동으로 관리하게 해주는 매크로
	UStaticMeshComponent *Body; //분수대 구조물의 비주얼, 충돌담당

	UPROPERTY(VisibleAnywhere) //VisibleAnywhere = 컴포넌트의 속성을 윈도우에서 편집할 수 있게 해주는 키워드
	UStaticMeshComponent *Water; //물의 비주얼 담당

	UPROPERTY(VisibleAnywhere)
	UPointLightComponent *Light; //분수대의 조명 담당

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent *Splash; //물의 찰랑거리는 이펙트 담당

	UPROPERTY(EditAnywhere, Category=ID) //정수 유형의 멤버 변수에 UPROPERTY 매크로를 설정할 경우 0의 초기값이 설정된다. 데이터를 변경하고 싶다면 EditAnywhere 사용
	int32 ID;

private:
	UPROPERTY(EditAnywhere, Category=Stat, Meta = (AllowPrivateAccess = true)) //private 변수를 에디터에서 편집할 수 있음 (SerializeField)
	float RotateSpeed;
};
