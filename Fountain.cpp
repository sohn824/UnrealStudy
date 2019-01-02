// Fill out your copyright notice in the Description page of Project Settings.

#include "Fountain.h"

// Sets default values
AFountain::AFountain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//CreateDefaultSubobject : 컴포넌트를 생성한다
	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY"));
	Water = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WATER"));
	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("LIGHT"));
	Splash = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SPLASH"));
	//TEXT 문자열 값은 액터에 속한 컴포넌트를 구별하기 위한 Hash 값이다. (중복되면 안됨)

	RootComponent = Body; //액터를 대표할 루트 컴포넌트를 Body로 지정
	Water->SetupAttachment(Body); //Water를 Body의 자식으로 지정
	Light->SetupAttachment(Body); //...
	Splash->SetupAttachment(Body);

	Water->SetRelativeLocation(FVector(0.0f, 0.0f, 135.0f)); //컴포넌트의 기본 위치 값 설정
	Light->SetRelativeLocation(FVector(0.0f, 0.0f, 195.0f));
	Splash->SetRelativeLocation(FVector(0.0f, 0.0f, 195.0f));

	///////////////////////////컴포넌트 생성 후 에셋 지정하기//////////////////////////////////////

	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		SM_BODY(TEXT("/Game/InfinityBladeGrassLands/Environments/Plains/Env_Plains_Ruins/StaticMesh/SM_Plains_Castle_Fountain_01.SM_Plains_Castle_Fountain_01"));
	//생성자에서 에셋을 불러오려면 ConstructorHelpers 클래스의 FObjectFinder<오브젝트타입>[변수명]("레퍼런스 경로명")을 사용한다.
	//이후 변수명(여기선 SM_BODY)의 Object 멤버값을 사용하면 에셋의 포인터를 가져올 수 있다.
	//경로는 게임 실행 중 변경될 일이 없기 때문에 static으로 한번만 초기화하도록 했다.

	if (SM_BODY.Succeeded())
	{
		Body->SetStaticMesh(SM_BODY.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		SM_WATER(TEXT("/Game/InfinityBladeGrassLands/Environments/Plains/Env_Plains_Ruins/StaticMesh/SM_Plains_Castle_Fountain_02.SM_Plains_Castle_Fountain_02"));

	if (SM_WATER.Succeeded())
	{
		Water->SetStaticMesh(SM_WATER.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem>
		PS_SPLASH(TEXT("/Game/InfinityBladeGrassLands/Effects/FX_Ambient/Water/P_Water_Fountain_Splash_Base_01.P_Water_Fountain_Splash_Base_01"));

	if (PS_SPLASH.Succeeded())
	{
		Splash->SetTemplate(PS_SPLASH.Object);
	}

	RotateSpeed = 30.0f;
}

// Called when the game starts or when spawned
void AFountain::BeginPlay() //Unity의 Start() 함수
{
	Super::BeginPlay();


	//GetName()앞에 포인터 변수가 붙은 이유는 FString 변수에서 문자열 정보를 얻어오려면 필요하기 때문
	UE_LOG(ArenaBattle, Warning, TEXT("Actor name : %s, ID : %d, Location X : %.3f"), *GetName(), ID, GetActorLocation().X);
	//ABLOG_S(Warning);
	//ABLOG(Warning, TEXT("Actor Name : %s, ID : %d, Location X : %.3f"), *GetName(), ID, GetActorLocation().X);
	//이거 왜 안됨?
	//경로 문제인거 같은데 확인 요망
}

// Called every frame
void AFountain::Tick(float DeltaTime) //Unity의 Update() 함수
{
	Super::Tick(DeltaTime);
	AddActorLocalRotation(FRotator(0.0f, RotateSpeed * DeltaTime, 0.0f)); //Tick마다 회전
}

//Unity의 OnDestroy() 소멸될 때 호출
void AFountain::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

//모든 컴포넌트 세팅이 완료된 후 호출됨
//PostInitializeComponents() -> BeginPlay() -> EndPlay()
void AFountain::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

