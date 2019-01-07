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

	Trigger->SetBoxExtent(FVector(40.0f, 42.0f, 30.0f)); //Extent 값은 전체 박스 영역 크기의 절반 값을 의미한다. 해당 박스의 스태틱메시 에디터를 열어보면 알 수 있음
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

	WeaponItemClass = AABWeapon::StaticClass(); //클래스 정보 저장
}

// Called when the game starts or when spawned
void AABItemBox::BeginPlay()
{
	Super::BeginPlay();
	
}

void AABItemBox::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//OnComponentBeginOverlap은 박스 컴포넌트에 선언되어 있는 델리게이트로, Overlap 이벤트가 발생할 때마다 바인딩한 멤버 함수가 호출된다
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AABItemBox::OnCharacterOverlap); //멤버 함수 바인딩
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
		if (ABCharacter->CanSetWeapon()) //착용 가능한 상태라면
		{
			auto NewWeapon = GetWorld()->SpawnActor<AABWeapon>(WeaponItemClass, FVector::ZeroVector, FRotator::ZeroRotator); //저장해둔 Weapon클래스를 생성
			ABCharacter->SetWeapon(NewWeapon); //캐릭터에게 무기를 장착시킨다
			Effect->Activate(true); //상자 오픈 이펙트 발생
			Box->SetHiddenInGame(true, true); //상자를 게임 화면에서 숨김
			SetActorEnableCollision(false); //Collision Off
			Effect->OnSystemFinished.AddDynamic(this, &AABItemBox::OnEffectFinished); //델리게이트에 멤버함수 바인딩
		}
		else
		{
			ABLOG(Warning, TEXT("%s can't equip weapon currently."), *ABCharacter->GetName());
		}
	}
}

void AABItemBox::OnEffectFinished(UParticleSystemComponent *PSystem)
{
	Destroy(); //이펙트 재생이 끝나면 파티클 시스템 파괴
}


