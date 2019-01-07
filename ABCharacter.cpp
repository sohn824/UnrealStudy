// Fill out your copyright notice in the Description page of Project Settings.

#include "ABCharacter.h"
#include "ABAnimInstance.h"
#include "ABWeapon.h"
#include "ABCharacterStatComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/WidgetComponent.h" //UI�� WidgetComponent ����� ����
#include "ABCharacterWidget.h"
#include "ABAIController.h"

// Sets default values
AABCharacter::AABCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	CharacterStat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("CHARACTERSTAT"));
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));
		
	SpringArm->SetupAttachment(GetCapsuleComponent()); //Spring Arm�� ������ ĸ�� ������Ʈ �ؿ� ���δ�
	Camera->SetupAttachment(SpringArm);
	HPBarWidget->SetupAttachment(GetMesh());

	//Mesh�� �⺻ Location, Rotation ����
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

	//SpringArm Lerp ��
	ArmLengthSpeed = 3.0f;
	ArmRotationSpeed = 10.0f;

	//Combo ���� ��
	MaxCombo = 4;
	AttackEndComboState(); //�޺� ������ �� �� �ʱ�ȭ �Լ��ε� �����ڿ����� �ʱ�ȭ �� �뵵�� ȣ��

	IsAttacking = false; //���� ���������� üũ�� ����

	//���� ���� Ž�� ���� ��
	AttackRange = 200.0f;
	AttackRadius = 50.0f;

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

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCharacter")); //ĸ�� ������Ʈ�� ���� ������ ABCharacter �������� ����ϵ��� ����

	HPBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen); //UI ������ �׻� �÷��̾ ���� ������ �����Ѵ�
	static ConstructorHelpers::FClassFinder<UUserWidget>
		UI_HUD(TEXT("/Game/UI/UI_HPBar.UI_HPBar_C")); //Ŭ���� ������ �������� ���� �ڿ� _C
	if (UI_HUD.Succeeded())
	{
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		HPBarWidget->SetDrawSize(FVector2D(150.0f, 50.0f)); //�������Ʈ �۾��������� �����ߴ� ũ��� ����
	}

	AIControllerClass = AABAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; //�÷��̾ ������ ABCharacter���� AIController�� ���踦 �޴´�
}

// Called when the game starts or when spawned
void AABCharacter::BeginPlay()
{
	Super::BeginPlay();
}

bool AABCharacter::CanSetWeapon()
{
	if (CurrentWeapon == nullptr)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AABCharacter::SetWeapon(AABWeapon *NewWeapon) //�Ű������� ���� ���⸦ ĳ���Ϳ��� ���������ִ� �Լ�
{
	if (NewWeapon == nullptr || CurrentWeapon != nullptr)
	{
		return;
	}
	FName WeaponSocket(TEXT("hand_rSocket")); //����(�𸮾� �����Ϳ��� �����ϴ� ���) : ���⸦ ������ �� �ִ� ����
	if (NewWeapon != nullptr)
	{
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		NewWeapon->SetOwner(this);
		CurrentWeapon = NewWeapon;
	}
}

void AABCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABAnim = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
	if (ABAnim == nullptr)
	{
		UE_LOG(LogClass, Warning, TEXT("AnimInstance Error"));
		return;
	}
	
	//Anim Instance�� OnMontageEnded ��������Ʈ�� ABCharacter���� ������ OnAttackMontageEnded�� ������, ��������Ʈ�� �ߵ��� ������ �ִϸ��̼� �ý��ۿ��� ��Ÿ�� ����� ���� ���ϵ��� ���´�
	//(OnMontageEnded��� ��������Ʈ�� AnimInstance���� �����Ǵ� �ִϸ��̼� ��Ÿ�� ����� ������ �ߵ��Ǵ� ��������Ʈ�̴�)
	ABAnim->OnMontageEnded.AddDynamic(this, &AABCharacter::OnAttackMontageEnded);

	ABAnim->OnNextAttackCheck.AddLambda([this]()->void{
		ABLOG(Warning, TEXT("OnNextAttackCheck"));
		CanNextCombo = false;

		if (IsComboInputOn) //���콺 Ŭ���� ������ ���� �޺� ��Ÿ�� ���� ����
		{
			AttackStartComboState();
			ABAnim->JumpToAttackMontageSection(CurrentCombo);
		}
	}
	);

	ABAnim->OnAttackHitCheck.AddUObject(this, &AABCharacter::AttackCheck); //��������Ʈ�� ����

	//HP�� 0�� ���� �� ȣ��� OnHPIsZero ��������Ʈ�� ���ٽ��� �����Ѵ�
	CharacterStat->OnHPIsZero.AddLambda([this]() -> void{
		ABLOG(Warning, TEXT("OnHPIsZero"));
		ABAnim->SetDeadAnim();
		SetActorEnableCollision(false);
	});

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
	case EControlMode::NPC:
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true; //�̵� ���⿡ ���� �ڿ������� ȸ���ϰ� �ȴ�
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 480.0f, 0.0f);
		break;
	}
}

