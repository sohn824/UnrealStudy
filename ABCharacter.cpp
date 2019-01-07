// Fill out your copyright notice in the Description page of Project Settings.

#include "ABCharacter.h"
#include "ABAnimInstance.h"
#include "ABWeapon.h"
#include "ABCharacterStatComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/WidgetComponent.h" //UI의 WidgetComponent 사용을 위해
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
		
	SpringArm->SetupAttachment(GetCapsuleComponent()); //Spring Arm을 액터의 캡슐 컴포넌트 밑에 붙인다
	Camera->SetupAttachment(SpringArm);
	HPBarWidget->SetupAttachment(GetMesh());

	//Mesh의 기본 Location, Rotation 설정
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

	//SpringArm Lerp 값
	ArmLengthSpeed = 3.0f;
	ArmRotationSpeed = 10.0f;

	//Combo 관련 값
	MaxCombo = 4;
	AttackEndComboState(); //콤보 끝났을 때 값 초기화 함수인데 생성자에서도 초기화 할 용도로 호출

	IsAttacking = false; //현재 공격중인지 체크할 변수

	//공격 판정 탐지 관련 값
	AttackRange = 200.0f;
	AttackRadius = 50.0f;

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

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCharacter")); //캡슐 컴포넌트가 내가 설정한 ABCharacter 프리셋을 사용하도록 설정

	HPBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen); //UI 위젯이 항상 플레이어를 향해 보도록 지정한다
	static ConstructorHelpers::FClassFinder<UUserWidget>
		UI_HUD(TEXT("/Game/UI/UI_HPBar.UI_HPBar_C")); //클래스 정보를 가져오기 위해 뒤에 _C
	if (UI_HUD.Succeeded())
	{
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		HPBarWidget->SetDrawSize(FVector2D(150.0f, 50.0f)); //블루프린트 작업공간에서 설정했던 크기와 동일
	}

	AIControllerClass = AABAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; //플레이어를 제외한 ABCharacter들은 AIController의 지배를 받는다
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

void AABCharacter::SetWeapon(AABWeapon *NewWeapon) //매개변수로 들어온 무기를 캐릭터에게 장착시켜주는 함수
{
	if (NewWeapon == nullptr || CurrentWeapon != nullptr)
	{
		return;
	}
	FName WeaponSocket(TEXT("hand_rSocket")); //소켓(언리얼 에디터에서 제공하는 기능) : 무기를 장착할 수 있는 소켓
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
	
	//Anim Instance의 OnMontageEnded 델리게이트와 ABCharacter에서 선언한 OnAttackMontageEnded를 연결해, 델리게이트가 발동할 때까지 애니메이션 시스템에서 몽타주 재생을 하지 못하도록 막는다
	//(OnMontageEnded라는 델리게이트는 AnimInstance에서 제공되는 애니메이션 몽타주 재생이 끝나면 발동되는 델리게이트이다)
	ABAnim->OnMontageEnded.AddDynamic(this, &AABCharacter::OnAttackMontageEnded);

	ABAnim->OnNextAttackCheck.AddLambda([this]()->void{
		ABLOG(Warning, TEXT("OnNextAttackCheck"));
		CanNextCombo = false;

		if (IsComboInputOn) //마우스 클릭이 들어오면 현재 콤보 몽타주 섹션 실행
		{
			AttackStartComboState();
			ABAnim->JumpToAttackMontageSection(CurrentCombo);
		}
	}
	);

	ABAnim->OnAttackHitCheck.AddUObject(this, &AABCharacter::AttackCheck); //델리게이트에 전달

	//HP가 0이 됐을 때 호출될 OnHPIsZero 델리게이트에 람다식을 정의한다
	CharacterStat->OnHPIsZero.AddLambda([this]() -> void{
		ABLOG(Warning, TEXT("OnHPIsZero"));
		ABAnim->SetDeadAnim();
		SetActorEnableCollision(false);
	});

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
	case EControlMode::NPC:
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true; //이동 방향에 따라 자연스럽게 회전하게 된다
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 480.0f, 0.0f);
		break;
	}
}

void AABCharacter::PossessedBy(AController *NewController)
{
	Super::PossessedBy(NewController);
	if (IsPlayerControlled()) //플레이어의 캐릭터인 경우
	{
		SetControlMode(EControlMode::DIABLO);
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}
	else //플레이어의 캐릭터가 아닌 경우 (NPC인 경우)
	{
		SetControlMode(EControlMode::NPC);
		GetCharacterMovement()->MaxWalkSpeed = 300.0f; //플레이어보다 속도를 느리게 제한한다
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
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AABCharacter::Attack); //어택은 직접 구현한거니까 AABCharacter 상속

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

void AABCharacter::Attack()
{
	if (IsAttacking) //이미 공격을 하고있을때 추가로 마우스를 누를 경우
	{
		//ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo)); //CurrentCombo가 1~MaxCombo 사이에 있는지 체크
		if (CanNextCombo) //다음 콤보로 넘어갈 수 있는 상황이면 IsComboInputOn을 true로 바꿔서 다음 섹션으로 넘어감
		{
			IsComboInputOn = true;
		}
		return;
	}
	else //각 섹션에서 처음 마우스를 누를 경우
	{
		ABCHECK(CurrentCombo == 0); //공격중이 아니고 CurrentCombo가 0이 아니면 종료
		AttackStartComboState(); //CurrentCombo가 0일경우 콤보 시작

		//AnimInstance에서 몽타주를 연동시키고 나서 Attack()함수 구현함
		//캐릭터의 AnimInstance를 가져와서 AnimInstance의 내가 구현한 PlayAttackMontage() 함수를 호출함
		ABAnim->PlayAttackMontage();
		ABAnim->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}


}

//콤보 공격 부분... 안됨

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
	//CurrentCombo가 0과 MaxCombo-1 사이에 있는지 체크한다
	ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo); //Clamp를 사용하여 CurrentCombo+1을 (1, MaxCombo)사이로 제한시킨다
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
	FCollisionQueryParams Params(NAME_None, false, this); //무시할 탐색 목록에 본인 추가
	bool bResult = GetWorld()->SweepSingleByChannel
	(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * 200.0f, //도형의 탐색영역, 액터 시선방향으로 200cm
		FQuat::Identity, //회전값 기본
		ECollisionChannel::ECC_GameTraceChannel2, //내가 설정한 Attack 프리셋이 ECC_GameTraceChannel 2번을 배정받음 (Config 폴더의 DefaultEngine.ini에서 확인 가능)
		FCollisionShape::MakeSphere(50.0f), //탐지에 사용할 50cm 반지름의 구체
		Params //탐색에서 무시할 객체
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
			ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.Actor->GetName()); //로그창에서 확인
			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(CharacterStat->GetAttack(), DamageEvent, GetController(), this); //감지된 액터에 데미지를 전달했다 -> todo: 피해입은 액터에 관련 로직 작성
			//데미지 양, 데미지 이벤트, 가해자 컨트롤러, 가해자 액터
		}
	}
	
}

float AABCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& 
	DamageEvent, class AController *EventInstigator, AActor *DamageCauser)
{
	//데미지 양, 데미지 이벤트, 가해자 컨트롤러, 가해자 액터
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	ABLOG(Warning, TEXT("Actor %s took Damage : %f"), *GetName(), FinalDamage);
	CharacterStat->SetDamage(FinalDamage); //캐릭터 스탯의 HP에서 FinalDamage만큼 데미지를 입히는 함수
	return FinalDamage;
}