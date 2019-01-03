// Fill out your copyright notice in the Description page of Project Settings.

#include "ABCharacter.h"

// Sets default values
AABCharacter::AABCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	SpringArm->SetupAttachment(GetCapsuleComponent()); //Spring Arm�� ������ ĸ�� ������Ʈ �ؿ� ���δ�
	Camera->SetupAttachment(SpringArm);

	//Mesh�� �⺻ Location, Rotation ����
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

	//SpringArm Lerp ��
	ArmLengthSpeed = 3.0f;
	ArmRotationSpeed = 10.0f;

	//Mesh�� SkeletalMeseh�� ���� ����
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>
		SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));

	if (SK_CARDBOARD.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);
	}

	//�ִϸ��̼� �������Ʈ ����
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance>
		WARRIOR_ANIM(TEXT("/Game/Animations/WarriorAnimBlueprint.WarriorAnimBlueprint_C"));

	if (WARRIOR_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}

	//SetControlMode(EControlMode::GTA); //GTA ���۸��
	SetControlMode(EControlMode::DIABLO); //��ƺ�� ���۸��

	GetCharacterMovement()->JumpZVelocity = 800.0f; //���� ���� ����
}

// Called when the game starts or when spawned
void AABCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AABCharacter::SetControlMode(EControlMode NewControlMode)
{
	CurrentControlMode = NewControlMode;

	switch (CurrentControlMode) //GTA ����� 3��Ī ���� ���
	{
	case EControlMode::GTA:
		//SpringArm���� 3��Ī ������ ī�޶� ������ ������ �� ���ϰ� ����� �� �ֵ��� ���� �Ӽ����� ����� �ִ�
		//SpringArm->TargetArmLength = 450.0f; //ī�޶� ����
		//SpringArm->SetRelativeRotation(FRotator::ZeroRotator); //ī�޶� ȸ��
		ArmLengthTo = 450.0f;
		SpringArm->bUsePawnControlRotation = true; //���� ��, ��Ʈ�� ȸ�� ���
		SpringArm->bInheritPitch = true; //pitch�� ���
		SpringArm->bInheritRoll = true; //roll�� ���
		SpringArm->bInheritYaw = true; //yaw�� ���
		SpringArm->bDoCollisionTest = true; 
		bUseControllerRotationYaw = false;
		//CharacterMovement ������Ʈ�� OrientRotationToMovement = ĳ���Ͱ� �����̴� �������� ĳ���͸� �ڵ����� ȸ������ �ش�
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
		break;
	case EControlMode::DIABLO:
		//SpringArm->TargetArmLength = 800.0f;
		//SpringArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
		ArmLengthTo = 800.0f;
		ArmRotationTo = FRotator(-45.0f, 0.0f, 0.0f);
		//�÷��̾ �Է��� �������� ȸ���Ҽ� �ֵ��� ����
		SpringArm->bUsePawnControlRotation = false;
		SpringArm->bInheritPitch = false;
		SpringArm->bInheritRoll = false;
		SpringArm->bInheritYaw = false;
		SpringArm->bDoCollisionTest = false;
		bUseControllerRotationYaw = false; //ĳ���Ͱ� 45�� ������ �������� ȸ���ϱ� ������ �ڿ������� ȸ���ϱ� ���� ������� �ʴ´�
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true; //��� �̰��� ����ϸ� �ڿ������� ȸ���Ѵ�
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
		break;
	}
}

// Called every frame
void AABCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//InterpTo�� (float, vector, rotator) ������ �����ϸ�, Unity�� Vector3.Lerp()�� ����� ���� ����� �����Ѵ�
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
			//�÷��̾ �Է��� �������� ȸ���ϵ��� ��
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

	//Ű �Է� ����

	PlayerInputComponent->BindAction(TEXT("ViewChange"), EInputEvent::IE_Pressed, this, &AABCharacter::ViewChange);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump); //������ �����Ϸ��� ACharacter Ŭ������ �����Ǿ��ִ� Jump �Լ� �����ϸ� ��

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AABCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AABCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AABCharacter::LookUp); //ĳ���� Y�� ȸ��
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AABCharacter::Turn); //ĳ���� Z�� ȸ��

}

void AABCharacter::UpDown(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		//ȸ�� �����κ��� �ü� ���� ���� ���� �����´�
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), NewAxisValue);
		break;
	case EControlMode::DIABLO:
		DirectionToMove.X = NewAxisValue; //DirectionToMove�� X�� ����
		break;
	}

}

void AABCharacter::LeftRight(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		//ȸ�� �����κ��� ���� ���� ���� ���� �����´�
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), NewAxisValue);
		break;
	case EControlMode::DIABLO:
		DirectionToMove.Y = NewAxisValue; //DirectionToMove�� Y�� ����
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
	//DIABLO ���� ����� ���� Pitch, Yaw���� �ǵ帱 �ʿ䰡 ����
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
		GetController()->SetControlRotation(GetActorRotation()); //�� ��带 ��ȯ�� �� ī�޶� �ڿ������� �̵� (Lerp)
		SetControlMode(EControlMode::DIABLO);
		break;
	case EControlMode::DIABLO:
		GetController()->SetControlRotation(SpringArm->RelativeRotation);
		SetControlMode(EControlMode::GTA);
		break;
	}
}