void AABCharacter::PossessedBy(AController *NewController)
{
	Super::PossessedBy(NewController);
	if (IsPlayerControlled()) //�÷��̾��� ĳ������ ���
	{
		SetControlMode(EControlMode::DIABLO);
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}
	else //�÷��̾��� ĳ���Ͱ� �ƴ� ��� (NPC�� ���)
	{
		SetControlMode(EControlMode::NPC);
		GetCharacterMovement()->MaxWalkSpeed = 300.0f; //�÷��̾�� �ӵ��� ������ �����Ѵ�
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
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AABCharacter::Attack); //������ ���� �����ѰŴϱ� AABCharacter ���

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

void AABCharacter::Attack()
{
	if (IsAttacking) //�̹� ������ �ϰ������� �߰��� ���콺�� ���� ���
	{
		//ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo)); //CurrentCombo�� 1~MaxCombo ���̿� �ִ��� üũ
		if (CanNextCombo) //���� �޺��� �Ѿ �� �ִ� ��Ȳ�̸� IsComboInputOn�� true�� �ٲ㼭 ���� �������� �Ѿ
		{
			IsComboInputOn = true;
		}
		return;
	}
	else //�� ���ǿ��� ó�� ���콺�� ���� ���
	{
		ABCHECK(CurrentCombo == 0); //�������� �ƴϰ� CurrentCombo�� 0�� �ƴϸ� ����
		AttackStartComboState(); //CurrentCombo�� 0�ϰ�� �޺� ����

		//AnimInstance���� ��Ÿ�ָ� ������Ű�� ���� Attack()�Լ� ������
		//ĳ������ AnimInstance�� �����ͼ� AnimInstance�� ���� ������ PlayAttackMontage() �Լ��� ȣ����
		ABAnim->PlayAttackMontage();
		ABAnim->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}


}

//�޺� ���� �κ�... �ȵ�

void AABCharacter::OnAttackMontageEnded(UAnimMontage *Montage, bool bInterrupted)
{
	ABCHECK(IsAttacking);
	ABCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();
}

void AABCharacter::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;
	//CurrentCombo�� 0�� MaxCombo-1 ���̿� �ִ��� üũ�Ѵ�
	ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo); //Clamp�� ����Ͽ� CurrentCombo+1�� (1, MaxCombo)���̷� ���ѽ�Ų��
}

void AABCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

void AABCharacter::AttackCheck()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this); //������ Ž�� ��Ͽ� ���� �߰�
	bool bResult = GetWorld()->SweepSingleByChannel
	(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * 200.0f, //������ Ž������, ���� �ü��������� 200cm
		FQuat::Identity, //ȸ���� �⺻
		ECollisionChannel::ECC_GameTraceChannel2, //���� ������ Attack �������� ECC_GameTraceChannel 2���� �������� (Config ������ DefaultEngine.ini���� Ȯ�� ����)
		FCollisionShape::MakeSphere(50.0f), //Ž���� ����� 50cm �������� ��ü
		Params //Ž������ ������ ��ü
	);


#if ENABLE_DRAW_DEBUG

	FVector TraceVec = GetActorForwardVector() * AttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 5.0f;

	DrawDebugCapsule
	(
		GetWorld(),
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime
	);
#endif

	if (bResult)
	{
		if (HitResult.Actor.IsValid())
		{
			ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.Actor->GetName()); //�α�â���� Ȯ��
			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(CharacterStat->GetAttack(), DamageEvent, GetController(), this); //������ ���Ϳ� �������� �����ߴ� -> todo: �������� ���Ϳ� ���� ���� �ۼ�
			//������ ��, ������ �̺�Ʈ, ������ ��Ʈ�ѷ�, ������ ����
		}
	}
	
}

float AABCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& 
	DamageEvent, class AController *EventInstigator, AActor *DamageCauser)
{
	//������ ��, ������ �̺�Ʈ, ������ ��Ʈ�ѷ�, ������ ����
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	ABLOG(Warning, TEXT("Actor %s took Damage : %f"), *GetName(), FinalDamage);
	CharacterStat->SetDamage(FinalDamage); //ĳ���� ������ HP���� FinalDamage��ŭ �������� ������ �Լ�
	return FinalDamage;
}