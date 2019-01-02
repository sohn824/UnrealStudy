// Fill out your copyright notice in the Description page of Project Settings.

#include "Fountain.h"

// Sets default values
AFountain::AFountain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//CreateDefaultSubobject : ������Ʈ�� �����Ѵ�
	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY"));
	Water = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WATER"));
	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("LIGHT"));
	Splash = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SPLASH"));
	//TEXT ���ڿ� ���� ���Ϳ� ���� ������Ʈ�� �����ϱ� ���� Hash ���̴�. (�ߺ��Ǹ� �ȵ�)

	RootComponent = Body; //���͸� ��ǥ�� ��Ʈ ������Ʈ�� Body�� ����
	Water->SetupAttachment(Body); //Water�� Body�� �ڽ����� ����
	Light->SetupAttachment(Body); //...
	Splash->SetupAttachment(Body);

	Water->SetRelativeLocation(FVector(0.0f, 0.0f, 135.0f)); //������Ʈ�� �⺻ ��ġ �� ����
	Light->SetRelativeLocation(FVector(0.0f, 0.0f, 195.0f));
	Splash->SetRelativeLocation(FVector(0.0f, 0.0f, 195.0f));

	///////////////////////////������Ʈ ���� �� ���� �����ϱ�//////////////////////////////////////

	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		SM_BODY(TEXT("/Game/InfinityBladeGrassLands/Environments/Plains/Env_Plains_Ruins/StaticMesh/SM_Plains_Castle_Fountain_01.SM_Plains_Castle_Fountain_01"));
	//�����ڿ��� ������ �ҷ������� ConstructorHelpers Ŭ������ FObjectFinder<������ƮŸ��>[������]("���۷��� ��θ�")�� ����Ѵ�.
	//���� ������(���⼱ SM_BODY)�� Object ������� ����ϸ� ������ �����͸� ������ �� �ִ�.
	//��δ� ���� ���� �� ����� ���� ���� ������ static���� �ѹ��� �ʱ�ȭ�ϵ��� �ߴ�.

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
void AFountain::BeginPlay() //Unity�� Start() �Լ�
{
	Super::BeginPlay();


	//GetName()�տ� ������ ������ ���� ������ FString �������� ���ڿ� ������ �������� �ʿ��ϱ� ����
	UE_LOG(ArenaBattle, Warning, TEXT("Actor name : %s, ID : %d, Location X : %.3f"), *GetName(), ID, GetActorLocation().X);
	//ABLOG_S(Warning);
	//ABLOG(Warning, TEXT("Actor Name : %s, ID : %d, Location X : %.3f"), *GetName(), ID, GetActorLocation().X);
	//�̰� �� �ȵ�?
	//��� �����ΰ� ������ Ȯ�� ���
}

// Called every frame
void AFountain::Tick(float DeltaTime) //Unity�� Update() �Լ�
{
	Super::Tick(DeltaTime);
	AddActorLocalRotation(FRotator(0.0f, RotateSpeed * DeltaTime, 0.0f)); //Tick���� ȸ��
}

//Unity�� OnDestroy() �Ҹ�� �� ȣ��
void AFountain::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

//��� ������Ʈ ������ �Ϸ�� �� ȣ���
//PostInitializeComponents() -> BeginPlay() -> EndPlay()
void AFountain::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

