// Fill out your copyright notice in the Description page of Project Settings.

#include "ABCharacter.h"

// Sets default values
AABCharacter::AABCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	SpringArm->SetupAttachment(GetCapsuleComponent()); //Spring Arm을 액터의 캡슐 컴포넌트 밑에 붙인다
	Camera->SetupAttachment(SpringArm);

	//Mesh의 기본 Location, Rotation 설정
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

	//SpringArm Lerp 값
	ArmLengthSpeed = 3.0f;
	ArmRotationSpeed = 10.0f;

	//Mesh의 SkeletalMeseh에 에셋 설정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>
		SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));

	if (SK_CARDBOARD.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);
	}

	//애니메이션 블루프린트 설정
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance>
		WARRIOR_ANIM(TEXT("/Game/Animations/WarriorAnimBlueprint.WarriorAnimBlueprint_C"));

	if (WARRIOR_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}

	//SetControlMode(EControlMode::GTA); //GTA 조작모드
	SetControlMode(EControlMode::DIABLO); //디아블로 조작모드

	GetCharacterMovement()->JumpZVelocity = 800.0f; //점프 높이 설정
}

// Called when the game starts or when spawned
void AABCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AABCharacter::SetControlMode(EControlMode NewControlMode)
{
	CurrentControlMode = NewControlMode;

	switch (CurrentControlMode) //GTA 방식의 3인칭 조작 모드
	{
	case EControlMode::GTA:
		//SpringArm에는 3인칭 시점의 카메라 설정을 구현할 때 편리하게 사용할 수 있도록 여러 속성들이 설계돼 있다
		//SpringArm->TargetArmLength = 450.0f; //카메라 길이
		//SpringArm->SetRelativeRotation(FRotator::ZeroRotator); //카메라 회전
		ArmLengthTo = 450.0f;
		SpringArm->bUsePawnControlRotation = true; //폰의 뷰, 컨트롤 회전 사용
		SpringArm->bInheritPitch = true; //pitch값 상속
		SpringArm->bInheritRoll = true; //roll값 상속
		SpringArm->bInheritYaw = true; //yaw값 상속
		SpringArm->bDoCollisionTest = true; 
		bUseControllerRotationYaw = false;
		//CharacterMovement 컴포넌트의 OrientRotationToMovement = 캐릭터가 움직이는 방향으로 캐릭터를 자동으로 회전시켜 준다
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
		break;
	case EControlMode::DIABLO:
		//SpringArm->TargetArmLength = 800.0f;
		//SpringArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
		ArmLengthTo = 800.0f;
		ArmRotationTo = FRotator(-45.0f, 0.0f, 0.0f);
		//플레이어가 입력한 방향으로 회전할수 있도록 세팅
		SpringArm->bUsePawnControlRotation = false;
		SpringArm->bInheritPitch = false;
		SpringArm->bInheritRoll = false;
		SpringArm->bInheritYaw = false;
		SpringArm->bDoCollisionTest = false;
		bUseControllerRotationYaw = false; //캐릭터가 45도 단위로 끊어져서 회전하기 때문에 자연스럽게 회전하기 위해 사용하지 않는다
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true; //대신 이것을 사용하면 자연스럽게 회전한다
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
		break;
	}
}

// Called every frame
void AABCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//InterpTo는 (float, vector, rotator) 형식을 제공하며, Unity의 Vector3.Lerp()와 비슷한 보간 기능을 제공한다
	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo, DeltaTime, ArmLengthSpeed);

	switch (CurrentControlMode)
	{
	case EControlMode::DIABLO:
		SpringArm->RelativeRotation = FMath::RInterpTo(SpringArm->RelativeRotation, ArmRotationTo, DeltaTime, ArmRotationSpeed);
		break;
	}


	switch (CurrentControlMode)
	{
	case EControlMode::DIABLO:
		if (DirectionToMove.SizeSquared() > 0.0f)
		{
			//플레이어가 입력한 방향으로 회전하도록 함
			GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
			AddMovementInput(DirectionToMove);
		}
		break;
	}

}

// Called to bind functionality to input
void AABCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//키 입력 매핑

	PlayerInputComponent->BindAction(TEXT("ViewChange"), EInputEvent::IE_Pressed, this, &AABCharacter::ViewChange);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump); //점프를 구현하려면 ACharacter 클래스에 구현되어있는 Jump 함수 연동하면 됨

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AABCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AABCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AABCharacter::LookUp); //캐릭터 Y축 회전
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AABCharacter::Turn); //캐릭터 Z축 회전

}

void AABCharacter::UpDown(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		//회전 값으로부터 시선 방향 벡터 값을 가져온다
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), NewAxisValue);
		break;
	case EControlMode::DIABLO:
		DirectionToMove.X = NewAxisValue; //DirectionToMove의 X값 갱신
		break;
	}

}

void AABCharacter::LeftRight(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		//회전 값으로부터 우측 방향 벡터 값을 가져온다
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), NewAxisValue);
		break;
	case EControlMode::DIABLO:
		DirectionToMove.Y = NewAxisValue; //DirectionToMove의 Y값 갱신
		break;
	}

}

void AABCharacter::LookUp(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		AddControllerPitchInput(NewAxisValue);
		break;
	}
	//DIABLO 조작 모드일 때는 Pitch, Yaw값을 건드릴 필요가 없다
}

void AABCharacter::Turn(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		AddControllerYawInput(NewAxisValue);
		break;
	}
}

void AABCharacter::ViewChange()
{
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		GetController()->SetControlRotation(GetActorRotation()); //뷰 모드를 전환할 때 카메라를 자연스럽게 이동 (Lerp)
		SetControlMode(EControlMode::DIABLO);
		break;
	case EControlMode::DIABLO:
		GetController()->SetControlRotation(SpringArm->RelativeRotation);
		SetControlMode(EControlMode::GTA);
		break;
	}
}


