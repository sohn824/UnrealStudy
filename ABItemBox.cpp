// Fill out your copyright notice in the Description page of Project Settings.

#include "ABItemBox.h"
#include "ABWeapon.h"
#include "ABCharacter.h"

// Sets default values
AABItemBox::AABItemBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Box = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BOX"));
	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EFFECT"));

	RootComponent = Trigger;
	Box->SetupAttachment(RootComponent);
	Effect->SetupAttachment(RootComponent);

	Trigger->SetBoxExtent(FVector(40.0f, 42.0f, 30.0f)); //Extent ���� ��ü �ڽ� ���� ũ���� ���� ���� �ǹ��Ѵ�. �ش� �ڽ��� ����ƽ�޽� �����͸� ����� �� �� ����
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		SM_BOX(TEXT("/Game/InfinityBladeGrassLands/Environments/Breakables/StaticMesh/Box/SM_Env_Breakables_Box1.SM_Env_Breakables_Box1"));
	if (SM_BOX.Succeeded())
	{
		Box->SetStaticMesh(SM_BOX.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem>
		P_CHESTOPEN(TEXT("/Game/InfinityBladeGrassLands/Effects/FX_Treasure/Chest/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh"));
	if (P_CHESTOPEN.Succeeded())
	{
		Effect->SetTemplate(P_CHESTOPEN.Object);
		Effect->bAutoActivate = false;
	}

	Box->SetRelativeLocation(FVector(0.0f, -3.5f, -30.0f));

	Trigger->SetCollisionProfileName(TEXT("ItemBox"));
	Box->SetCollisionProfileName(TEXT("NoCollision"));

	WeaponItemClass = AABWeapon::StaticClass(); //Ŭ���� ���� ����
}

// Called when the game starts or when spawned
void AABItemBox::BeginPlay()
{
	Super::BeginPlay();
	
}

void AABItemBox::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//OnComponentBeginOverlap�� �ڽ� ������Ʈ�� ����Ǿ� �ִ� ��������Ʈ��, Overlap �̺�Ʈ�� �߻��� ������ ���ε��� ��� �Լ��� ȣ��ȴ�
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AABItemBox::OnCharacterOverlap); //��� �Լ� ���ε�
}

void AABItemBox::OnCharacterOverlap(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	auto ABCharacter = Cast<AABCharacter>(OtherActor);
	if (ABCharacter == nullptr)
	{
		return;
	}

	if (ABCharacter != nullptr && WeaponItemClass != nullptr)
	{
		if (ABCharacter->CanSetWeapon()) //���� ������ ���¶��
		{
			auto NewWeapon = GetWorld()->SpawnActor<AABWeapon>(WeaponItemClass, FVector::ZeroVector, FRotator::ZeroRotator); //�����ص� WeaponŬ������ ����
			ABCharacter->SetWeapon(NewWeapon); //ĳ���Ϳ��� ���⸦ ������Ų��
			Effect->Activate(true); //���� ���� ����Ʈ �߻�
			Box->SetHiddenInGame(true, true); //���ڸ� ���� ȭ�鿡�� ����
			SetActorEnableCollision(false); //Collision Off
			Effect->OnSystemFinished.AddDynamic(this, &AABItemBox::OnEffectFinished); //��������Ʈ�� ����Լ� ���ε�
		}
		else
		{
			ABLOG(Warning, TEXT("%s can't equip weapon currently."), *ABCharacter->GetName());
		}
	}
}

void AABItemBox::OnEffectFinished(UParticleSystemComponent *PSystem)
{
	Destroy(); //����Ʈ ����� ������ ��ƼŬ �ý��� �ı�
}


