// Fill out your copyright notice in the Description page of Project Settings.

#include "ABAnimInstance.h"

UABAnimInstance::UABAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsInAir = false;
	IsDead = false;

	static ConstructorHelpers::FObjectFinder<UAnimMontage>
		ATTACK_MONTAGE(TEXT("/Game/Animations/SK_Mannequin_Skeleton_Montage.SK_Mannequin_Skeleton_Montage"));
	if (ATTACK_MONTAGE.Succeeded())
	{
		AttackMontage = ATTACK_MONTAGE.Object;
	}
	//몽타주 에셋과 관련된 명령은 항상 몽타주 에셋을 참조하기 때문에 Anim Instance의 생성자에서 몽타주 에셋을 멤버 변수에 저장해놓으면 편리하다
}

void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner(); //폰의 정보 가져옴
	if (!::IsValid(Pawn))
	{
		return;
	}
	if (!IsDead)
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size(); //폰의 속력값을 가져온 폰의 정보로 업데이트함
		auto Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			IsInAir = Character->GetMovementComponent()->IsFalling(); //IsFalling()함수로 폰이 현재 점프 중인지 판별한 후, IsInAir변수와 일치하도록 해준다
		}
	}
}

void UABAnimInstance::PlayAttackMontage()
{
	//if (!Montage_IsPlaying(AttackMontage)) //AttackMontage를 재생하고 있지 않다면
	//{
	//	Montage_Play(AttackMontage, 1.0f); //AttackMontage 재생
	//}

	ABCHECK(!IsDead);
	Montage_Play(AttackMontage, 1.0f); //델리게이트에 의해 공격의 시작과 종료가 감지되므로 더 이상 Montage_IsPlaying()함수는 필요없다
}


void UABAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	ABCHECK(!IsDead);
	if (!Montage_IsPlaying(AttackMontage)) //어택 몽타주를 재생하고 있지 않다면 종료
	{
		//UE_LOG(LogClass, Warning, TEXT("Montage is not playing"));
		return;
	}
	//ABCHECK(Montage_IsPlaying(AttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage); //전달받은 섹션으로 넘어감 (여기선 다음 Attack 섹션)
}

void UABAnimInstance::AnimNotify_AttackHitCheck() //노티파이를 언리얼 에디터에서 배치하면 해당 노티파이가 호출될 때, 'AnimNotify_노티파이명'이라는 멤버함수를 찾아서 호출한다
{
	OnAttackHitCheck.Broadcast(); //멀티캐스트 델리게이트에 등록된 모든 함수를 호출하는 명령 함수
}

void UABAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

FName UABAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	//if (!FMath::IsWithinInclusive<int32>(Section, 1, 4)) //section이 1~4 사이가 아니면 return (현재 콤보 카운트가 4까지만 있으므로)
	//{
	//	return NAME_None;
	//}

	ABCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);

	return FName(*FString::Printf(TEXT("Attack%d"), Section)); //Attack+Section 이름을 반환
}