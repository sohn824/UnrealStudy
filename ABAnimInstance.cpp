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
	//��Ÿ�� ���°� ���õ� ����� �׻� ��Ÿ�� ������ �����ϱ� ������ Anim Instance�� �����ڿ��� ��Ÿ�� ������ ��� ������ �����س����� ���ϴ�
}

void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner(); //���� ���� ������
	if (!::IsValid(Pawn))
	{
		return;
	}
	if (!IsDead)
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size(); //���� �ӷ°��� ������ ���� ������ ������Ʈ��
		auto Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			IsInAir = Character->GetMovementComponent()->IsFalling(); //IsFalling()�Լ��� ���� ���� ���� ������ �Ǻ��� ��, IsInAir������ ��ġ�ϵ��� ���ش�
		}
	}
}

void UABAnimInstance::PlayAttackMontage()
{
	//if (!Montage_IsPlaying(AttackMontage)) //AttackMontage�� ����ϰ� ���� �ʴٸ�
	//{
	//	Montage_Play(AttackMontage, 1.0f); //AttackMontage ���
	//}

	ABCHECK(!IsDead);
	Montage_Play(AttackMontage, 1.0f); //��������Ʈ�� ���� ������ ���۰� ���ᰡ �����ǹǷ� �� �̻� Montage_IsPlaying()�Լ��� �ʿ����
}


void UABAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	ABCHECK(!IsDead);
	if (!Montage_IsPlaying(AttackMontage)) //���� ��Ÿ�ָ� ����ϰ� ���� �ʴٸ� ����
	{
		//UE_LOG(LogClass, Warning, TEXT("Montage is not playing"));
		return;
	}
	//ABCHECK(Montage_IsPlaying(AttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage); //���޹��� �������� �Ѿ (���⼱ ���� Attack ����)
}

void UABAnimInstance::AnimNotify_AttackHitCheck() //��Ƽ���̸� �𸮾� �����Ϳ��� ��ġ�ϸ� �ش� ��Ƽ���̰� ȣ��� ��, 'AnimNotify_��Ƽ���̸�'�̶�� ����Լ��� ã�Ƽ� ȣ���Ѵ�
{
	OnAttackHitCheck.Broadcast(); //��Ƽĳ��Ʈ ��������Ʈ�� ��ϵ� ��� �Լ��� ȣ���ϴ� ��� �Լ�
}

void UABAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

FName UABAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	//if (!FMath::IsWithinInclusive<int32>(Section, 1, 4)) //section�� 1~4 ���̰� �ƴϸ� return (���� �޺� ī��Ʈ�� 4������ �����Ƿ�)
	//{
	//	return NAME_None;
	//}

	ABCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);

	return FName(*FString::Printf(TEXT("Attack%d"), Section)); //Attack+Section �̸��� ��ȯ
}