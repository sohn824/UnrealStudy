// Fill out your copyright notice in the Description page of Project Settings.

#include "ABPawn.h"

// Sets default values
AABPawn::AABPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CAPSULE"));
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MESH"));
	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MOVEMENT"));
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	//부모 지정
	RootComponent = Capsule;
	Mesh->SetupAttachment(Capsule);
	SpringArm->SetupAttachment(Capsule);
	Camera->SetupAttachment(SpringArm);

	//디폴트 값 설정
	Capsule->SetCapsuleHalfHeight(88.0f);
	Capsule->SetCapsuleRadius(34.0f);
	Capsule->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

	///////////////////////////컴포넌트 생성 후 에셋 지정하기//////////////////////////////////////

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>
		SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));

	if (SK_CARDBOARD.Succeeded())
	{
		Mesh->SetSkeletalMesh(SK_CARDBOARD.Object);
	}

	///////////////////////애니메이션 블루프린트를 스켈레탈 메시에 연결하기//////////////////////////
	Mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance>
		WARRIOR_ANIM(TEXT("/Game/Animations/WarriorAnimBlueprint.WarriorAnimBlueprint_C")); //에셋의 클래스 정보
	if (WARRIOR_ANIM.Succeeded())
	{
		Mesh->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}
}

// Called when the game starts or when spawned
void AABPawn::BeginPlay()
{
	Super::BeginPlay();

	//캐릭터에 달리기 애니메이션 적용 (직접 코드로 하는 방식, 나중에 프로젝트가 커지면 이 방법은 비효율적이므로 블루프린트 사용)
	/*
	Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	UAnimationAsset *AnimAsset = LoadObject<UAnimationAsset>(nullptr, TEXT("/Game/Animations/WarriorRun.WarriorRun"));
	if (AnimAsset != nullptr)
	{
		Mesh->PlayAnimation(AnimAsset, true);
	}
	*/
}

// Called every frame
void AABPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AABPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AABPawn::PossessedBy(AController *NewController) //폰이 컨트롤러에 의해 빙의 당한다
{
	Super::PossessedBy(NewController);
}

// Called to bind functionality to input
// 플레이어 입력 매핑
void AABPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AABPawn::UpDown); //프로젝트 세팅->입력에서 설정한 키 매핑
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AABPawn::LeftRight);

}

void AABPawn::UpDown(float NewAxisValue)
{
	AddMovementInput(GetActorForwardVector(), NewAxisValue); //액터의 전진 방향 벡터를 가져온 후, axis값 (-1,1)을 전달한다
}

void AABPawn::LeftRight(float NewAxisValue)
{
	AddMovementInput(GetActorRightVector(), NewAxisValue); //액터의 우 방향 벡터를 가져온 후, axis값 (-1,1)을 전달한다
}

